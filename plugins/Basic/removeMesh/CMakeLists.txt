cmake_minimum_required(VERSION 3.15)

##############################################
### project
##############################################
project(removeMesh
    VERSION 2022.3.0
    DESCRIPTION "Remove single mesh"
    HOMEPAGE_URL "https://github.com/n-taka/Doppelganger_Plugin"
    LANGUAGES CXX
)


##############################################
### dependencies
##############################################
# json
find_package(nlohmann_json CONFIG REQUIRED)


##############################################
### target
##############################################
add_library(${PROJECT_NAME} MODULE)


##############################################
### source
##############################################
target_sources(
    ${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}.cpp
    ${CMAKE_SOURCE_DIR}/plugins/common/src/Doppelganger/TriangleMesh.cpp
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/src/Doppelganger/Util/storeHistory.cpp
)


##############################################
### include directories
##############################################
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/include
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/submodule/libigl/include
    ${CMAKE_SOURCE_DIR}/plugins/common/include
)


##############################################
### libraries
##############################################
if(WIN32)
    # we need to explicitly add bcrypt.lib due to some bug.
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        nlohmann_json
        nlohmann_json::nlohmann_json
    )
elseif (APPLE)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        nlohmann_json
        nlohmann_json::nlohmann_json
    )
elseif (UNIX)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        nlohmann_json
        nlohmann_json::nlohmann_json
    )
endif ()


##############################################
### compile options
##############################################
if (WIN32)
    # because we want to use std::filesystem, we require C++17 for windows
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
    # our target is windows 10 or newer
    target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=0x0A00)
elseif (APPLE)
    # because we support macOS Mojave, we only require C++14 for macOS
    # we use boost::filesystem for macOS
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
elseif (UNIX)
    # because we want to use std::filesystem, we require C++17 for linux
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
endif ()
target_compile_definitions(${PROJECT_NAME} PUBLIC _USE_MATH_DEFINES)


##############################################
### resources
##############################################
# directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}/${CMAKE_SYSTEM_NAME}")
# dll
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}/${CMAKE_SYSTEM_NAME}")
# module.js
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/module.js")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/module.js" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}")
endif()

# create zip archive
# https://stackoverflow.com/questions/44796465/cmake-zip-folder-but-remove-leading-path
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}.zip" --format=zip -- .
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}/")

# erase temporal directory
# add_custom_command(
#     TARGET ${PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E rm "-r" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}")
