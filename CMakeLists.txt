cmake_minimum_required (VERSION 2.8)
project (up_pqied)

# add_definitions(-DFOR_DEBUG)

# The version number.
set (_VERSION_MAJOR 1)
set (_VERSION_MINOR 0)
set (_VERSION_PATCH 0)

# configure a header file to pass some of the CMake settings to the source code
configure_file ( "${PROJECT_SOURCE_DIR}/config.h.in"
                 "${PROJECT_BINARY_DIR}/config.h" )
#configure_file ( "${PROJECT_BINARY_DIR}/bin/up_pqied.exe"
#                 "${PROJECT_SOURCE_DIR}/release" COPYONLY )

# add the binary tree to the search path for include files so that we will find config.h
include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}")

# add the src/ to the search path for src files
add_subdirectory (src bin)
add_subdirectory (scan_upsys)

#execute_process(COMMAND bash lua2lc.sh
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lua
#  RESULT_VARIABLE git_result
#  OUTPUT_VARIABLE git_ver )
#message(STATUS "git ver[${git_result}]: ${git_ver}")

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${_VERSION_PATCH}")
include (CPack)
