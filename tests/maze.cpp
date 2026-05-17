#include <catch2/catch.hpp>

#include <maze/maze.hpp>
#include <maze/player.hpp>

TEST_CASE("Player food math") {
  // Regression for the saturating-arithmetic rewrite of pickFood/consumeFood. The previous
  // version underflowed uint32_t on over-consumption and miscalculated the pickFood cap.
  SECTION("pickFood saturates at maxWeight without underflow") {
    maze::Player player(100);
    REQUIRE(player.getCurrentFood() == 100);
    player.pickFood(50); // already at the cap; no change
    REQUIRE(player.getCurrentFood() == 100);
  }

  SECTION("pickFood from below the cap fills exactly to maxWeight") {
    maze::Player player(100);
    REQUIRE(player.consumeFood(40) == true);
    REQUIRE(player.getCurrentFood() == 60);
    player.pickFood(50); // would exceed the cap; should clamp to 100, not the old 50 bug
    REQUIRE(player.getCurrentFood() == 100);
  }

  SECTION("consumeFood saturates at zero and reports starvation") {
    maze::Player player(10);
    REQUIRE(player.consumeFood(10) == false); // exact drain -> false (no food left)
    REQUIRE(player.getCurrentFood() == 0);
    REQUIRE(player.consumeFood(5) == false); // would have underflowed in the old version
    REQUIRE(player.getCurrentFood() == 0);
  }
}

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

  SECTION("movePlayer rejects out-of-bounds moves without underflowing") {
    // Regression for the unsigned-underflow bug where Coordinates::col-- at col=0 wrapped
    // to ~4 billion and the subsequent `>= 0` guard let the out-of-bounds index reach
    // grid_[...] -- a crash. Now we expect a clean `false` return value.
    using namespace maze;
    const std::vector<std::vector<Maze::PerceivedTile>> layout = {
      {Maze::PerceivedTile::START, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::END},
      {Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::EMPTY, Maze::PerceivedTile::EMPTY},
    };
    Maze m(layout);
    REQUIRE(m.movePlayer(Maze::Move::LEFT) == false); // would underflow col
    REQUIRE(m.movePlayer(Maze::Move::UP) == false);   // would underflow row
    REQUIRE(m.getPlayerPosition() == Coordinates{0, 0});
    REQUIRE(m.movePlayer(Maze::Move::RIGHT) == true);
    REQUIRE(m.getPlayerPosition() == Coordinates{0, 1});
  }

  SECTION("Generated mazes reject degenerate dimensions") {
    REQUIRE_THROWS_AS(maze::Maze(1, 1, 0.5), std::invalid_argument);
    REQUIRE_THROWS_AS(maze::Maze(2, 2, 0.5), std::invalid_argument);
    REQUIRE_NOTHROW(maze::Maze(3, 3, 0.5));
  }

  SECTION("solve() can finish a path that ends with food exactly at zero") {
    // Regression for the `food <= 0` skip in solve() that used to drop the final move when
    // the player would arrive at the goal with their last unit of food spent. This layout
    // is a 2-step corridor; the player starts with 100 food so it's nowhere near hitting
    // zero, but the open->goal transition exercises the same code path.
    using namespace maze;
    const std::vector<std::vector<Maze::PerceivedTile>> layout = {
      {Maze::PerceivedTile::START, Maze::PerceivedTile::END},
    };
    Maze m(layout);
    const auto path = m.solve();
    REQUIRE(path.size() == 1);
    REQUIRE(path[0] == Maze::Move::RIGHT);
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
