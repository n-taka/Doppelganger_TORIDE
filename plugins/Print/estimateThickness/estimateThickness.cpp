#ifndef ESTIMATETHICKNESS_CPP
#define ESTIMATETHICKNESS_CPP

#include "Doppelganger/pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/storeHistory.h"

#include <string>
#include <vector>

#include "igl/per_vertex_normals.h"
#include "igl/embree/shape_diameter_function.h"

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/history");
	for (const auto &UUID : parameter.at("meshes"))
	{
		std::string targetMeshPath("/meshes/");
		targetMeshPath += UUID.get<std::string>();
		ptrStrArrayRoom.push_back(targetMeshPath);
	}
	writeJSONToChar(ptrStrArrayRoomChar, ptrStrArrayRoom);
}

void pluginProcess(
	const char *&configCoreChar,
	const char *&configRoomChar,
	const char *&parameterChar,
	char *&configCorePatchChar,
	char *&configRoomPatchChar,
	char *&responseChar,
	char *&broadcastChar)
{
	////
	// [IN]
	// parameters = {
	// 	"meshes": [
	// 	 "UUID-A",
	// 	 "UUID-B",
	//   ...
	//  ],
	//  "acceptableThickness": double for minimum acceptable thickness,
	//  "sampleCount": #samples for shape diameter calculation
	// }

	// [OUT]
	// response = {
	// }
	// broadcast = {
	// 	"meshes" : {
	//    "<meshUUID>": JSON object that represents the mesh (with modified vertex colors)
	//  }
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json configRoomPatch = nlohmann::json::object();
	nlohmann::json broadcast = nlohmann::json::object();
	broadcast["meshes"] = nlohmann::json::object();
	configRoomPatch["meshes"] = nlohmann::json::object();

	const double acceptableThickness = parameter.at("acceptableThickness").get<double>();
	const int sampleCount = parameter.at("sampleCount").get<int>();

	for (const auto &UUID : parameter.at("meshes"))
	{
		const std::string meshUUID = UUID.get<std::string>();
		Doppelganger::TriangleMesh mesh = configRoom.at("meshes").at(meshUUID).get<Doppelganger::TriangleMesh>();

		nlohmann::json diff = nlohmann::json::object();
		nlohmann::json diffInv = nlohmann::json::object();
		diff["meshes"] = nlohmann::json::object();
		diffInv["meshes"] = nlohmann::json::object();

		// store current mesh
		diffInv.at("meshes")[mesh.UUID_] = configRoom.at("meshes").at(meshUUID);

		// estimate thickness
		{
			igl::embree::EmbreeIntersector ei;
			ei.init(mesh.V_.template cast<float>(), mesh.F_, true);

			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> P;
			P = mesh.V_;
			igl::per_vertex_normals(mesh.V_, mesh.F_, mesh.VN_);
			Eigen::Matrix<double, Eigen::Dynamic, 1> S;
			igl::embree::shape_diameter_function(ei, P, mesh.VN_, sampleCount, S);

			mesh.VC_.resizeLike(mesh.V_);
			for (int v = 0; v < mesh.VC_.rows(); ++v)
			{
				if (S(v, 0) < acceptableThickness)
				{
					// orange
					mesh.VC_.row(v) << (245.0 / 255.0),
						(108.0 / 255.0),
						(10.0 / 255.0);
				}
				else
				{
					// gray
					mesh.VC_.row(v) << (220.0 / 255.0),
						(220.0 / 255.0),
						(220.0 / 255.0);
				}
			}
		}

		// store modified mesh
		diff.at("meshes")[mesh.UUID_] = mesh;
		configRoomPatch.at("meshes")[mesh.UUID_] = mesh;

		// update broadcast
		nlohmann::json meshJsonf;
		Doppelganger::to_json(meshJsonf, mesh, true);
		broadcast.at("meshes")[mesh.UUID_] = meshJsonf;

		// store history
		configRoomPatch["history"] = nlohmann::json::object();
		Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));
	}

	// write result
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
	writeJSONToChar(broadcastChar, broadcast);
}

#endif
