cmake_minimum_required(VERSION 3.15)

##############################################
### project
##############################################
project(loadCADData
    VERSION 2.0.0
    DESCRIPTION "Load CAD data (iges, step)"
    HOMEPAGE_URL "https://github.com/n-taka/Doppelganger_TORIDE"
    LANGUAGES CXX
)


##############################################
### dependencies
##############################################
# Boost
if (WIN32)
    find_package(Boost REQUIRED COMPONENTS random)
elseif (APPLE)
    find_package(Boost REQUIRED COMPONENTS random filesystem)
elseif (UNIX)
    find_package(Boost REQUIRED COMPONENTS random)
endif ()
# json
find_package(nlohmann_json CONFIG REQUIRED)
# openSSL
find_package(OpenSSL REQUIRED)


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
)


##############################################
### include directories
##############################################
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger/submodule/libigl/include
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger/Doppelganger_install/include
)


##############################################
### libraries
##############################################
if(WIN32)
    # we need to explicitly add bcrypt.lib due to some bug.
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        ${CMAKE_SOURCE_DIR}/submodule/Doppelganger/Doppelganger_install/lib/Doppelganger.lib
        bcrypt.lib
    )
elseif (APPLE)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        ${CMAKE_SOURCE_DIR}/submodule/Doppelganger/Doppelganger_install/lib/libDoppelganger.a
    )
elseif (UNIX)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
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
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/TORIDE/${PROJECT_NAME}_${PROJECT_VERSION}/${CMAKE_SYSTEM_NAME}")
# dll
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_SOURCE_DIR}/bin/TORIDE/${PROJECT_NAME}_${PROJECT_VERSION}/${CMAKE_SYSTEM_NAME}")
# module.js
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/module.js")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/module.js" "${CMAKE_SOURCE_DIR}/bin/TORIDE/${PROJECT_NAME}_${PROJECT_VERSION}")
endif()
# script
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/FreeCADConvert.py" "${CMAKE_SOURCE_DIR}/bin/TORIDE/${PROJECT_NAME}_${PROJECT_VERSION}/")

# create zip archive
# https://stackoverflow.com/questions/44796465/cmake-zip-folder-but-remove-leading-path
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/TORIDE/${PROJECT_NAME}_${PROJECT_VERSION}.zip" --format=zip -- .
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/TORIDE/${PROJECT_NAME}_${PROJECT_VERSION}/")

# erase temporal directory
# add_custom_command(
#     TARGET ${PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E rm "-r" "${CMAKE_SOURCE_DIR}/bin/TORIDE/${PROJECT_NAME}_${PROJECT_VERSION}")
