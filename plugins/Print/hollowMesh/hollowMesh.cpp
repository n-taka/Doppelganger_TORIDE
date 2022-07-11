#ifndef HOLLOWMESH_CPP
#define HOLLOWMESH_CPP

#include "Doppelganger/pluginCommon.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "Doppelganger/TriangleMesh.h"
#include "Doppelganger/Util/storeHistory.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "igl/grid.h"
#include "igl/signed_distance.h"
#include "igl/copyleft/marching_cubes.h"

#include "igl/per_face_normals.h"
#include "igl/per_vertex_normals.h"
#include "igl/facet_components.h"
#include "igl/remove_unreferenced.h"
#include "igl/triangle_triangle_adjacency.h"
#include "igl/vertex_triangle_adjacency.h"

void getPtrStrArrayForPartialConfig(
	const char *&parameterChar,
	char *&ptrStrArrayCoreChar,
	char *&ptrStrArrayRoomChar)
{
	const nlohmann::json parameter = nlohmann::json::parse(parameterChar);

	nlohmann::json ptrStrArrayCore = nlohmann::json::array();
	writeJSONToChar(ptrStrArrayCoreChar, ptrStrArrayCore);
	nlohmann::json ptrStrArrayRoom = nlohmann::json::array();
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
	//  ],
	//  "oneVoxelSize": double for size of a voxel,
	//  "shellThickness": double for shell thickness,
	//  "largestVoidOnly": boolean flag
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

	const double oneVoxelSize = parameter.at("oneVoxelSize").get<double>();
	const double shellThickness = parameter.at("shellThickness").get<double>();
	const bool largestVoidOnly = parameter.at("largestVoidOnly").get<bool>();

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

		// hollow
		Doppelganger::TriangleMesh hollowedMesh = mesh;
		hollowedMesh.name_ += "_hollow";
		{
			Eigen::Matrix<double, 1, Eigen::Dynamic> BBmin, BBmax, BB, BBcenter;
			{
				// calculate BB
				BBmin = hollowedMesh.V_.colwise().minCoeff();
				BBmax = hollowedMesh.V_.colwise().maxCoeff();
				BB = BBmax - BBmin;
				BBcenter = (BBmax + BBmin) * 0.5;
			}

			Eigen::Matrix<int, 1, Eigen::Dynamic> res;
			{
				// calculate grid size
				res = (BB / oneVoxelSize).array().ceil().matrix().template cast<int>();
				// margin 2 voxels for each side
				res(0, 0) += 4;
				res(0, 1) += 4;
				res(0, 2) += 4;
			}

			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> GV;
			{
				// prepare grid
				igl::grid(res, GV);
				GV.col(0) *= (res(0, 0) * oneVoxelSize);
				GV.col(1) *= (res(0, 1) * oneVoxelSize);
				GV.col(2) *= (res(0, 2) * oneVoxelSize);
				// translate grid
				Eigen::Matrix<double, 1, Eigen::Dynamic> trans;
				trans = BBcenter - ((GV.row(0) + GV.row(GV.rows() - 1)) * 0.5);
				GV.rowwise() += trans;
			}

			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> SV;
			Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> SF;
			{
				// marching cubes
				Eigen::Matrix<double, Eigen::Dynamic, 1> S;
				Eigen::Matrix<int, Eigen::Dynamic, 1> I;
				Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> C, N;
				igl::signed_distance(GV, hollowedMesh.V_, hollowedMesh.F_, igl::SIGNED_DISTANCE_TYPE_FAST_WINDING_NUMBER, S, I, C, N);
				S.array() += shellThickness;
				igl::copyleft::marching_cubes(S, GV, res(0), res(1), res(2), SV, SF);

				// flip internal void
				for (int f = 0; f < SF.rows(); ++f)
				{
					std::swap(SF(f, 1), SF(f, 2));
				}

				if (largestVoidOnly && SF.rows() > 0)
				{
					Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> SN;
					igl::per_face_normals(SV, SF, SN);

					Eigen::Matrix<int, Eigen::Dynamic, 1> C;
					igl::facet_components(SF, C);

					std::unordered_map<int, std::vector<int>> faceIdVec;
					std::unordered_map<int, double> faceIdVol;
					for (int f = 0; f < C.rows(); ++f)
					{
						faceIdVec[C(f, 0)].push_back(f);

						Eigen::Matrix<double, 1, Eigen::Dynamic> v01, v02;
						v01 = SV.row(SF(f, 1)) - SV.row(SF(f, 0));
						v02 = SV.row(SF(f, 2)) - SV.row(SF(f, 0));
						const double triarea = v01.template head<3>().cross(v02.template head<3>()).norm() * 0.5;
						const double height = -1.0 * SN.row(f).dot(SV.row(SF(f, 0)));
						const double volume = triarea * height;
						faceIdVol[C(f, 0)] += volume;
					}

					int maxVolId = -1;
					double maxVol = -1.0;
					for (const auto &id_vol : faceIdVol)
					{
						const int &id = id_vol.first;
						const double &vol = id_vol.second;
						if (vol >= maxVol)
						{
							maxVolId = id;
							maxVol = vol;
						}
					}
					if (maxVolId > -1)
					{
						const std::vector<int> &faces = faceIdVec.at(maxVolId);
						Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> SV_ = std::move(SV);
						Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> SF_;

						SF_.resize(faces.size(), SF.cols());
						for (int ff = 0; ff < faces.size(); ++ff)
						{
							SF_.row(ff) = SF.row(faces.at(ff));
						}
						Eigen::Matrix<int, Eigen::Dynamic, 1> I;
						igl::remove_unreferenced(SV_, SF_, SV, SF, I);
					}
				}
			}

			// merge to original mesh
			{
				// vertex
				const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> VPrev = std::move(hollowedMesh.V_);
				{
					hollowedMesh.V_.resize(VPrev.rows() + SV.rows(), 3);
					hollowedMesh.V_ << VPrev, SV;
				}
				// faces
				const Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> FPrev = std::move(hollowedMesh.F_);
				{
					hollowedMesh.F_.resize(FPrev.rows() + SF.rows(), 3);
					hollowedMesh.F_ << FPrev, (SF.array() + VPrev.rows());
				}

				// face normal (re-calculate)
				igl::per_face_normals(hollowedMesh.V_, hollowedMesh.F_, hollowedMesh.FN_);
				// vertex normal (re-calculate)
				igl::per_vertex_normals(hollowedMesh.V_, hollowedMesh.F_, hollowedMesh.FN_, hollowedMesh.VN_);

				// vertex color
				if (VPrev.rows() == hollowedMesh.VC_.rows())
				{
					const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> VCPrev = std::move(hollowedMesh.VC_);

					hollowedMesh.VC_.resize(hollowedMesh.V_.rows(), VCPrev.cols());
					hollowedMesh.VC_.setOnes();
					hollowedMesh.VC_.block(0, 0, VCPrev.rows(), VCPrev.cols()) = VCPrev;
				}
				// face color
				if (FPrev.rows() == hollowedMesh.FC_.rows())
				{
					const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> FCPrev = std::move(hollowedMesh.FC_);

					hollowedMesh.FC_.resize(hollowedMesh.F_.rows(), FCPrev.cols());
					hollowedMesh.FC_.setOnes();
					hollowedMesh.FC_.block(0, 0, FCPrev.rows(), FCPrev.cols()) = FCPrev;
				}

				// face group
				if (FPrev.rows() == hollowedMesh.FG_.rows())
				{
					const Eigen::Matrix<int, Eigen::Dynamic, 1> FGPrev = std::move(hollowedMesh.FG_);

					hollowedMesh.FG_.resize(hollowedMesh.F_.rows(), FGPrev.cols());
					const int freshGroupIdx = FGPrev.maxCoeff() + 1;
					hollowedMesh.FG_.setOnes();
					hollowedMesh.FG_ *= freshGroupIdx;
					hollowedMesh.FG_.block(0, 0, FGPrev.rows(), FGPrev.cols()) = FGPrev;
				}

				// texture coordinates
				{
					if (FPrev.rows() == hollowedMesh.FTC_.rows())
					{
						// face texture coordinates (FTC)
						// this implementation is not perfect ...
						//   FTC could contain different uv-coordinate on the same vertex
						const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TCPrev = std::move(hollowedMesh.TC_);
						const Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> FTCPrev = std::move(hollowedMesh.FTC_);

						hollowedMesh.TC_.resize(hollowedMesh.V_.rows(), 2);
						// we assign (0 ,0) for vertices on the internal void.
						hollowedMesh.TC_.setZero();
						for (int f = 0; f < FPrev.rows(); ++f)
						{
							for (int fv = 0; fv < FPrev.cols(); ++fv)
							{
								hollowedMesh.TC_.row(FPrev(f, fv)) = TCPrev.row(FTCPrev(f, fv));
							}
						}
						hollowedMesh.FTC_ = hollowedMesh.F_;
					}
					else if (VPrev.rows() == hollowedMesh.TC_.rows())
					{
						// (vertex) texture coordinates
						const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TCPrev = std::move(hollowedMesh.TC_);

						hollowedMesh.TC_.resize(hollowedMesh.V_.rows(), TCPrev.cols());
						// we assign (0 ,0) for vertices on the internal void.
						hollowedMesh.TC_.setZero();
						hollowedMesh.TC_.block(0, 0, TCPrev.rows(), TCPrev.cols()) = TCPrev;
					}
				}

				// misc
				{
					igl::triangle_triangle_adjacency(
						hollowedMesh.F_,
						hollowedMesh.TT_,
						hollowedMesh.TTi_);
					igl::vertex_triangle_adjacency(
						hollowedMesh.V_,
						hollowedMesh.F_,
						hollowedMesh.VF_,
						hollowedMesh.VFi_);
				}
			}
		}

		// store modified mesh
		diff.at("meshes")[hollowedMesh.UUID_] = hollowedMesh;
		configRoomPatch.at("meshes")[hollowedMesh.UUID_] = hollowedMesh;

		// update broadcast
		nlohmann::json meshJsonf;
		Doppelganger::to_json(meshJsonf, hollowedMesh, true);
		broadcast.at("meshes")[hollowedMesh.UUID_] = meshJsonf;

		// store history
		configRoomPatch["history"] = nlohmann::json::object();
		Doppelganger::Util::storeHistory(configRoom.at("history"), diff, diffInv, configRoomPatch.at("history"));
	}

	// write result
	writeJSONToChar(configRoomPatchChar, configRoomPatch);
	writeJSONToChar(broadcastChar, broadcast);
}

#endif
