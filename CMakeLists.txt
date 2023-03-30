cmake_minimum_required(VERSION 3.8)

project(maze)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)

# Add include directory to include path
include_directories(include)

# Add library target for non-main source files
add_library(maze_lib src/tiles.cpp src/player.cpp src/maze.cpp)

# Add executable target for main source file and link to library
add_executable(${PROJECT_NAME}-bin src/main.cpp)
target_link_libraries(${PROJECT_NAME}-bin maze_lib)
