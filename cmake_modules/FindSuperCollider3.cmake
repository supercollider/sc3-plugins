# Find the SuperCollider development headers
#
# This module defines these variables:
#
#  SC_FOUND
#   True if the SC3 header files were found
#
#  SC_PATH
#   The path to the SC source+header files


#
# Find the telltale header file
#
GET_FILENAME_COMPONENT(SOURCEPARENT "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
find_path(SC_PATH NAMES include/plugin_interface/SC_PlugIn.h
	PATHS "${SOURCEPARENT}"
	PATH_SUFFIXES SuperCollider)

set(SC_FOUND FALSE)
if(IS_DIRECTORY ${SC_PATH})
	set(SC_FOUND TRUE)
endif()
