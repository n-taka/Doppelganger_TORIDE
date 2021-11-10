cmake_minimum_required(VERSION 3.15)

### top menu (left)
add_subdirectory(loadCADData)
### top menu (right)
# no entry
### per-mesh
add_subdirectory(splitIntoIslands)
add_subdirectory(hollowMesh)
add_subdirectory(repairMesh)
add_subdirectory(estimateThickness)
### summary
# no entry
### bottom menu (left)
add_subdirectory(repairAllMeshes)
### bottom menu (right)
# no entry
### others
# no entry
