cmake_minimum_required(VERSION 3.8)
project(maze
  LANGUAGES CXX
  VERSION 0.0.1
  DESCRIPTION "Maze generator, simulator, and solver"
  HOMEPAGE_URL "https://github.com/dedicate-project/maze/")

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directory to include path
include_directories(include)

# Add library target for non-main source files
add_library(${PROJECT_NAME} SHARED src/tiles.cpp src/player.cpp src/maze.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Add executable target for main source file and link to library
add_executable(${PROJECT_NAME}-bin src/main.cpp)
target_link_libraries(${PROJECT_NAME}-bin ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}-bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
