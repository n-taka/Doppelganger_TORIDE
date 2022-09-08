cmake_minimum_required(VERSION 3.15)

##############################################
### project
##############################################
project(saveScreenshotAll
    VERSION 2022.9.1
    DESCRIPTION "Save a screenshot of current view"
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
    convertImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdAllocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdBitUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdBlendMode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdColorMode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdDecompressRle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdEndianConversion.h
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdExport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdFixedSizeString.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdInterleave.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/Psdispod.h
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdLayerCanvasCopy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdMallocAllocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdMemoryUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/Psdminiz.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/Psdminiz.h
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdParseColorModeDataSection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdParseDocument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdParseImageDataSection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdParseImageResourcesSection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdParseLayerMaskSection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdPch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdStringUtil.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdSyncFileReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdSyncFileUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdSyncFileWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdUnionCast.h
    $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdNativeFile.cpp>
    $<$<PLATFORM_ID:Linux>:${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdNativeFile_Linux.cpp>
    $<$<PLATFORM_ID:Darwin>:${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src/Psd/PsdNativeFile_Mac.mm>
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/src/Doppelganger/Util/getCurrentTimestampAsString.cpp
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/src/Doppelganger/Util/log.cpp
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/src/Doppelganger/Util/uuid.cpp
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/src/Doppelganger/Util/writeBase64ToFile.cpp
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/src/Doppelganger/Util/encodeBinDataToBase64.cpp
)


##############################################
### include directories
##############################################
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/include
    ${CMAKE_SOURCE_DIR}/submodule/Doppelganger_Util/submodule/libigl/include
    ${CMAKE_SOURCE_DIR}/plugins/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/submodule/psd_sdk/src
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
    )
elseif (APPLE)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json
        nlohmann_json::nlohmann_json
    )
elseif (UNIX)
    target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${Boost_LIBRARIES}
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
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}/${CMAKE_SYSTEM_NAME}")
# dll
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}/${CMAKE_SYSTEM_NAME}")
# module.js
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/module.js")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/module.js" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}")
endif()

# create zip archive
# https://stackoverflow.com/questions/44796465/cmake-zip-folder-but-remove-leading-path
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}.zip" --format=zip -- .
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}/")

# erase temporal directory
# add_custom_command(
#     TARGET ${PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E rm "-r" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}")
