cmake_minimum_required(VERSION 2.6)
project(METACADE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2 -DRELEASE")
elseif (MSVC)
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /wd4100")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} /DRELEASE /MT")
endif()

if (WIN32)
	find_package(OpenGL REQUIRED)
	find_package(SDL2 REQUIRED)
else()
	INCLUDE(FindPkgConfig)
	#PKG_SEARCH_MODULE(OpenGL REQUIRED opengl)
	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
endif()

SET(LUA_INCLUDE_DIR "" CACHE PATH "Lua include directory")
SET(LUA_LIBRARY "" CACHE FILEPATH "Lua library")
SET(GLEW_DIR "" CACHE PATH "GLEW directory")

SET(IL_INCLUDE_DIR "" CACHE PATH "DevIL include directory")
SET(IL_LIBRARY_DIR "" CACHE PATH "DevIL library dir")

SET(PYTHON_INCLUDE_DIR "" CACHE PATH "Python include directory")
SET(PYTHON_CONFIG_DIR "" CACHE PATH "Python config directory")
SET(PYTHON_LIBRARY_DIR "" CACHE PATH "Python library directory")

SET(V8_ENABLED FALSE CACHE BOOL "Enable v8 Javascript support")
SET(V8_INCLUDE_DIR "" CACHE PATH "v8 include directory")
SET(V8_LIBRARY_DIR "" CACHE PATH "v8 library directory")

if(${V8_ENABLED})
	add_definitions(-DWITH_V8)
endif()

add_subdirectory(src)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/lib CACHE
    PATH "Directory where all the .lib files are dumped." FORCE)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/bin CACHE
    PATH "Directory where .exe and .dll files are dumped." FORCE)
