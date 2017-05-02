include_directories(${PROJECT_SOURCE_DIR}/src/runtime/sound)

file (GLOB_RECURSE SOUND_SOURCES 
	"*.h"
	"*.cpp")

source_group("src" FILES ${SOUND_SOURCES})

add_library(sound STATIC ${SOUND_SOURCES})