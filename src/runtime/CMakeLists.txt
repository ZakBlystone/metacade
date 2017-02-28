include_directories(${PROJECT_SOURCE_DIR}/src/runtime ${PYTHON_INCLUDE_DIR})

file (GLOB RUNTIME_SOURCES 
	"*.h"
	"*.cpp")

source_group("runtime" FILES ${RUNTIME_SOURCES})

add_definitions(-DRUNTIME_API_EXPORT)
add_library(runtime SHARED ${RUNTIME_SOURCES})

target_link_libraries(runtime ${PYTHON_LIBRARY})