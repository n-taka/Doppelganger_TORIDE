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
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "" FORCE)
endif ()


##############################################
### we use vcpkg for dependencies
##############################################
# set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/submodule/Doppelganger_Util/submodule/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")


##############################################
### project
##############################################
project(Doppelganger_Plugin
    VERSION 2.0.0
    DESCRIPTION ""
    HOMEPAGE_URL "https://github.com/n-taka/Doppelganger_Plugin"
    LANGUAGES CXX
)


##############################################
### plugins
##############################################
add_subdirectory(plugins)
