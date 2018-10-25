# (C) Hiventive 2018
# Guillaume Delbergue <guillaume.delbergue@hiventive.com>
# MIT License

# YAML CMake parser
# FIXME: This parser is broken with depth >= 1

function(yaml_parse FILENAME PREFIX)
	file(READ ${FILENAME} _YAML_FILE_CONTENT)
	string(REGEX REPLACE "\n" ";" _YAML_FILE_CONTENT_LINES "${_YAML_FILE_CONTENT}")
	set(_YAML_INDENT 2)
	set(_YAML_DEPTH 0)
	set(_YAML_NODE "")
	set(_YAML_PREFIX ${PREFIX})
	foreach(_YAML_FILE_CONTENT_LINE ${_YAML_FILE_CONTENT_LINES})
		if(${_YAML_FILE_CONTENT_LINE} MATCHES ".*([0-9_a-zA-Z]+):$")
			# Get map name
			string(REGEX REPLACE "^[ ]*([0-9_a-zA-Z]+):$" "\\1" _YAML_MAP_NAME "${_YAML_FILE_CONTENT_LINE}")
			string(TOUPPER ${_YAML_MAP_NAME} _YAML_MAP_NAME)
			# Count number of whitespaces to get the depth
			string(REGEX MATCHALL " " _YAML_MAP_INDENT "${_YAML_FILE_CONTENT_LINE}")
			list(LENGTH _YAML_MAP_INDENT _YAML_DEPTH)
			# Store name
			# FIXME: broken here
			if(NOT ${_YAML_DEPTH})
				set(_YAML_NODE ${_YAML_MAP_NAME})
			else()
				set(_YAML_NODE "${_YAML_NODE}_${_YAML_MAP_NAME}")
			endif()
		elseif(${_YAML_FILE_CONTENT_LINE} MATCHES ".*([0-9_a-zA-Z]+): ([0-9_a-zA-Z.-]+)")
			# Get scalar name and value
			string(REGEX REPLACE "^[ ]*([0-9_a-zA-Z]+): [0-9_a-zA-Z.-]+$" "\\1" _YAML_SCALAR_NAME "${_YAML_FILE_CONTENT_LINE}")
			string(TOUPPER ${_YAML_SCALAR_NAME} _YAML_SCALAR_NAME)
			# Get scalar value
			string(REGEX REPLACE "^[ ]*[0-9_a-zA-Z]+: ([0-9_a-zA-Z.-]+)$" "\\1" _YAML_SCALAR_VALUE "${_YAML_FILE_CONTENT_LINE}")
			# Store value
			set(${_YAML_PREFIX}_${_YAML_NODE}_${_YAML_SCALAR_NAME} "${_YAML_SCALAR_VALUE}" PARENT_SCOPE)
		endif()
	endforeach()
endfunction()


