#ifndef SYNCCURSOR_CPP
#define SYNCCURSOR_CPP

#if defined(_WIN32) || defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __attribute__((visibility("default")))
#endif
#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/Room.h"
#include "Doppelganger/Plugin.h"

#include <string>

extern "C" DLLEXPORT void pluginProcess(const std::shared_ptr<Doppelganger::Room> &room, const nlohmann::json &parameters, nlohmann::json &response, nlohmann::json &broadcast)
{
    ////
    // [IN]
    // parameters = {
    //  "sessionUUID": sessionUUID string,
    //  "cursor": {
    //   "dir": {
    //    "x": x corrdinate of this cursor,
    //    "y": y corrdinate of this cursor
    //   },
    //   "idx": idx for cursor icon
    //  }
    // }

	// [OUT]
	// response = {
	// }
	// broadcast = parameters

	// create empty response/broadcast
	response = nlohmann::json::object();
	broadcast = parameters;

	{
		std::lock_guard<std::mutex> lock(room->interfaceParams.mutex);
		// todo support delete operation

		std::unordered_map<std::string, Doppelganger::Room::cursorInfo> &cursors = room->interfaceParams.cursors;
		const std::string &sessionUUID = parameters.at("sessionUUID").get<std::string>();
		const double &x = parameters.at("cursor").at("dir").at("x").get<double>();
		const double &y = parameters.at("cursor").at("dir").at("y").get<double>();
		const int &idx = parameters.at("cursor").at("idx").get<int>();

		// we don't need to check timestamp for cursors
		cursors[sessionUUID] = Doppelganger::Room::cursorInfo({x, y, idx});
	}
}

#endif
