#ifndef CONVERT_IMAGE_CPP
#define CONVERT_IMAGE_CPP

#include "convertImage.h"

#include <vector>
#include <fstream>
#include <sstream>

#include <boost/beast/core/detail/base64.hpp>
#include "Doppelganger/Util/encodeBinDataToBase64.h"
#include "Doppelganger/Util/filesystem.h"
#include "Doppelganger/Util/uuid.h"
#include "Doppelganger/Util/log.h"

#include "Psd/Psd.h"
#include "Psd/PsdMallocAllocator.h"
#include "Psd/PsdNativeFile.h"
#include "Psd/PsdExport.h"
#include "Psd/PsdExportDocument.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Doppelganger/Util/stb_image.h"
#include "Doppelganger/Util/stb_image_write.h"

#include <iostream>

void convertImage(
    std::vector<Image> &images,
    const std::string &formatToBeSaved,
    const bool &imageAsLayers,
    const std::string &screenshotFileName,
    const nlohmann::json &configRoom,
    std::vector<Image> &convertedImages)
{
    // load pixels
    for (auto &image : images)
    {
        std::vector<unsigned char> binData;
        const size_t len = boost::beast::detail::base64::decoded_size(image.fileBase64Str.size());
        binData.resize(len);
        const std::pair<size_t, size_t> lenWrittenRead = boost::beast::detail::base64::decode(&(binData[0]), image.fileBase64Str.data(), image.fileBase64Str.size());
        const size_t binSize = lenWrittenRead.first;

        unsigned char *binImage = stbi_load_from_memory(&(binData[0]), binSize, &image.width, &image.height, &image.channels, STBI_rgb_alpha);

        image.pixels = std::vector<unsigned char>(binImage, binImage + (image.width * image.height * image.channels * sizeof(unsigned char)));

        stbi_image_free(binImage);
    }

    // note: mergeAll is only works with converting into psd.
    //       for other formats, merge operation is done on clients.
    if (formatToBeSaved == "psd")
    {
        if (imageAsLayers)
        {
            fs::path filePath = fs::temp_directory_path();
            filePath /= Doppelganger::Util::uuid("DoppelgangerTmpFile-");
            filePath += ".";
            filePath += formatToBeSaved;

            // open the file
            psd::MallocAllocator allocator;
            psd::NativeFile file(&allocator);
            // try opening the file. if it fails, bail out.
            if (!file.OpenWrite(filePath.wstring().c_str()))
            {
                std::stringstream ss;
                ss << "We cannot open ";
                ss << filePath.string();
                Doppelganger::Util::log(ss.str(), "ERROR", configRoom);
                return;
            }

            if (images.size() > 0)
            {
                // construct document
                //    * here, we assume the document is RGB
                psd::ExportDocument *document = psd::CreateExportDocument(&allocator, images.at(0).width, images.at(0).height, 8u, psd::exportColorMode::RGB);

                for (const auto &image : images)
                {
                    if (image.format != formatToBeSaved)
                    {
                        const unsigned int layer = psd::AddLayer(document, &allocator, image.name.c_str());

                        std::vector<unsigned char> red, green, blue, alpha;
                        red.resize(image.width * image.height, 0);
                        green.resize(image.width * image.height, 0);
                        blue.resize(image.width * image.height, 0);
                        alpha.resize(image.width * image.height, std::numeric_limits<unsigned char>::max());
                        for (int h = 0; h < image.height; ++h)
                        {
                            for (int w = 0; w < image.width; ++w)
                            {
                                for (int rgba = 0; rgba < image.channels; ++rgba)
                                {
                                    const int position = h * image.width + w;
                                    const int idx = h * image.width * image.channels + w * image.channels + rgba;
                                    const unsigned char &channelPixel = image.pixels.at(idx);
                                    switch (rgba)
                                    {
                                    case 0:
                                        red.at(position) = channelPixel;
                                        break;
                                    case 1:
                                        green.at(position) = channelPixel;
                                        break;
                                    case 2:
                                        blue.at(position) = channelPixel;
                                        break;
                                    case 3:
                                        alpha.at(position) = channelPixel;
                                        break;
                                    }
                                }
                            }
                        }

                        psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::RED, 0, 0, image.width, image.height, red.data(), psd::compressionType::ZIP);
                        psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::GREEN, 0, 0, image.width, image.height, green.data(), psd::compressionType::ZIP);
                        psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::BLUE, 0, 0, image.width, image.height, blue.data(), psd::compressionType::ZIP);
                        psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::ALPHA, 0, 0, image.width, image.height, alpha.data(), psd::compressionType::ZIP);
                    }
                }

                WriteDocument(document, &allocator, &file);
                DestroyExportDocument(document, &allocator);
                file.Close();

                // for psd with layers, we don't care about width,height,channels
                Image cImage;
                cImage.name = screenshotFileName;
                cImage.format = formatToBeSaved;

                // todo handle some errors
                //   e.g. partial read
                std::ifstream ifs(filePath.string(), std::ios::in | std::ios::binary);
                int length;
                ifs.seekg(0, ifs.end);
                length = static_cast<int>(ifs.tellg());
                std::vector<unsigned char> binData;
                binData.resize(length);
                ifs.seekg(0, ifs.beg);
                ifs.read(reinterpret_cast<char *>(binData.data()), length);
                ifs.close();

                cImage.fileBase64Str = Doppelganger::Util::encodeBinDataToBase64(binData);

                convertedImages.push_back(cImage);
            }
        }
        else
        {
            for (const auto &image : images)
            {
                if (image.format != formatToBeSaved)
                {
                    // for consistency, we write to file and then load it into buffer
                    //   cf. stb image doesn't have to_mem for jpg
                    fs::path filePath = fs::temp_directory_path();
                    filePath /= Doppelganger::Util::uuid("DoppelgangerTmpFile-");
                    filePath += ".";
                    filePath += formatToBeSaved;

                    // open the file
                    psd::MallocAllocator allocator;
                    psd::NativeFile file(&allocator);
                    // try opening the file. if it fails, bail out.
                    if (!file.OpenWrite(filePath.wstring().c_str()))
                    {
                        std::stringstream ss;
                        ss << "We cannot open ";
                        ss << filePath.string();
                        Doppelganger::Util::log(ss.str(), "ERROR", configRoom);
                        break;
                    }

                    // construct document
                    //    * here, we assume the document is RGB
                    psd::ExportDocument *document = psd::CreateExportDocument(&allocator, image.width, image.height, 8u, psd::exportColorMode::RGB);

                    const unsigned int layer = psd::AddLayer(document, &allocator, image.name.c_str());

                    std::vector<unsigned char> red, green, blue, alpha;
                    red.resize(image.width * image.height, 0);
                    green.resize(image.width * image.height, 0);
                    blue.resize(image.width * image.height, 0);
                    alpha.resize(image.width * image.height, std::numeric_limits<unsigned char>::max());
                    for (int h = 0; h < image.height; ++h)
                    {
                        for (int w = 0; w < image.width; ++w)
                        {
                            for (int rgba = 0; rgba < image.channels; ++rgba)
                            {
                                const int position = h * image.width + w;
                                const int idx = h * image.width * image.channels + w * image.channels + rgba;
                                const unsigned char &channelPixel = image.pixels.at(idx);
                                switch (rgba)
                                {
                                case 0:
                                    red.at(position) = channelPixel;
                                    break;
                                case 1:
                                    green.at(position) = channelPixel;
                                    break;
                                case 2:
                                    blue.at(position) = channelPixel;
                                    break;
                                case 3:
                                    alpha.at(position) = channelPixel;
                                    break;
                                }
                            }
                        }
                    }

                    psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::RED, 0, 0, image.width, image.height, red.data(), psd::compressionType::ZIP);
                    psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::GREEN, 0, 0, image.width, image.height, green.data(), psd::compressionType::ZIP);
                    psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::BLUE, 0, 0, image.width, image.height, blue.data(), psd::compressionType::ZIP);
                    psd::UpdateLayer(document, &allocator, layer, psd::exportChannel::ALPHA, 0, 0, image.width, image.height, alpha.data(), psd::compressionType::ZIP);

                    WriteDocument(document, &allocator, &file);
                    DestroyExportDocument(document, &allocator);
                    file.Close();

                    Image cImage;
                    cImage.name = image.name;
                    cImage.format = formatToBeSaved;
                    cImage.width = image.width;
                    cImage.height = image.height;
                    cImage.channels = image.channels;

                    // todo handle some errors
                    //   e.g. partial read
                    std::ifstream ifs(filePath.string(), std::ios::in | std::ios::binary);
                    int length;
                    ifs.seekg(0, ifs.end);
                    length = static_cast<int>(ifs.tellg());
                    std::vector<unsigned char> binData;
                    binData.resize(length);
                    ifs.seekg(0, ifs.beg);
                    ifs.read(reinterpret_cast<char *>(binData.data()), length);
                    ifs.close();

                    cImage.fileBase64Str = Doppelganger::Util::encodeBinDataToBase64(binData);

                    convertedImages.push_back(cImage);
                }
            }
        }
    }
    else
    {
        for (const auto &image : images)
        {
            if (image.format != formatToBeSaved)
            {
                // for consistency, we write to file and then load it into buffer
                //   cf. stb image doesn't have to_mem for jpg
                fs::path filePath = fs::temp_directory_path();
                filePath /= Doppelganger::Util::uuid("DoppelgangerTmpFile-");
                filePath += ".";
                filePath += formatToBeSaved;

                if (formatToBeSaved == "jpg" || formatToBeSaved == "jpeg")
                {
                    // currently, quality == 90
                    stbi_write_jpg(filePath.string().c_str(), image.width, image.height, image.channels, &(image.pixels[0]), 90);
                }
                else if (formatToBeSaved == "png")
                {
                    stbi_write_png(filePath.string().c_str(), image.width, image.height, image.channels, &(image.pixels[0]), sizeof(unsigned char) * image.width * image.channels);
                }
                else if (formatToBeSaved == "bmp")
                {
                    stbi_write_bmp(filePath.string().c_str(), image.width, image.height, image.channels, &(image.pixels[0]));
                }
                else if (formatToBeSaved == "tga")
                {
                    stbi_write_tga(filePath.string().c_str(), image.width, image.height, image.channels, &(image.pixels[0]));
                }

                Image cImage;
                cImage.name = image.name;
                cImage.format = formatToBeSaved;
                cImage.width = image.width;
                cImage.height = image.height;
                cImage.channels = image.channels;

                // todo handle some errors
                //   e.g. partial read
                std::ifstream ifs(filePath.string(), std::ios::in | std::ios::binary);
                int length;
                ifs.seekg(0, ifs.end);
                length = static_cast<int>(ifs.tellg());
                std::vector<unsigned char> binData;
                binData.resize(length);
                ifs.seekg(0, ifs.beg);
                ifs.read(reinterpret_cast<char *>(binData.data()), length);

                cImage.fileBase64Str = Doppelganger::Util::encodeBinDataToBase64(binData);

                convertedImages.push_back(cImage);
            }
            else
            {
                convertedImages.push_back(image);
            }
        }
    }
}

#endif
