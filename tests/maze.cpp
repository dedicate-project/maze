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

  SECTION("A maze from layout has the specified size") {
    using namespace maze;
    const std::vector<std::vector<Maze::PerceivedTile>> layout = {
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::START, Maze::PerceivedTile::FOOD, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::DOOR, Maze::PerceivedTile::END},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL}
    };

    maze::Maze layouted_maze(layout);

    REQUIRE(layouted_maze.getRows() == 5);
    REQUIRE(layouted_maze.getCols() == 4);
  }

  SECTION("A layout is constructed correctly") {
    using namespace maze;
    const std::vector<std::vector<Maze::PerceivedTile>> layout = {
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::START, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::FOOD, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::DOOR, Maze::PerceivedTile::END},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL}
    };

    maze::Maze layouted_maze(layout);

    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(0, 0)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(0, 1)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(0, 2)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(0, 3)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<EmptyTile>(layouted_maze.getTile(1, 0)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<EmptyTile>(layouted_maze.getTile(1, 1)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<EmptyTile>(layouted_maze.getTile(1, 2)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(1, 3)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(2, 0)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<FoodTile >(layouted_maze.getTile(2, 1)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<EmptyTile>(layouted_maze.getTile(2, 2)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(2, 3)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(3, 0)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<EmptyTile>(layouted_maze.getTile(3, 1)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<DoorTile >(layouted_maze.getTile(3, 2)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<EmptyTile>(layouted_maze.getTile(3, 3)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(4, 0)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(4, 1)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(4, 2)) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<WallTile >(layouted_maze.getTile(4, 3)) != nullptr);

    REQUIRE(layouted_maze.getStartPosition() == maze::Coordinates{1, 0});
    REQUIRE(layouted_maze.getPlayerPosition() == maze::Coordinates{1, 0});
    REQUIRE(layouted_maze.getEndPosition() == maze::Coordinates{3, 3});
  }

  SECTION("An unsolvable layout throws when attempted to find a solution") {
    using namespace maze;
    const std::vector<std::vector<Maze::PerceivedTile>> layout = {
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::START, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::DOOR, Maze::PerceivedTile::END},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL}
    };

    maze::Maze layouted_maze(layout);

    REQUIRE_THROWS_AS(layouted_maze.solve(), std::runtime_error);
  }

  SECTION("A solvable layout returns a solution path") {
    using namespace maze;
    const std::vector<std::vector<Maze::PerceivedTile>> layout = {
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::START, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::END},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL}
    };

    maze::Maze layouted_maze(layout);
    auto path = layouted_maze.solve();

    REQUIRE(path[0] == Maze::Move::RIGHT);
    REQUIRE(path[1] == Maze::Move::RIGHT);
    REQUIRE(path[2] == Maze::Move::RIGHT);
  }

  SECTION("Perceiving current tiles yields the correct result") {
    using namespace maze;
    const std::vector<std::vector<Maze::PerceivedTile>> layout = {
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::START, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::DOOR, Maze::PerceivedTile::END},
      {Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL, Maze::PerceivedTile::WALL}
    };

    maze::Maze layouted_maze(layout);
    layouted_maze.movePlayer(Maze::Move::RIGHT);
    layouted_maze.movePlayer(Maze::Move::RIGHT);

    auto tiles = layouted_maze.perceiveTiles(2);

    REQUIRE(tiles[0][0] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[0][1] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[0][2] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[0][3] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[0][4] == Maze::PerceivedTile::UNKNOWN);

    REQUIRE(tiles[1][0] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[1][1] == Maze::PerceivedTile::WALL);
    REQUIRE(tiles[1][2] == Maze::PerceivedTile::WALL);
    REQUIRE(tiles[1][3] == Maze::PerceivedTile::WALL);
    REQUIRE(tiles[1][4] == Maze::PerceivedTile::UNKNOWN);

    REQUIRE(tiles[2][0] == Maze::PerceivedTile::START);
    REQUIRE(tiles[2][1] == Maze::PerceivedTile::EMPTY);
    REQUIRE(tiles[2][2] == Maze::PerceivedTile::EMPTY);
    REQUIRE(tiles[2][3] == Maze::PerceivedTile::WALL);
    REQUIRE(tiles[2][4] == Maze::PerceivedTile::UNKNOWN);

    REQUIRE(tiles[3][0] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[3][1] == Maze::PerceivedTile::EMPTY);
    REQUIRE(tiles[3][2] == Maze::PerceivedTile::WALL);
    REQUIRE(tiles[3][3] == Maze::PerceivedTile::WALL);
    REQUIRE(tiles[3][4] == Maze::PerceivedTile::UNKNOWN);

    REQUIRE(tiles[4][0] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[4][1] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[4][2] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[4][3] == Maze::PerceivedTile::UNKNOWN);
    REQUIRE(tiles[4][4] == Maze::PerceivedTile::UNKNOWN);
  }
}
