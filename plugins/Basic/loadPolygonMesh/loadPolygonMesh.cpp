#ifndef LOADPOLYGONMESH_CPP
#define LOADPOLYGONMESH_CPP

#if defined(_WIN64)
#include <filesystem>
namespace fs = std::filesystem;
#elif defined(__APPLE__)
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;
#elif defined(__linux__)
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include "pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/Room.h"
#include "Doppelganger/triangleMesh.h"
#include "Doppelganger/Util/uuid.h"
#include "Doppelganger/Util/writeBase64ToFile.h"
#include "Doppelganger/Util/log.h"
#include "Doppelganger/Util/storeHistory.h"

#include <string>
#include <sstream>

#include "igl/readOBJ.h"
#include "igl/readPLY.h"
#include "igl/readSTL.h"
#include "igl/readWRL.h"
#include "igl/remove_duplicate_vertices.h"
#include "igl/polygon_corners.h"
#include "igl/polygons_to_triangles.h"

#include "igl/per_face_normals.h"
#include "igl/per_vertex_normals.h"

extern "C" DLLEXPORT void pluginProcess(
	const char *&configCoreChar,
	const char *&configRoomChar,
	const char *&parameterChar,
	char *&configCoreUpdateChar,
	char *&configRoomUpdateChar,
	char *&responseChar,
	char *&broadcastChar)
{
	////
	// [IN]
	// parameters = {
	// 	"mesh": {
	// 	 "name": name of this mesh (usually, filename without extension),
	// 	 "file": {
	//    "id": unique id for this file,
	//    "size": bytes of this file,
	//    "packetId": id for this packet,
	//    "packetSize": size of each packet,
	//    "packetTotal": total count of packets,
	// 	  "type": extensiton of this file,
	// 	  "base64Packet": base64-encoded fragment
	// 	 }
	// 	}
	// }

	// [OUT]
	// response = {
	// }
	// broadcast = {
	// 	"meshes" : {
	//    "<meshUUID>": JSON object that represents the loaded mesh
	//  }
	// }

	// initialize
	const nlohmann::json configRoom = nlohmann::json::parse(configRoomChar);
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);
	nlohmann::json configRoomUpdate = nlohmann::json::object();

	const nlohmann::json &fileJson = parameter.at("mesh").at("file");
	const std::string &fileId = fileJson.at("id").get<std::string>();
	const int &fileSize = fileJson.at("size").get<int>();
	const int &packetId = fileJson.at("packetId").get<int>();
	const int &packetSize = fileJson.at("packetSize").get<int>();
	const int &packetTotal = fileJson.at("packetTotal").get<int>();
	const std::string &fileType = fileJson.at("type").get<std::string>();
	const std::string &base64Packet = fileJson.at("base64Packet").get<std::string>();

	configRoomUpdate["extension"]["loadPolygonMesh"] = nlohmann::json::object();
	// read existing data
	if (configRoom.at("extension").contains("loadPolygonMesh"))
	{
		configRoomUpdate.at("extension").at("loadPolygonMesh").update(configRoom.at("extension").at("loadPolygonMesh"));
	}
	// allocate for new file
	if (!configRoomUpdate.at("extension").at("loadPolygonMesh").contains(fileId))
	{
		configRoomUpdate.at("extension").at("loadPolygonMesh")[fileId] = nlohmann::json::object();
		configRoomUpdate.at("extension").at("loadPolygonMesh").at(fileId)["packetArrived"] = nlohmann::json::array();
		for (int packet = 0; packet < packetTotal; ++packet)
		{
			configRoomUpdate.at("extension").at("loadPolygonMesh").at(fileId).at("packetArrived").push_back(false);
		}
		configRoomUpdate.at("extension").at("loadPolygonMesh").at(fileId)["base64Str"] = std::string(fileSize, '\0');
	}

	// read from this packet
	bool allPacketArrived = true;
	std::string base64Str;
	{
		std::string packetBase64Str = configRoomUpdate.at("extension").at("loadPolygonMesh").at(fileId).at("base64Str").get<std::string>();
		configRoomUpdate.at("extension").at("loadPolygonMesh").at(fileId).at("packetArrived").at(packetId) = true;

		packetBase64Str.replace(packetId * packetSize, base64Packet.size(), base64Packet);

		for (const auto &b : configRoomUpdate.at("extension").at("loadPolygonMesh").at(fileId).at("packetArrived"))
		{
			allPacketArrived &= b.get<bool>();
		}

		if (allPacketArrived)
		{
			base64Str = std::move(packetBase64Str);
			configRoomUpdate.at("extension").at("loadPolygonMesh").erase(fileId);
		}
		else
		{
			configRoomUpdate.at("extension").at("loadPolygonMesh").at(fileId).at("base64Str") = packetBase64Str;
		}
	}

	if (base64Str.empty())
	{
		// some packets are missing
		writeJSONToChar(configRoomUpdateChar, configRoomUpdate);
	}
	else
	{
		// for log
		const fs::path roomDataDir(configRoom.at("dataDir").get<std::string>());
		Doppelganger::Util::LogConfig roomLogConfig;
		{
			for (const auto &level_value : configRoom.at("log").at("level").items())
			{
				const std::string &level = level_value.key();
				const bool &value = level_value.value().get<bool>();
				roomLogConfig.level[level] = value;
			}
			for (const auto &type_value : configRoom.at("log").at("type").items())
			{
				const std::string &type = type_value.key();
				const bool &value = type_value.value().get<bool>();
				roomLogConfig.type[type] = value;
			}
		}

		fs::path filePath = fs::temp_directory_path();
		filePath /= Doppelganger::Util::uuid("DoppelgangerTmpFile-");
		filePath += ".";
		filePath += fileType;
		Doppelganger::Util::writeBase64ToFile(base64Str, filePath);

		Doppelganger::TriangleMesh mesh;
		mesh.UUID_ = Doppelganger::Util::uuid("mesh-");
		mesh.name_ = parameter.at("mesh").at("name").get<std::string>();

		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &V = mesh.V_;
		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &VN = mesh.VN_;
		Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> &F = mesh.F_;
		Eigen::Matrix<int, Eigen::Dynamic, 1> &FG = mesh.FG_;
		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &FN = mesh.FN_;
		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &VC = mesh.VC_;
		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &TC = mesh.TC_;
		Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> &FTC = mesh.FTC_;

		std::vector<std::vector<double>> VVec;
		std::vector<std::vector<double>> TCVec;
		std::vector<std::vector<double>> VNVec;
		std::vector<std::vector<double>> VCVec;
		std::vector<std::vector<int>> FVec;
		std::vector<std::vector<int>> FTCVec;
		std::vector<std::vector<int>> FNVec;
		std::vector<int> FGVec;

		if (fileType == "obj")
		{
			std::vector<std::tuple<std::string, int, int>> FM;
			igl::readOBJ(filePath.string(), VVec, TCVec, VNVec, VCVec, FVec, FTCVec, FNVec, FGVec, FM);
		}
		else if (fileType == "ply")
		{
			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> temp_V, temp_E, temp_VN, temp_VC, temp_TC;
			Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> temp_F;
			std::vector<std::string> Vheader({"red", "green", "blue"});
			igl::readPLY(filePath.string(), temp_V, temp_F, temp_E, temp_VN, temp_TC, temp_VC, Vheader);
			// normalize vertex colors
			temp_VC /= 255.;

			// write into vector (this is not optimal, but makes the code easy-to-read)
			VVec.resize(temp_V.rows());
			for (int v = 0; v < temp_V.rows(); ++v)
			{
				VVec.at(v) = std::vector<double>({temp_V(v, 0), temp_V(v, 1), temp_V(v, 2)});
			}
			FVec.resize(temp_F.rows());
			for (int f = 0; f < temp_F.rows(); ++f)
			{
				FVec.at(f) = std::vector<int>({temp_F(f, 0), temp_F(f, 1), temp_F(f, 2)});
			}
			VNVec.resize(temp_VN.rows());
			for (int v = 0; v < temp_VN.rows(); ++v)
			{
				VNVec.at(v) = std::vector<double>({temp_VN(v, 0), temp_VN(v, 1), temp_VN(v, 2)});
			}
			TCVec.resize(temp_TC.rows());
			for (int v = 0; v < temp_TC.rows(); ++v)
			{
				TCVec.at(v) = std::vector<double>({temp_TC(v, 0), temp_TC(v, 1)});
			}
			VCVec.resize(temp_VC.rows());
			for (int v = 0; v < temp_VC.rows(); ++v)
			{
				VCVec.at(v) = std::vector<double>({temp_VC(v, 0), temp_VC(v, 1), temp_VC(v, 2)});
			}
		}
		else if (fileType == "stl")
		{
			// todo: support vertex color (magics extension)
			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> temp_V, unified_V, temp_FN;
			Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> temp_F;
			Eigen::Matrix<int, Eigen::Dynamic, 1> SVI, SVJ;
			FILE *fp = fopen(filePath.string().c_str(), "rb");
			igl::readSTL(fp, temp_V, temp_F, temp_FN);
			fclose(fp);
			igl::remove_duplicate_vertices(temp_V, 0, unified_V, SVI, SVJ);
			std::for_each(temp_F.data(), temp_F.data() + temp_F.size(), [&SVJ](int &f)
						  { f = SVJ(f); });

			// write into vector (this is not optimal, but makes the code easy-to-read)
			VVec.resize(unified_V.rows());
			for (int v = 0; v < unified_V.rows(); ++v)
			{
				VVec.at(v) = std::vector<double>({unified_V(v, 0), unified_V(v, 1), unified_V(v, 2)});
			}
			FVec.resize(temp_F.rows());
			for (int f = 0; f < temp_F.rows(); ++f)
			{
				FVec.at(f) = std::vector<int>({temp_F(f, 0), temp_F(f, 1), temp_F(f, 2)});
			}
			// Currently, FN is ignored.
		}
		else if (fileType == "wrl")
		{
			igl::readWRL(filePath.string(), VVec, FVec, VCVec);
		}
		else
		{
			std::stringstream ss;
			ss << "file type ";
			ss << fileType;
			ss << " is not yet supported.";
			Doppelganger::Util::log(ss.str(), "ERROR", roomDataDir, roomLogConfig);
		}

		// format parsed info
		// V (required)
		{
			V.resize(VVec.size(), 3);
			for (int v = 0; v < VVec.size(); ++v)
			{
				V.row(v) << VVec.at(v).at(0), VVec.at(v).at(1), VVec.at(v).at(2);
			}
		}
		// F (required)
		{
			Eigen::Matrix<int, Eigen::Dynamic, 1> I, C;
			igl::polygon_corners(FVec, I, C);
			Eigen::Matrix<int, Eigen::Dynamic, 1> J;
			igl::polygons_to_triangles(I, C, F, J);
		}

		// VN (optional, calculate manually)
		VN.resize(VNVec.size(), 3);
		for (int v = 0; v < VNVec.size(); ++v)
		{
			VN.row(v) << VNVec.at(v).at(0), VNVec.at(v).at(1), VNVec.at(v).at(2);
		}
		if (VVec.size() != VNVec.size())
		{
			igl::per_vertex_normals(V, F, VN);
		}
		// FN (optional)
		igl::per_face_normals(V, F, FN);
		// VC (optional, but filled manually)
		VC.resize(VCVec.size(), 3);
		for (int v = 0; v < VCVec.size(); ++v)
		{
			VC.row(v) << VCVec.at(v).at(0), VCVec.at(v).at(1), VCVec.at(v).at(2);
		}
		// TC (optional)
		TC.resize(TCVec.size(), 2);
		for (int v = 0; v < TCVec.size(); ++v)
		{
			TC.row(v) << TCVec.at(v).at(0), TCVec.at(v).at(1);
		}
		// FTC (optional)
		{
			Eigen::Matrix<int, Eigen::Dynamic, 1> I, C;
			igl::polygon_corners(FTCVec, I, C);
			Eigen::Matrix<int, Eigen::Dynamic, 1> J;
			igl::polygons_to_triangles(I, C, FTC, J);
		}
		// FG (optional)
		// for dealing with non-triangle polygons (which is triangulated onLoad), we manually fill FG
		if (FVec.size() == FGVec.size())
		{
			FG.resize(F.rows(), 1);
			int fIdx = 0;
			for (int f = 0; f < FVec.size(); ++f)
			{
				for (int tri = 0; tri < FVec.at(f).size() - 2; ++tri)
				{
					FG(fIdx++, 0) = FGVec.at(f);
				}
			}
		}

		// register to this room
		configRoomUpdate["meshes"] = nlohmann::json::object();
		configRoomUpdate.at("meshes")[mesh.UUID_] = mesh;

		// write broadcast
		nlohmann::json broadcast = nlohmann::json::object();
		nlohmann::json meshJsonf;
		Doppelganger::to_json(meshJsonf, mesh, true);
		broadcast["meshes"] = nlohmann::json::object();
		broadcast.at("meshes")[mesh.UUID_] = meshJsonf;

		{
			// message
			std::stringstream ss;
			ss << "New mesh \"" << mesh.UUID_ << "\" is loaded.";
			Doppelganger::Util::log(ss.str(), "APICALL", roomDataDir, roomLogConfig);
			// file
			Doppelganger::Util::log(filePath, "APICALL", roomDataDir, roomLogConfig);
		}

		{
			// edit history
			nlohmann::json diff = nlohmann::json::object();
			nlohmann::json diffInv = nlohmann::json::object();
			diff["meshes"] = nlohmann::json::object();
			diff.at("meshes")[mesh.UUID_] = configRoomUpdate.at("meshes").at(mesh.UUID_);
			diff.at("meshes").at(mesh.UUID_)["remove"] = false;
			diffInv["meshes"] = nlohmann::json::object();
			diffInv.at("meshes")[mesh.UUID_] = nlohmann::json::object();
			diffInv.at("meshes").at(mesh.UUID_)["remove"] = true;

			configRoomUpdate["history"] = nlohmann::json::object();
			Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomUpdate.at("history"));
		}

		// write result
		writeJSONToChar(configRoomUpdateChar, configRoomUpdate);
		writeJSONToChar(broadcastChar, broadcast);
	}
}

#endif
