cmake_minimum_required(VERSION 3.5.0)
project(OpenOcean VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(_SOURCE_DIR ./src)

set(INCLUDE_DIRS C:/msys64/mingw64/include)
set(LIBRARIES -LC:/msys64/mingw64/lib)
include_directories(${INCLUDE_DIRS})
include_directories(${_SOURCE_DIR})
file(GLOB SOURCE_FILES ${_SOURCE_DIR}/*.cpp)
file(GLOB INCLUDE_FILES ${_SOURCE_DIR}/*.h ${_SOURCE_DIR}/*.hpp)
link_libraries(-lglfw3 -lglew32 -lgdi32 -lassimp -lfreeglut -lopengl32 -lwinmm)
add_executable(main ${SOURCE_FILES} ${INCLUDE_FILES})

add_compile_options("-fdiagnostics-color=always" "-g" "-Wall" "-Wno-unknown-pragmas" "-Wno-sign-compare" "-Og" )

target_link_libraries(main ${LIBRARIES})
