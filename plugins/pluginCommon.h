#ifndef PLUGINCOMMON_H
#define PLUGINCOMMON_H

#if defined(_WIN64)
#define DLLEXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define DLLEXPORT __attribute__((visibility("default")))
#elif defined(__linux__)
#define DLLEXPORT __attribute__((visibility("default")))
#endif

#include <string>
#include <nlohmann/json.hpp>

namespace
{
	std::vector<std::string> allocated;
}

void writeJSONToChar(
	char *&dest,
	const nlohmann::json &content)
{
	allocated.push_back(content.dump(-1, ' ', true));
	dest = allocated.back().data();
}

#endif
