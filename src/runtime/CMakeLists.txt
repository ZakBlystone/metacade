include_directories(${PROJECT_SOURCE_DIR}/src/runtime)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/core)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/render)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/engine)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/runtime)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/script/py)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/script/lua)
include_directories(${LUA_INCLUDE_DIR})
#include_directories(${PYTHON_INCLUDE_DIR} ${PYTHON_CONFIG_DIR})
#link_directories(${PYTHON_LIBRARY_DIR})

include_directories(${PYTHON_INCLUDE_DIR} ${PYTHON_CONFIG_DIR})

file (GLOB_RECURSE CORE_SOURCES 
	"core/*.h"
	"core/*.cpp")

source_group("src\\core" FILES ${CORE_SOURCES})

file (GLOB_RECURSE RENDER_SOURCES 
	"render/*.h"
	"render/*.cpp")

source_group("src\\render" FILES ${RENDER_SOURCES})

file (GLOB_RECURSE ENGINE_SOURCES 
	"engine/*.h"
	"engine/*.cpp")

source_group("src\\engine" FILES ${ENGINE_SOURCES})

file (GLOB_RECURSE SOUND_SOURCES 
	"sound/*.h"
	"sound/*.cpp")

source_group("src\\sound" FILES ${SOUND_SOURCES})

file (GLOB_RECURSE PYTHON_SOURCES 
	"script/py/*.h"
	"script/py/*.cpp")

source_group("src\\script\\python" FILES ${PYTHON_SOURCES})

file (GLOB_RECURSE LUA_SOURCES 
	"script/lua/*.h"
	"script/lua/*.cpp")

source_group("src\\script\\lua" FILES ${LUA_SOURCES})

add_definitions(-DMETACADE_API_EXPORT)
add_library(runtime SHARED ${CORE_SOURCES} ${RENDER_SOURCES} ${ENGINE_SOURCES} ${SOUND_SOURCES} ${LUA_SOURCES})

if (WIN32)
	#target_link_libraries(runtime debug python37_d.lib)
	#target_link_libraries(runtime optimized python37.lib)
endif()

target_link_libraries(runtime ${LUA_LIBRARY})

add_custom_command(TARGET runtime POST_BUILD
    COMMAND python tounreal.py
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)