#ifndef HOLLOWMESH_CPP
#define HOLLOWMESH_CPP

#if defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define DLLEXPORT __attribute__((visibility("default")))
#elif defined(__linux__)
#define DLLEXPORT __attribute__((visibility("default")))
#endif

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/Room.h"
#include "Doppelganger/triangleMesh.h"

#include <string>
#include <mutex>
#include <vector>

#include "igl/per_vertex_normals.h"
#include "igl/embree/shape_diameter_function.h"

extern "C" DLLEXPORT void pluginProcess(const std::shared_ptr<Doppelganger::Room> &room, const nlohmann::json &parameters, nlohmann::json &response, nlohmann::json &broadcast)
{
	////
	// [IN]
	// parameters = {
	// 	"meshes": [
	// 	 "UUID-A",
	// 	 "UUID-B",
	//   ...
	//  ],
	//  "acceptableThickness": double for minimum acceptable thickness
	// }

	// [OUT]
	// response = {
	// }
	// broadcast = {
	// 	"meshes" : {
	//    "<meshUUID>": JSON object that represents the mesh (with modified vertex colors)
	//  }
	// }

	// create empty response/broadcast
	response = nlohmann::json::object();
	broadcast = nlohmann::json::object();
	nlohmann::json diff = nlohmann::json::object();
	nlohmann::json diffInv = nlohmann::json::object();

	broadcast["meshes"] = nlohmann::json::object();
	diff["meshes"] = nlohmann::json::object();
	diffInv["meshes"] = nlohmann::json::object();
	{
		std::lock_guard<std::mutex> lock(room->mutexMeshes);

		const double &acceptableThickness = parameters.at("acceptableThickness").get<double>();
		const int &sampleCount = parameters.at("sampleCount").get<int>();

		// remove mesh
		for (const auto &UUID : parameters.at("meshes"))
		{
			const std::string &meshUUID = UUID.get<std::string>();
			const std::shared_ptr<Doppelganger::triangleMesh> &mesh = room->meshes.at(meshUUID);
			// store current mesh
			diffInv.at("meshes")[meshUUID] = mesh->dumpToJson(false);
			diffInv.at("meshes").at(meshUUID)["remove"] = false;

			igl::embree::EmbreeIntersector ei;
			ei.init(mesh->V.template cast<float>(), mesh->F, true);

			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> P, N;
			P = mesh->V;
			igl::per_vertex_normals(mesh->V, mesh->F, N);
			Eigen::Matrix<double, Eigen::Dynamic, 1> S;
			igl::embree::shape_diameter_function(ei, P, N, sampleCount, S);

			mesh->VC.resizeLike(mesh->V);
			for (int v = 0; v < mesh->VC.rows(); ++v)
			{
				if (S(v, 0) < acceptableThickness)
				{
					// orange
					mesh->VC.row(v) << (245.0 / 255.0),
						(108.0 / 255.0),
						(10.0 / 255.0);
				}
				else
				{
					// gray
					mesh->VC.row(v) << (220.0 / 255.0),
						(220.0 / 255.0),
						(220.0 / 255.0);
				}
			}

			broadcast["meshes"][meshUUID] = mesh->dumpToJson(true);
			broadcast["meshes"][meshUUID]["remove"] = false;
			diff["meshes"][meshUUID] = mesh->dumpToJson(false);
			diff["meshes"][meshUUID]["remove"] = false;
		}
	}
	room->storeHistory(diff, diffInv);
}

#endif
