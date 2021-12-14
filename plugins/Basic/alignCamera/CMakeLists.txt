cmake_minimum_required(VERSION 3.15)

##############################################
### project
##############################################
project(alignCamera
    VERSION 2.0.0
    DESCRIPTION "Reset the camera so that the view direction aligns with an axis"
    HOMEPAGE_URL "https://github.com/n-taka/Doppelganger_Plugin_Core"
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
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}")
# module.js
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/module.js")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/module.js" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}/")
endif()

# create zip archive
# https://stackoverflow.com/questions/44796465/cmake-zip-folder-but-remove-leading-path
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}.zip" --format=zip -- .
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}/")

# erase temporal directory
# add_custom_command(
#     TARGET ${PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E rm "-r" "${CMAKE_SOURCE_DIR}/bin/Basic/${PROJECT_NAME}")
