include_directories(${PROJECT_SOURCE_DIR}/src)

if (WIN32)
add_definitions(-DWINDOWS)
else()
add_definitions(-DLINUX)
endif()

add_subdirectory(runtime)
add_subdirectory(tool)
