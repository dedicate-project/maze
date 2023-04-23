cmake_minimum_required(VERSION 3.8)
project(maze
  LANGUAGES CXX
  VERSION 0.0.1
  DESCRIPTION "Maze generator, simulator, and solver"
  HOMEPAGE_URL "https://github.com/dedicate-project/maze/")

# Build options
option(MAZE_BUILD_TESTS "Whether to build tests or not" YES)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directory to include path
include_directories(include)

# Add library target for non-main source files
add_library(${PROJECT_NAME} src/tiles.cpp src/player.cpp src/maze.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Specify include directories for the library
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# Add executable target for main source file and link to library
add_executable(${PROJECT_NAME}-bin src/main.cpp)
target_link_libraries(${PROJECT_NAME}-bin ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}-bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Testing
if (MAZE_BUILD_TESTS)
  enable_testing()

  add_library(tests_main tests/tests_main.cpp)
  target_include_directories(tests_main PUBLIC 3rdparty/Catch2/single_include)
  target_link_libraries(tests_main PUBLIC ${PROJECT_NAME})

  macro(declare_test name)
    message(STATUS "Add test: ${name}")
    add_executable(test_${name} tests/${name}.cpp)
    target_include_directories(test_${name} PUBLIC 3rdparty/Catch2/single_include)
    target_link_libraries(test_${name} PUBLIC tests_main)

    set_target_properties(test_${name}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
    add_test(NAME test_${name} COMMAND test_${name})
  endmacro()

  declare_test(maze)
endif(MAZE_BUILD_TESTS)

# Configure installation process
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}-bin
  EXPORT ${PROJECT_NAME}-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
