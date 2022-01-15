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
	//     "removeLog": true|false,
	//     "removeOutput": true|false
	// }

	// [OUT]
	// response = {
	// }
	// broadcast = {
	// }

	// create response/broadcast
	response = nlohmann::json::object();
	broadcast = nlohmann::json::object();

	const bool removeLog = parameters.at("removeLog").get<bool>();
	const bool removeOutput = parameters.at("removeOutput").get<bool>();
	// we remove log/output directory
	{
		if (removeLog && removeOutput)
		{
			fs::remove_all(room->core_->dataDir);
		}
		else
		{
			if (removeLog)
			{
				fs::path logDir(room->core_->dataDir);
				logDir.append("log");
				fs::remove_all(logDir);
			}
			if (removeOutput)
			{
				fs::path outputDir(room->core_->dataDir);
				outputDir.append("output");
				fs::remove_all(outputDir);
			}
		}
	}
	for (const auto &r : room->core_->rooms)
	{
		{
			fs::path installedJson(r.second->dataDir);
			installedJson.append("installed.json");
			fs::remove_all(installedJson);
		}
		if (removeLog && removeOutput)
		{
			fs::remove_all(r.second->dataDir);
		}
		else
		{
			if (removeLog)
			{
				fs::path logDir(r.second->dataDir);
				logDir.append("log");
				fs::remove_all(logDir);
			}
			if (removeOutput)
			{
				fs::path outputDir(r.second->dataDir);
				outputDir.append("output");
				fs::remove_all(outputDir);
			}
		}
	}

	// graceful shutdown
	room->core_->ioc_.stop();
}

#endif
