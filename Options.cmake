# log all *_INIT variables
get_cmake_property(_varNames VARIABLES)
list(REMOVE_DUPLICATES _varNames)
list(SORT _varNames)
foreach(_varName ${_varNames})
	if(_varName MATCHES "_INIT$")
		message(STATUS "${_varName}=${${_varName}}")
	endif()
endforeach()

if(MSVC)
	string(REGEX REPLACE "/W[0-5]" "/W4" CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT}")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebugInfo")
		string(REGEX REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT}")
	endif()
endif()