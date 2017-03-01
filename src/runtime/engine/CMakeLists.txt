include_directories(${PROJECT_SOURCE_DIR}/src/runtime/render)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/core)
include_directories(${PROJECT_SOURCE_DIR}/src/runtime/engine)

file (GLOB_RECURSE ENGINE_SOURCES 
	"*.h"
	"*.cpp")

source_group("src" FILES ${ENGINE_SOURCES})

add_library(engine STATIC ${ENGINE_SOURCES})