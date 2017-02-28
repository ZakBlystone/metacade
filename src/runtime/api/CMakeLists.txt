file (GLOB_RECURSE API_SOURCES 
	"*.h"
	"*.cpp")

source_group("src" FILES ${API_SOURCES})

add_library(api STATIC ${API_SOURCES})