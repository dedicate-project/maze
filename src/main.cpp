// Standard
#include <iostream>

// Maze
#include <maze/maze.hpp>

void printMaze(const maze::Maze &maze) {
  int rows = maze.getRows();
  int cols = maze.getCols();

  std::cout << "Maze:\n";
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (maze.isPlayerAt(i, j)) {
        std::cout << "X";
      } else if (maze.isStartAt(i, j)) {
        std::cout << "o";
      } else if (maze.isEndAt(i, j)) {
        std::cout << "O";
      } else if (maze.getTile(i, j).isPassable()) {
        if (dynamic_cast<maze::FoodTile *>(&maze.getTile(i, j))) {
          std::cout << "F";
        } else if (dynamic_cast<maze::DoorTile *>(&maze.getTile(i, j))) {
          std::cout << "D";
        } else {
          std::cout << ".";
        }
      } else {
        std::cout << "#";
      }
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

maze::Maze getSolvableMaze(uint32_t rows, uint32_t cols, float difficulty, uint32_t max_tries) {
  srand(time(0));
  uint32_t tries = 0;

  do {
    maze::Maze maze(rows, cols, difficulty);
    printMaze(maze);
    if (maze.isSolvable()) {
      return maze;
    }
    tries++;
  } while(tries < max_tries);

  throw std::runtime_error(std::string("Failed to generate a solvable maze with these parameters: ")
                           + "rows=" + std::to_string(rows) + ", cols=" + std::to_string(cols)
                           + ", difficulty=" + std::to_string(difficulty)
                           + ", max_tries=" + std::to_string(max_tries));
}

std::string moveToString(maze::Maze::Move move) {
  switch (move) {
  case maze::Maze::Move::LEFT:
    return "LEFT";
  case maze::Maze::Move::RIGHT:
    return "RIGHT";
  case maze::Maze::Move::UP:
    return "UP";
  case maze::Maze::Move::DOWN:
    return "DOWN";
  }
  return "<>";
}

int main() {
  maze::Maze maze = getSolvableMaze(20, 20, 0.2, 10);
  printMaze(maze);

  auto path = maze.solve();
  for (const auto &move : path) {
    std::cout << moveToString(move) << std::endl;
  }

  for (const auto &move : path) {
    if (!maze.movePlayer(move)) {
      std::cout << "Game over: Out of food!\n";
      break;
    }

    //printMaze(maze);

    if (maze.isFinished()) {
      std::cout << "Congratulations, you reached the end of the maze!\n";
      break;
    }
  }

  return 0;
}
