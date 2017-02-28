file (GLOB_RECURSE RENDER_SOURCES 
	"*.h"
	"*.cpp")

source_group("src" FILES ${RENDER_SOURCES})

add_library(render STATIC ${RENDER_SOURCES})