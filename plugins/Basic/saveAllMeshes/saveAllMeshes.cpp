#ifndef SAVEALLMESHES_CPP
#define SAVEALLMESHES_CPP

#include "Doppelganger/pluginCommon.h"

#include "Doppelganger/Util/filesystem.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/encodeBinDataToBase64.h"

#include <string>
#include <fstream>

#include "igl/writeOBJ.h"
#include "igl/writePLY.h"
#include "igl/writeSTL.h"
#include "igl/writeWRL.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Doppelganger/Util/stb_image.h"
#include "Doppelganger/Util/stb_image_write.h"

namespace
{
	void writeTextureToFile(const fs::path &texPath, const Doppelganger::TriangleMesh::Texture &texture)
	{
		const int width = texture.texData_.cols();
		const int height = texture.texData_.rows();
		const int channels = STBI_rgb_alpha;
		stbi_flip_vertically_on_write(true);

		std::vector<unsigned char> image(width * height * channels);
		union
		{
			uint32_t uint_;
			unsigned char uchar_[4];
		} u;
		for (int h = 0; h < height; ++h)
		{
			for (int w = 0; w < width; ++w)
			{
				u.uint_ = texture.texData_(h, w);
				for (int rgba = 0; rgba < channels; ++rgba)
				{
					image.at((h * width + w) * channels + rgba) = u.uchar_[rgba];
				}
			}
		}

		if (texture.fileFormat_ == "jpg" || texture.fileFormat_ == "jpeg")
		{
			// currently, quality == 90
			stbi_write_jpg(texPath.string().c_str(), width, height, channels, &(image[0]), 90);
		}
		else if (texture.fileFormat_ == "png")
		{
			stbi_write_png(texPath.string().c_str(), width, height, channels, &(image[0]), sizeof(unsigned char) * width * channels);
		}
		else if (texture.fileFormat_ == "bmp")
		{
			stbi_write_bmp(texPath.string().c_str(), width, height, channels, &(image[0]));
		}
		else if (texture.fileFormat_ == "tga")
		{
			stbi_write_tga(texPath.string().c_str(), width, height, channels, &(image[0]));
		}
		// else if (texture.fileFormat == "hdr")
		// {
		// 	stbi_write_hdr(fileName.c_str(), width, height, channels, &(image[0]));
		// }
	}
}

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	for (const auto &UUID : parameter.at("meshes"))
	{
		std::string targetMeshPath("/meshes/");
		targetMeshPath += UUID.get<std::string>();
		ptrStrArrayRoom.push_back(targetMeshPath);
	}
	ptrStrArrayRoom.push_back("/output");
	ptrStrArrayRoom.push_back("/dataDir");
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
	// 	"format": "obj"|"ply"|"stl"|"wrl",
	// 	"meshes": [
	// 	 "UUID-A",
	// 	 "UUID-B",
	//   ...
	//  ]
	// }

	// [OUT]
	// <when configRoom.at("output").at("type") == "download">
	// response = {
	// 	"meshes" : {
	// 	 "<meshUUID-A>" : {
	//    "mesh": {
	//     fileName: fileName,
	//     base64Str: base64-encoded data,
	//     format: "obj"|"ply"|"stl"|"wrl"
	//    },
	//    "material" (optional): {
	//     fileName: fileName,
	//     base64Str: base64-encoded data,
	//     format: "mtl"
	//    },
	//    "texture" (optional): {
	//     fileName: fileName,
	//     base64Str: base64-encoded data,
	//     format: "jpg"|"jpeg"|"png"|"bmp"|"tga"
	//    }
	//   }
	//  }
	// }
	// <when configRoom.at("output").at("type") == "storage">
	// response = {
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json response = nlohmann::json::object();

	const bool saveToLocal = (configRoom.at("output").at("type").get<std::string>() == "storage");
	const std::string meshFormat = parameter.at("format").get<std::string>();

	if (!saveToLocal)
	{
		response["meshes"] = nlohmann::json::object();
	}

	for (const auto &UUID : parameter.at("meshes"))
	{
		const std::string meshUUID = UUID.get<std::string>();
		const Doppelganger::TriangleMesh mesh = configRoom.at("meshes").at(meshUUID).get<Doppelganger::TriangleMesh>();

		////
		// write file(s)
		// path to directory
		fs::path directoryPath;
		if (saveToLocal)
		{
			directoryPath = fs::path(configRoom.at("dataDir").get<std::string>());
			directoryPath.append("output");
		}
		else
		{
			directoryPath = fs::temp_directory_path();
		}
		// path to mesh
		fs::path meshFilePath;
		{
			std::string meshFileName(mesh.name_);
			meshFileName += ".";
			meshFileName += meshFormat;
			meshFilePath = directoryPath;
			meshFilePath.append(meshFileName);
		}
		// path to mtl (this is only used when writing to OBJ and the mesh has texture(s))
		fs::path mtlFilePath;
		if (meshFormat == "obj" && mesh.textures_.size() > 0 && mesh.TC_.rows() > 0)
		{
			std::string mtlFileName(mesh.name_);
			mtlFileName += ".mtl";
			mtlFilePath = directoryPath;
			mtlFilePath.append(mtlFileName);
		}
		// path to texture (this is only used when the mesh has texture(s))
		fs::path textureFilePath;
		if (mesh.textures_.size() > 0 && mesh.TC_.rows() > 0)
		{
			std::string textureFileName(mesh.textures_.at(0).fileName_);
			textureFileName += ".";
			textureFileName += mesh.textures_.at(0).fileFormat_;
			textureFilePath = directoryPath;
			textureFilePath.append(textureFileName);
		}

		{
			// apply matrixWorld
			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> exportV;
			exportV = mesh.V_ * mesh.matrixWorld_.transpose().block(0, 0, 3, 3);
			exportV.rowwise() += mesh.matrixWorld_.transpose().block<1, 3>(3, 0, 1, 3);

			if (meshFormat == "obj")
			{
				// write OBJ
				igl::writeOBJ(meshFilePath.string(), exportV, mesh.F_, mesh.VC_, mesh.VN_, mesh.F_, mesh.TC_, mesh.FTC_);

				if (!mtlFilePath.empty())
				{
					{
						// we explicitly append header if the mesh has valid texture
						std::vector<unsigned char> byteMesh;
						std::ifstream ifs(meshFilePath.string(), std::ios::in | std::ios::binary);
						ifs.seekg(0, ifs.end);
						unsigned int length = static_cast<int>(ifs.tellg());
						std::stringstream objTexHeader;
						objTexHeader << "mtllib ";
						objTexHeader << mesh.name_;
						objTexHeader << ".mtl\n";
						objTexHeader << "usemtl material0\n";

						byteMesh.resize(length + objTexHeader.str().length());
#if defined(_WIN64)
						sprintf_s(reinterpret_cast<char *>(&byteMesh[0]), byteMesh.size(), "%s", objTexHeader.str().c_str());
#elif defined(__APPLE__)
						sprintf(reinterpret_cast<char *>(&byteMesh[0]), "%s", objTexHeader.str().c_str());
#elif defined(__linux__)
						sprintf(reinterpret_cast<char *>(&byteMesh[0]), "%s", objTexHeader.str().c_str());
#endif
						ifs.seekg(0, ifs.beg);
						ifs.read(reinterpret_cast<char *>(&byteMesh[0] + objTexHeader.str().length()), length);
						ifs.close();

						std::ofstream ofs(meshFilePath.string(), std::ios::out | std::ios::binary);
						ofs.write(reinterpret_cast<char *>(&byteMesh[0]), byteMesh.size());
						ofs.close();
					}

					// .mtl file
					{
						std::string mtl("newmtl material0\nKa 1.000000 1.000000 1.000000\nKd 1.000000 1.000000 1.000000\nKs 0.000000 0.000000 0.000000\nTr 1.000000\nillum 1\nNs 0.000000\nmap_Kd ");
						mtl += textureFilePath.filename().string();
						mtl += "\n";
						std::ofstream ofsMtl(mtlFilePath.string(), std::ios::out | std::ios::binary);
						ofsMtl.write(mtl.c_str(), mtl.size());
						ofsMtl.close();
					}

					// texture file
					writeTextureToFile(textureFilePath, mesh.textures_.at(0));
				}
			}
			else if (meshFormat == "ply")
			{
				Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> outVC255 = (mesh.VC_ * 255.0).array().round().template cast<unsigned char>();

				const bool convertFTCToTC = (mesh.F_.rows() == mesh.FTC_.rows());
				if (convertFTCToTC)
				{
					Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> outTC;
					// convert FTC+TC to outTC
					outTC.resize(exportV.rows(), 2);
					for (int f = 0; f < mesh.FTC_.rows(); ++f)
					{
						for (int fv = 0; fv < mesh.FTC_.cols(); ++fv)
						{
							outTC.row(mesh.F_(f, fv)) = mesh.TC_.row(mesh.FTC_(f, fv));
						}
					}

					// possibly, we need to support meshlab-style properties??
					igl::writePLY(meshFilePath.string(), exportV, mesh.F_, mesh.VN_, outTC, outVC255, ((exportV.rows() == outVC255.rows()) ? std::vector<std::string>({"red", "green", "blue"}) : std::vector<std::string>()));
				}
				else
				{
					igl::writePLY(meshFilePath.string(), exportV, mesh.F_, mesh.VN_, mesh.TC_, outVC255, ((exportV.rows() == outVC255.rows()) ? std::vector<std::string>({"red", "green", "blue"}) : std::vector<std::string>()));
				}

				// texture file
				if (!textureFilePath.empty())
				{
					writeTextureToFile(textureFilePath, mesh.textures_.at(0));
				}
			}
			else if (meshFormat == "stl")
			{
				igl::writeSTL(meshFilePath.string(), exportV, mesh.F_, mesh.FN_);
			}
			else if (meshFormat == "wrl")
			{
				if (mesh.VC_.rows() == exportV.rows())
				{
					igl::writeWRL(meshFilePath.string(), exportV, mesh.F_, mesh.VC_);
				}
				else
				{
					igl::writeWRL(meshFilePath.string(), exportV, mesh.F_);
				}
			}
		}

		if (saveToLocal)
		{
			// open a directory that containing filePath
			// open output directory.
			std::stringstream cmd;
#if defined(_WIN64)
			cmd << "start \"\" \"";
#elif defined(__APPLE__)
			cmd << "open \"";
#elif defined(__linux__)
			cmd << "xdg-open \"";
#endif
			cmd << directoryPath.string();
			cmd << "\"";
			system(cmd.str().c_str());
		}
		else
		{
			////
			// load temporary file(s) to memory
			std::vector<unsigned char> byteMesh, byteMtl, byteTex;
			const auto loadFileIntoBin = [](const fs::path &path, std::vector<unsigned char> &bytes)
			{
				if (fs::exists(path))
				{
					std::ifstream ifs(path.string(), std::ios::in | std::ios::binary);
					ifs.seekg(0, ifs.end);
					const unsigned int length = static_cast<unsigned int>(ifs.tellg());
					bytes.resize(length);

					ifs.seekg(0, ifs.beg);
					ifs.read(reinterpret_cast<char *>(&bytes[0]), length);
					ifs.close();
					fs::remove(path);
				}
			};
			// mesh/mtl/texture
			loadFileIntoBin(meshFilePath, byteMesh);
			loadFileIntoBin(mtlFilePath, byteMtl);
			loadFileIntoBin(textureFilePath, byteTex);

			const auto generateJsonObj = [](const std::string &fileName, const std::vector<unsigned char> &bytes, const std::string &fileFormat)
			{
				nlohmann::json json = nlohmann::json::object();
				json["fileName"] = fileName;
				json["base64Str"] = Doppelganger::Util::encodeBinDataToBase64(bytes);
				json["format"] = fileFormat;
				return json;
			};
			nlohmann::json meshJson = nlohmann::json::object();
			{
				meshJson["mesh"] = generateJsonObj(mesh.name_, byteMesh, meshFormat);
				if (byteMtl.size() > 0)
				{
					meshJson["material"] = generateJsonObj(mtlFilePath.filename().stem().string(), byteMtl, std::string("mtl"));
				}
				if (byteTex.size() > 0)
				{
					meshJson["texture"] = generateJsonObj(mesh.textures_.at(0).fileName_, byteTex, mesh.textures_.at(0).fileFormat_);
				}
			}

			response.at("meshes")[meshUUID] = meshJson;
		}
	}

	if (!saveToLocal)
	{
		// write result
		writeJSONToChar(responseChar, response);
	}
}

#endif
