#ifndef TOGGLEMESHVISIBILITY_CPP
#define TOGGLEMESHVISIBILITY_CPP

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
#include <sstream>
#include <mutex>

extern "C" DLLEXPORT void pluginProcess(const std::shared_ptr<Doppelganger::Room> &room, const nlohmann::json &parameters, nlohmann::json &response, nlohmann::json &broadcast)
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
	// }
	// broadcast = {
	// 	"meshes" : {
	//    "<meshUUID>": JSON object that represents the loaded mesh
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

		// toggle visibility
		for (const auto &UUID : parameters.at("meshes"))
		{
			const std::string &meshUUID = UUID.get<std::string>();
			const std::shared_ptr<Doppelganger::triangleMesh> &mesh = room->meshes.at(meshUUID);
			diffInv.at("meshes")[meshUUID] = mesh->dumpToJson(false);
			diffInv.at("meshes").at(meshUUID)["remove"] = false;
			mesh->visibility = !mesh->visibility;
			diff.at("meshes")[meshUUID] = mesh->dumpToJson(false);
			diff.at("meshes").at(meshUUID)["remove"] = false;
			broadcast.at("meshes")[meshUUID] = mesh->dumpToJson(true);
		}
	}
	room->storeHistory(diff, diffInv);
}

#endif
