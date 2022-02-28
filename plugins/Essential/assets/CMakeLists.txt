cmake_minimum_required(VERSION 3.15)

##############################################
### project
##############################################
project(assets
    VERSION 2022.3.0
    DESCRIPTION "Set of files for HTML client"
    HOMEPAGE_URL "https://github.com/n-taka/Doppelganger_Plugin"
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
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}/${CMAKE_SYSTEM_NAME}")
# css
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/css")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/css" "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}/css")
endif()
# html
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/html")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/html" "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}/html")
endif()
# icon
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icon")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/icon" "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}/icon")
endif()
# js
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/js")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/js" "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}/js")
endif()

# create zip archive
# https://stackoverflow.com/questions/44796465/cmake-zip-folder-but-remove-leading-path
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}_${PROJECT_VERSION}.zip" --format=zip -- .
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}/")

# erase temporal directory
# add_custom_command(
#     TARGET ${PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E rm "-r" "${CMAKE_SOURCE_DIR}/bin/Essential/${PROJECT_NAME}")
