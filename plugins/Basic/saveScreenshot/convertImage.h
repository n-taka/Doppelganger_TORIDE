#ifndef CONVERT_IMAGE_H
#define CONVERT_IMAGE_H

#include <string>
#include <nlohmann/json.hpp>
#include "Doppelganger/Util/encodeBinDataToBase64.h"
#include "Doppelganger/Util/decodeBase64ToEigenMatrix.h"

using Image = struct Image_
{
    std::string name;
    std::string format;
    std::string fileBase64Str;
    int width;
    int height;
    int channels;
    std::vector<unsigned char> pixels;
};

// note: imageAsLayers is only works with converting into psd.
void convertImage(
    std::vector<Image> &images,
    const std::string &formatToBeSaved,
    const nlohmann::json &configRoom,
    std::vector<Image> &convertedImages);

#endif
