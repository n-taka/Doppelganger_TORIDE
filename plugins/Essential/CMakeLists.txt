cmake_minimum_required(VERSION 3.15)

### top menu (left)
# no entry

### top menu (right)
add_subdirectory(sortMeshes)

### per-mesh
# no entry

### summary
# no entry

### bottom menu (left)
# no entry

### bottom menu (right)
add_subdirectory(configEditor)
add_subdirectory(listPlugins)
add_subdirectory(shutdown)

### others
add_subdirectory(assets)
add_subdirectory(updatePlugins)
add_subdirectory(pullCurrentMeshes)
