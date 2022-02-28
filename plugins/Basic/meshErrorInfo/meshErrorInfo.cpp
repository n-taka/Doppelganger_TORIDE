#ifndef MESHERRORINFO_CPP
#define MESHERRORINFO_CPP

#include "pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"

#include "igl/boundary_facets.h"
#include "igl/is_edge_manifold.h"
#include "igl/is_vertex_manifold.h"

#include <string>

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
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
	//  ]
	// }

	// [OUT]
	// response = {
	// 	"meshes" : {
	//   "<meshUUID>": {
	// 	  "closed" : boolean flag
	// 	  "edgeManifold" : boolean flag
	// 	  "vertexManifold" : boolean flag
	//   },
	//   ...
	//  }
	// }
	// broadcast = {
	// }

	// create empty response/broadcast
	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json response = nlohmann::json::object();

	response["meshes"] = nlohmann::json::object();
	for (const auto &UUID : parameter.at("meshes"))
	{
		const std::string meshUUID = UUID.get<std::string>();
		const Doppelganger::TriangleMesh mesh = configRoom.at("meshes").at(meshUUID).get<Doppelganger::TriangleMesh>();

		nlohmann::json errorInfo = nlohmann::json::object();
		{
			Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> BF;
			igl::boundary_facets(mesh.F_, BF);
			errorInfo["closed"] = (BF.rows() == 0);
		}
		{
			errorInfo["edgeManifold"] = igl::is_edge_manifold(mesh.F_);
		}
		{
			Eigen::Matrix<int, Eigen::Dynamic, 1> B;
			errorInfo["vertexManifold"] = igl::is_vertex_manifold(mesh.F_, B);
		}

		response.at("meshes")[meshUUID] = errorInfo;
	}

	writeJSONToChar(responseChar, response);
}

#endif
