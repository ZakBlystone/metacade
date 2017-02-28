file (GLOB_RECURSE ENGINE_SOURCES 
	"*.h"
	"*.cpp")

source_group("src" FILES ${ENGINE_SOURCES})

add_library(engine STATIC ${ENGINE_SOURCES})