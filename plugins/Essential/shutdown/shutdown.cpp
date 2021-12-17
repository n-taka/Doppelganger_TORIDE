#ifndef SHUTDOWN_CPP
#define SHUTDOWN_CPP

#if defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define DLLEXPORT __attribute__((visibility("default")))
#elif defined(__linux__)
#define DLLEXPORT __attribute__((visibility("default")))
#endif

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/Core.h"
#include "Doppelganger/Room.h"

#include <string>

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
	// }

	// create response/broadcast
	response = nlohmann::json::object();
	broadcast = nlohmann::json::object();

	// we remove logs directory (graceful shutdown)
	//   by removing this directory, we can easily erase log by core and all rooms
	fs::remove_all(room->core->config.at("log").at("dir").get<std::string>());

	// graceful shutdown
	room->core->ioc.stop();
}

#endif
