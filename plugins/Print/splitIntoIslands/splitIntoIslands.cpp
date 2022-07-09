#ifndef SPLITINTOISLANDS_CPP
#define SPLITINTOISLANDS_CPP

#include "Doppelganger/pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/uuid.h"
#include "Doppelganger/Util/storeHistory.h"

#include <string>
#include <vector>

#include "igl/facet_components.h"
#include "igl/remove_unreferenced.h"

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
	//  "dryRun": true|false
	// }

	// [OUT]
	// response = {
	//  "meshes" : {
	//   "<meshUUID>": #islands
	//  }
	// }
	// broadcast = { // if dryRun == true, broadcast is empty
	// 	"meshes" : {
	//    "<meshUUID>": JSON object that represents the loaded mesh
	//  }
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json configRoomPatch = nlohmann::json::object();
	nlohmann::json response = nlohmann::json::object();
	nlohmann::json broadcast = nlohmann::json::object();

	response["meshes"] = nlohmann::json::object();
	broadcast["meshes"] = nlohmann::json::object();
	for (const auto &UUID : parameter.at("meshes"))
	{
		const std::string meshUUID = UUID.get<std::string>();
		const Doppelganger::TriangleMesh mesh = configRoom.at("meshes").at(meshUUID).get<Doppelganger::TriangleMesh>();

		Eigen::Matrix<int, Eigen::Dynamic, 1> C;
		igl::facet_components(mesh.F_, C);
		response.at("meshes")[meshUUID] = nlohmann::json::object();
		response.at("meshes").at(meshUUID)["islandCount"] = C.maxCoeff() + 1;
		if (!parameter.at("dryRun").get<bool>())
		{
			// componentId -> triangles
			std::vector<std::vector<int>> faceIdVec;
			faceIdVec.resize(C.maxCoeff() + 1);
			for (int f = 0; f < C.rows(); ++f)
			{
				faceIdVec.at(C(f, 0)).push_back(f);
			}

			// generate and register to this room
			//   + generate broadcast message
			{
				configRoomPatch["meshes"] = nlohmann::json::object();

				// remove original mesh
				configRoomPatch.at("meshes")[mesh.UUID_] = nlohmann::json(nullptr);
				broadcast.at("meshes")[mesh.UUID_] = nlohmann::json(nullptr);

				for (int id = 0; id < faceIdVec.size(); ++id)
				{
					const std::vector<int> &faces = faceIdVec.at(id);

					// generate and assign UUID/name
					Doppelganger::TriangleMesh componentMesh;
					componentMesh.UUID_ = Doppelganger::Util::uuid("mesh-");
					componentMesh.name_ = mesh.name_;
					componentMesh.name_ += "_";
					componentMesh.name_ += std::to_string(id);

					Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> tmpF;
					tmpF.resize(faces.size(), mesh.F_.cols());
					for (int ff = 0; ff < faces.size(); ++ff)
					{
						tmpF.row(ff) = mesh.F_.row(faces.at(ff));
					}
					Eigen::Matrix<int, Eigen::Dynamic, 1> I;
					igl::remove_unreferenced(mesh.V_, tmpF, componentMesh.V_, componentMesh.F_, I);

					// todo: copy attributes
					//   * VC
					//   * UV
					//   * etc...

					configRoomPatch.at("meshes")[componentMesh.UUID_] = componentMesh;

					nlohmann::json meshJsonf;
					Doppelganger::to_json(meshJsonf, componentMesh, true);
					broadcast.at("meshes")[componentMesh.UUID_] = meshJsonf;
				}
			}

			{
				// edit history
				nlohmann::json diff = nlohmann::json::object();
				nlohmann::json diffInv = nlohmann::json::object();
				diff["meshes"] = nlohmann::json::object();
				diffInv["meshes"] = nlohmann::json::object();

				// original mesh
				diff.at("meshes")[mesh.UUID_] = nlohmann::json(nullptr);
				diffInv.at("meshes")[mesh.UUID_] = configRoom.at("meshes").at(meshUUID);
				// component meshes
				for (const auto &uuid_meshJson : configRoomPatch.at("meshes").items())
				{
					const std::string &uuid = uuid_meshJson.key();
					const nlohmann::json &meshJson = uuid_meshJson.value();
					if (!meshJson.is_null())
					{
						diff.at("meshes")[uuid] = meshJson;
						diffInv.at("meshes")[uuid] = nlohmann::json(nullptr);
					}
				}

				configRoomPatch["history"] = nlohmann::json::object();
				Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));
			}
		}
	}

	// write result
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
	writeJSONToChar(responseChar, response);
	writeJSONToChar(broadcastChar, broadcast);
}

#endif
