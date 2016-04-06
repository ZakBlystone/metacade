include_directories(${PROJECT_SOURCE_DIR}/src/runtime ${SDL2_INCLUDE_DIR} ${GLEW_DIR} ${IL_INCLUDE_DIR})
link_directories(${IL_LIBRARY_DIR})

file (GLOB TOOL_SOURCES 
	"*.h"
	"*.cpp")

source_group("runtime" FILES ${TOOL_SOURCES})

add_executable(tool ${TOOL_SOURCES})

target_link_libraries(tool runtime DevIL ILU opengl32 glu32 ${SDL2_LIBRARY})
