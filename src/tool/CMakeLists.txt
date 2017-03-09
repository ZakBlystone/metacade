include_directories(${PROJECT_SOURCE_DIR}/src/runtime ${SDL2_INCLUDE_DIR} ${GLEW_DIR} ${IL_INCLUDE_DIR})
link_directories(${IL_LIBRARY_DIR} ${PROJECT_SOURCE_DIR}/lib)

file (GLOB TOOL_SOURCES 
	"*.h"
	"*.cpp")

source_group("src" FILES ${TOOL_SOURCES})

add_executable(tool ${TOOL_SOURCES})

target_link_libraries(tool runtime.lib DevIL ILU opengl32 glu32 ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})
