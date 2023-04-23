#include <catch2/catch.hpp>

#include <maze/maze.hpp>

TEST_CASE("maze") {
  SECTION("A generated maze has the specified size") {
    const uint32_t rows = 40;
    const uint32_t cols = 26;

    maze::Maze generated_maze(rows, cols, 0.5);

    REQUIRE(generated_maze.getRows() == rows);
    REQUIRE(generated_maze.getCols() == cols);
  }
}
