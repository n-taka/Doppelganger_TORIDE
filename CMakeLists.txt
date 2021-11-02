cmake_minimum_required(VERSION 3.15)

##############################################
### we use MT for users without C++ runtime
##############################################
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()


##############################################
### at least we support macOS that is still maintained
##############################################
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "" FORCE)
endif ()


##############################################
### we use vcpkg for dependencies
##############################################
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/submodule/Doppelganger/submodule/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")


##############################################
### project
##############################################
project(Doppelganger_Plugin_Core
    VERSION 2.0.0
    DESCRIPTION ""
    HOMEPAGE_URL "https://github.com/n-taka/Doppelganger_Plugin_Core"
    LANGUAGES CXX
)


##############################################
### plugins (Core)
##############################################
### top menu (left)
add_subdirectory(plugins/Core/loadPolygonMesh)
### top menu (right)
add_subdirectory(plugins/Core/undo)
add_subdirectory(plugins/Core/redo)
add_subdirectory(plugins/Core/sortMeshes)
### per-mesh
add_subdirectory(plugins/Core/loadTexture)
add_subdirectory(plugins/Core/saveMesh)
add_subdirectory(plugins/Core/saveScreenshot)
add_subdirectory(plugins/Core/removeMesh)
add_subdirectory(plugins/Core/toggleMeshVisibility)
add_subdirectory(plugins/Core/meshInfo)
add_subdirectory(plugins/Core/meshErrorInfo)
### summary
add_subdirectory(plugins/Core/allMeshesInfo)
### bottom menu (left)
add_subdirectory(plugins/Core/saveAllMeshes)
add_subdirectory(plugins/Core/saveScreenshotAll)
add_subdirectory(plugins/Core/removeAllMeshes)
add_subdirectory(plugins/Core/toggleAxesVisibility)
### bottom menu (right)
add_subdirectory(plugins/Core/configEditor)
add_subdirectory(plugins/Core/listPlugins)
add_subdirectory(plugins/Core/shutdown)
### others
add_subdirectory(plugins/Core/updatePlugins)
add_subdirectory(plugins/Core/pullCanvasParameters)
add_subdirectory(plugins/Core/pushCanvasParameters)
add_subdirectory(plugins/Core/pullCurrentMeshes)
add_subdirectory(plugins/Core/syncCursor)
add_subdirectory(plugins/Core/outlineOnClick)
add_subdirectory(plugins/Core/fitToFrame)
add_subdirectory(plugins/Core/alignCamera)

##############################################
### plugins (TORIDE)
##############################################
### top menu (left)
add_subdirectory(plugins/TORIDE/loadCADData)
### top menu (right)
# no entry
### per-mesh
add_subdirectory(plugins/TORIDE/splitIntoIslands)
add_subdirectory(plugins/TORIDE/hollowMesh)
add_subdirectory(plugins/TORIDE/repairMesh)
### summary
# no entry
### bottom menu (left)
add_subdirectory(plugins/TORIDE/repairAllMeshes)
### bottom menu (right)
# no entry
### others
# no entry
