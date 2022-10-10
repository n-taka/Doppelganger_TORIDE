#ifndef LOADPOLYGONMESH_CPP
#define LOADPOLYGONMESH_CPP

#include "Doppelganger/pluginCommon.h"

#include "Doppelganger/Util/filesystem.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/uuid.h"
#include "Doppelganger/Util/writeBase64ToFile.h"
#include "Doppelganger/Util/storeHistory.h"
#include "Doppelganger/Util/log.h"

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

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/dataDir");
	ptrStrArrayRoom.push_back("/log");
	ptrStrArrayRoom.push_back("/history");
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
	// 	"mesh": {
	// 	 "name": name of this mesh (usually, filename without extension),
	// 	 "file": {
	// 	  "type": extension of this file,
	// 	  "base64Str": base64-encoded fragment
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
	nlohmann::json configRoomPatch = nlohmann::json::object();
	nlohmann::json broadcast = nlohmann::json::object();

	const nlohmann::json &fileJson = parameter.at("mesh").at("file");
	const std::string fileType = fileJson.at("type").get<std::string>();
	const std::string base64Str = fileJson.at("base64Str").get<std::string>();

	{
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
			std::ifstream ifs(filePath.string());
			igl::readSTL(ifs, temp_V, temp_F, temp_FN);
			ifs.close();
			igl::remove_duplicate_vertices(temp_V, 0.0, unified_V, SVI, SVJ);
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
			Doppelganger::Util::log(ss.str(), "ERROR", configRoom);
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
		configRoomPatch["meshes"] = nlohmann::json::object();
		configRoomPatch.at("meshes")[mesh.UUID_] = mesh;

		// write broadcast
		nlohmann::json meshJsonf;
		Doppelganger::to_json(meshJsonf, mesh, true);
		broadcast["meshes"] = nlohmann::json::object();
		broadcast.at("meshes")[mesh.UUID_] = meshJsonf;

		{
			// message
			std::stringstream ss;
			ss << "New mesh \"" << mesh.UUID_ << "\" is loaded.";
			Doppelganger::Util::log(ss.str(), "APICALL", configRoom);
			// file
			Doppelganger::Util::log(filePath, "APICALL", configRoom);
		}

		{
			// edit history
			nlohmann::json diff = nlohmann::json::object();
			nlohmann::json diffInv = nlohmann::json::object();
			diff["meshes"] = nlohmann::json::object();
			diff.at("meshes")[mesh.UUID_] = configRoomPatch.at("meshes").at(mesh.UUID_);
			diffInv["meshes"] = nlohmann::json::object();
			diffInv.at("meshes")[mesh.UUID_] = nlohmann::json(nullptr);

			configRoomPatch["history"] = nlohmann::json::object();
			Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));
		}

		// write result
		writeJSONToChar(configRoomPatchChar, configRoomPatch);
		writeJSONToChar(broadcastChar, broadcast);
	}
}

#endif
