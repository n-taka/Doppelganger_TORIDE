#ifndef SYNCVIEWPORT_CPP
#define SYNCVIEWPORT_CPP

#include "Doppelganger/pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/extension/syncViewport");
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
	//     "timestamp": timestamp,
	//     "controls": {
	//         "target": {
	//             "value": {
	//                 "x": x-coordinate,
	//                 "y": y-coordinate,
	//                 "z": z-coordinate,
	//             }
	//         }
	//     },
	//     "camera": {
	//         "position": {
	//             "value": {
	//                 "x": x-coordinate,
	//                 "y": y-coordinate,
	//                 "z": z-coordinate,
	//             }
	//         },
	//         "up": {
	//             "value": {
	//                 "x": x-coordinate,
	//                 "y": y-coordinate,
	//                 "z": z-coordinate,
	//             }
	//         },
	//         "zoom": {
	//             "value": {
	//                 "zoom": zoom parameter,
	//             }
	//         }
	//     }
	// }

	// [OUT]
	// response = {
	// }
	// broadcast = parameters
	// * only with valid ones (i.e. if timestamp is older than the timestamp on the server, we filter such entry)

	// initialize
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);

	if (parameter.empty())
	{
		if (configRoom.contains("extension") && configRoom.at("extension").contains("syncViewport"))
		{
			// write result
			writeJSONToChar(broadcastChar, configRoom.at("extension").at("syncViewport"));
		}
	}
	else
	{
		// check timestamp
		std::int64_t timestampServer = 0;
		if (configRoom.contains(nlohmann::json::json_pointer("/extension/syncViewport/timestamp")))
		{
			timestampServer = configRoom.at(nlohmann::json::json_pointer("/extension/syncViewport/timestamp")).get<std::int64_t>();
		}

		std::int64_t timestampParameter = 0;
		if (parameter.contains(nlohmann::json::json_pointer("/timestamp")))
		{
			timestampParameter = parameter.at(nlohmann::json::json_pointer("/timestamp")).get<std::int64_t>();
		}

		// we accept only if timestampParameter is newer
		if (timestampServer <= timestampParameter)
		{
			writeJSONToChar(broadcastChar, parameter);

			nlohmann::json configRoomPatch = nlohmann::json::object();
			configRoomPatch["extension"] = nlohmann::json::object();
			configRoomPatch.at("extension")["syncViewport"] = parameter;
			writeJSONToChar(configRoomPatchChar, configRoomPatch);
		}

#if 0
		// controls
		if (parameter.contains("controls"))
		{
			nlohmann::json controlsBroadcast = nlohmann::json::object();
			const nlohmann::json &controls = parameter.at("controls");

			if (controls.contains("target"))
			{
				if (configRoom.contains(nlohmann::json::json_pointer("/extension/syncViewport/controls/target")))
				{
					prevTimestamp = configRoom.at(nlohmann::json::json_pointer("/extension/syncViewport/controls/target/timestamp")).get<std::int64_t>();
				}

				const nlohmann::json &target = controls.at("target");
				const std::int64_t timestamp = target.at("timestamp").get<std::int64_t>();
				if (prevTimestamp <= timestamp)
				{
					controlsBroadcast["target"] = target;
				}
			}

			if (!controlsBroadcast.empty())
			{
				broadcast["controls"] = controlsBroadcast;
			}
		}

		// camera
		if (parameter.contains("camera"))
		{
			nlohmann::json cameraBroadcast = nlohmann::json::object();
			const nlohmann::json &camera = parameter.at("camera");

			if (camera.contains("position"))
			{
				std::int64_t prevTimestamp = 0;
				if (configRoom.contains(nlohmann::json::json_pointer("/extension/syncViewport/camera/position")))
				{
					prevTimestamp = configRoom.at(nlohmann::json::json_pointer("/extension/syncViewport/camera/position/timestamp")).get<std::int64_t>();
				}

				const nlohmann::json &position = camera.at("position");
				const std::int64_t timestamp = position.at("timestamp").get<std::int64_t>();
				if (prevTimestamp <= timestamp)
				{
					cameraBroadcast["position"] = position;
				}
			}
			if (camera.contains("up"))
			{
				std::int64_t prevTimestamp = 0;
				if (configRoom.contains(nlohmann::json::json_pointer("/extension/syncViewport/camera/up")))
				{
					prevTimestamp = configRoom.at(nlohmann::json::json_pointer("/extension/syncViewport/camera/up/timestamp")).get<std::int64_t>();
				}

				const nlohmann::json &up = camera.at("up");
				const std::int64_t timestamp = up.at("timestamp").get<std::int64_t>();
				if (prevTimestamp <= timestamp)
				{
					cameraBroadcast["up"] = up;
				}
			}
			if (camera.contains("zoom"))
			{
				std::int64_t prevTimestamp = 0;
				if (configRoom.contains(nlohmann::json::json_pointer("/extension/syncViewport/camera/zoom")))
				{
					prevTimestamp = configRoom.at(nlohmann::json::json_pointer("/extension/syncViewport/camera/zoom/timestamp")).get<std::int64_t>();
				}

				const nlohmann::json &zoom = camera.at("zoom");
				const std::int64_t timestamp = zoom.at("timestamp").get<std::int64_t>();
				if (prevTimestamp <= timestamp)
				{
					cameraBroadcast["zoom"] = zoom;
				}
			}
			if (!cameraBroadcast.empty())
			{
				broadcast["camera"] = cameraBroadcast;
			}
		}

		// write result
		if (!broadcast.empty())
		{
		}
#endif
	}
}

#endif
