include_directories(${PROJECT_SOURCE_DIR}/src/runtime ${LUA_INCLUDE_DIR} ${GLEW_DIR})

file (GLOB RUNTIME_SOURCES 
	"*.h"
	"*.cpp")

source_group("runtime" FILES ${RUNTIME_SOURCES})

add_definitions(-DRUNTIME_API_EXPORT)
add_library(runtime SHARED ${RUNTIME_SOURCES})

#${IL_LIBRARY_DIR}
target_link_libraries(runtime ${LUA_LIBRARY} opengl32 glu32)