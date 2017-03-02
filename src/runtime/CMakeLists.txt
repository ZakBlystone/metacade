include_directories(${PROJECT_SOURCE_DIR}/src/runtime)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/core)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/render)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/engine)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/runtime)

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

add_definitions(-DMETACADE_API_EXPORT)
add_library(runtime SHARED ${CORE_SOURCES} ${RENDER_SOURCES} ${ENGINE_SOURCES} ${SOUND_SOURCES})