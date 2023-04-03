cmake_minimum_required(VERSION 3.16)

##############################################
### project
##############################################
project(fitToFrame
    VERSION 2023.04.0
    DESCRIPTION "Reset the camera so that the visible meshes fits to the current view"
    HOMEPAGE_URL "https://github.com/n-taka/Doppelganger_TORIDE"
    LANGUAGES CXX
)


##############################################
### target
##############################################
add_custom_target(${PROJECT_NAME} ALL)


##############################################
### resources
##############################################
# directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}_${PROJECT_VERSION}/${CMAKE_SYSTEM_NAME}")
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
