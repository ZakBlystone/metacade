include_directories(${PROJECT_SOURCE_DIR}/src/runtime)

file (GLOB_RECURSE API_SOURCES 
	"api/*.h"
	"api/*.cpp")

source_group("src" FILES ${API_SOURCES})

add_subdirectory(core)
add_subdirectory(engine)
add_subdirectory(render)
add_subdirectory(sound)

add_definitions(-DRUNTIME_API_EXPORT)
add_library(runtime SHARED ${API_SOURCES})

target_link_libraries(runtime core engine render sound ${PYTHON_LIBRARY})