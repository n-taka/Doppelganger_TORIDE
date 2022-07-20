#ifndef REPAIRALLMESHES_CPP
#define REPAIRALLMESHES_CPP

#include "Doppelganger/pluginCommon.h"
#include "Doppelganger/Util/filesystem.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/uuid.h"
#include "Doppelganger/Util/storeHistory.h"

#include <string>
#include <vector>

#include "igl/facet_components.h"
#include "igl/remove_unreferenced.h"

#include "igl/writePLY.h"
#include "igl/readOFF.h"

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
	ptrStrArrayRoom.push_back("/dataDir");
	ptrStrArrayRoom.push_back("/history");
	for (const auto &UUID : parameter.at("meshes"))
	{
		std::string targetMeshPath("/meshes/");
		targetMeshPath += UUID.get<std::string>();
		ptrStrArrayRoom.push_back(targetMeshPath);
	}
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
	// 	"meshes": [
	// 	 "UUID-A",
	// 	 "UUID-B",
	//   ...
	//  ]
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
	broadcast["meshes"] = nlohmann::json::object();
	configRoomPatch["meshes"] = nlohmann::json::object();

	for (const auto &UUID : parameter.at("meshes"))
	{
		const std::string meshUUID = UUID.get<std::string>();
		const Doppelganger::TriangleMesh mesh = configRoom.at("meshes").at(meshUUID).get<Doppelganger::TriangleMesh>();

		nlohmann::json diff = nlohmann::json::object();
		nlohmann::json diffInv = nlohmann::json::object();
		diff["meshes"] = nlohmann::json::object();
		diffInv["meshes"] = nlohmann::json::object();

		// store current mesh
		diffInv.at("meshes")[mesh.UUID_] = configRoom.at("meshes").at(meshUUID);

		// repair
		Doppelganger::TriangleMesh repairedMesh = mesh;
		repairedMesh.name_ += "_repair";
		{
			// find binary executable
			fs::path meshFix;
			{
				fs::path pluginDir(configRoom.at("dataDir").get<std::string>());
				pluginDir.append("plugin");

				std::string dirName("repairAllMeshes");
				dirName += "_";
				// todo: should not use hard-code
				std::string installedVersion("2022.7.1.MeshFix");
				dirName += installedVersion;

				pluginDir.append(dirName);

				meshFix = fs::path(pluginDir);
#if defined(_WIN64)
				meshFix.append("Windows");
				meshFix.append("MeshFix.exe");
#elif defined(__APPLE__)
				meshFix.append("Darwin");
				meshFix.append("MeshFix");
#elif defined(__linux__)
				meshFix.append("Linux");
				meshFix.append("MeshFix");
#endif
			}

			// Because MeshFix only fixes the largest component
			//   (small components are removed)
			// So, we apply MeshFix for each connected components,
			// and then merge all the fixed meshes.
			// **NOTE** we have asssumption that the target mesh is
			// "well-connected"
			Eigen::Matrix<int, Eigen::Dynamic, 1> C;
			igl::facet_components(repairedMesh.F_, C);
			repairedMesh.V_.resize(0, repairedMesh.V_.cols());
			repairedMesh.F_.resize(0, repairedMesh.F_.cols());

			for (int c = 0; c < C.maxCoeff() + 1; ++c)
			{
				Doppelganger::TriangleMesh componentMesh;
				Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> componentV;
				Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> componentF;

				const int componentFCount = (C.array() == c).count();
				componentF.resize(componentFCount, mesh.F_.cols());
				int fIdx = 0;
				for (int f = 0; f < C.rows(); ++f)
				{
					if (C(f) == c)
					{
						componentF.row(fIdx++) = mesh.F_.row(f);
					}
				}
				Eigen::Matrix<int, Eigen::Dynamic, 1> I;
				igl::remove_unreferenced(mesh.V_, componentF, componentMesh.V_, componentMesh.F_, I);

				fs::path fileNameIn, fileNameOut;
				const std::string tmpFileName = Doppelganger::Util::uuid("DoppelgangerTmpFile-");
				fileNameIn = fs::temp_directory_path();
				fileNameOut = fs::temp_directory_path();
				fileNameIn /= tmpFileName;
				fileNameOut /= tmpFileName;
				fileNameIn += ".ply";
				fileNameOut += "_fixed.off";

				// MeshFix requires float (not double) for floating-point
				igl::writePLY(fileNameIn.string(), componentMesh.V_.template cast<float>(), componentMesh.F_);

				std::stringstream cmd;
#if defined(_WIN64)
				cmd << "\"";
#endif
				cmd << meshFix;
				cmd << " ";
				cmd << fileNameIn;
				cmd << " ";
				cmd << fileNameOut;
#if defined(_WIN64)
				cmd << "\"";
#endif
				system(cmd.str().c_str());

				igl::readOFF(fileNameOut.string(), componentV, componentF);
				if (componentF.rows() > 0)
				{
					Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> tempV = std::move(repairedMesh.V_);
					Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> tempF = std::move(repairedMesh.F_);
					repairedMesh.V_.resize(tempV.rows() + componentV.rows(), tempV.cols());
					repairedMesh.F_.resize(tempF.rows() + componentF.rows(), tempF.cols());
					repairedMesh.V_ << tempV,
						componentV;
					repairedMesh.F_ << tempF,
						(componentF.array() + tempV.rows()).matrix();
				}

				fs::remove(fileNameIn);
				fs::remove(fileNameOut);
			}
		}

		// todo: copy attributes
		//   * VC
		//   * UV
		//   * etc...
		repairedMesh.VN_.resize(0, 3);
		repairedMesh.FN_.resize(0, 3);
		repairedMesh.VC_.resize(0, 3);
		repairedMesh.FC_.resize(0, 3);
		repairedMesh.FG_.resize(0, 1);
		repairedMesh.TC_.resize(0, 2);
		repairedMesh.FTC_.resize(0, 3);

		// store modified mesh
		diff.at("meshes")[repairedMesh.UUID_] = repairedMesh;
		configRoomPatch.at("meshes")[repairedMesh.UUID_] = repairedMesh;

		// update broadcast
		nlohmann::json meshJsonf;
		Doppelganger::to_json(meshJsonf, repairedMesh, true);
		broadcast.at("meshes")[repairedMesh.UUID_] = meshJsonf;

		// store history
		configRoomPatch["history"] = nlohmann::json::object();
		Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));
	}

	// write result
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
	writeJSONToChar(broadcastChar, broadcast);
}

#endif
