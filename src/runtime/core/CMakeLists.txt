file (GLOB_RECURSE CORE_SOURCES 
	"*.h"
	"*.cpp")

source_group("src" FILES ${CORE_SOURCES})

add_library(core STATIC ${CORE_SOURCES})