#ifndef UNDO_CPP
#define UNDO_CPP

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

extern "C" DLLEXPORT void pluginProcess(const std::shared_ptr<Doppelganger::Room> &room, const nlohmann::json &parameters, nlohmann::json &response, nlohmann::json &broadcast)
{
	////
	// [IN]
	// parameters = {
	// }

	// [OUT]
	// response = {
	// }
	// broadcast = {
	// 	"meshes" : {
	//    "<meshUUID>": JSON object that represents the loaded mesh
	//  }
	// }

	// create empty response/broadcast
	response = nlohmann::json::object();
	broadcast = nlohmann::json::object();

	// update mesh
	const int updatedIndex = std::max(0, static_cast<int>(room->editHistory.index - 1));
	if (updatedIndex != room->editHistory.index)
	{
		// do we surely need this lock...?
		std::lock_guard<std::mutex> lock(room->mutexMeshes);

		room->editHistory.index = updatedIndex;
		broadcast = room->editHistory.diffFromNext.at(room->editHistory.index);
		// update other parameters (if needed)
		//  * currently, we only store meshes.
		// update meshes
		for (const auto &uuid_mesh : broadcast.at("meshes").items())
		{
			const std::string &meshUUID = uuid_mesh.key();
			nlohmann::json &meshJson = uuid_mesh.value();
			if (meshJson.contains("remove") && meshJson.at("remove").get<bool>())
			{
				// remove
				room->meshes.erase(meshUUID);
			}
			else if (room->meshes.find(meshUUID) == room->meshes.end())
			{
				// new mesh
				std::shared_ptr<Doppelganger::triangleMesh> mesh = std::make_shared<Doppelganger::triangleMesh>(meshUUID);
				mesh->restoreFromJson(meshJson);
				room->meshes[meshUUID] = mesh;
				// this dumpToJson looks stupid, but mandatory. 
				//   Because our editHistory uses double, but the clients use float for save the amount of communication.
				//   In addition, this conversion is needed to handle faceColors
				meshJson = mesh->dumpToJson(true);
			}
			else
			{
				// existing mesh
				room->meshes[meshUUID]->restoreFromJson(meshJson);
				meshJson = room->meshes[meshUUID]->dumpToJson(true);
			}
		}
	}
}

#endif
