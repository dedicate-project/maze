#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

struct Coordinates {
  int row;
  int col;

  bool operator==(const Coordinates& other) const {
    return other.row == row && other.col == col;
  }

  bool operator!=(const Coordinates& other) const {
    return other.row != row || other.col != col;
  }

  bool operator<(const Coordinates &other) const {
    return (row < other.row) || (row == other.row && col < other.col);
  }
};

namespace std {
  template <>
  struct hash<Coordinates> {
    size_t operator()(const Coordinates &c) const {
      return hash<int>()(c.row) ^ (hash<int>()(c.col) << 1);
    }
  };
}

class Tile {
public:
  virtual bool isPassable() const = 0;
  virtual ~Tile() = default;
};

class EmptyTile : public Tile {
public:
  bool isPassable() const override { return true; }
};

class WallTile : public Tile {
public:
  bool isPassable() const override { return false; }
};

class DoorTile : public Tile {
public:
  bool isPassable() const override { return true; }
};

class FoodTile : public Tile {
public:
  FoodTile(int weight) : weight(weight) {}

  bool isPassable() const override { return true; }
  int getWeight() const { return weight; }

private:
  int weight;
};

class Player {
 public:
  Player(int maxWeight) : maxWeight(maxWeight), currentWeight(0) {}

  void pickFood(int weight) {
    currentWeight += weight;
    if (currentWeight > maxWeight) currentWeight = maxWeight - weight;
  }

  bool consumeFood(int amount) {
    currentWeight -= amount;
    return currentWeight > 0;
  }

  int getCurrentFood() const {
    return currentWeight;
  }

 private:
  int maxWeight;
  int currentWeight;
};

class Maze {
public:
  enum class Move { LEFT, RIGHT, UP, DOWN };

  Maze(int rows, int cols, float difficulty)
    : rows(rows), cols(cols), player(100) {
    grid.resize(rows * cols);
    generateMaze(difficulty);
  }

  bool movePlayer(Move move) {
    // Calculate the new position based on the move.
    Coordinates newPos = playerPos;
    switch (move) {
    case Move::LEFT:
      newPos.col--;
      break;
    case Move::RIGHT:
      newPos.col++;
      break;
    case Move::UP:
      newPos.row--;
      break;
    case Move::DOWN:
      newPos.row++;
      break;
    }

    // Check if the new position is within bounds and passable.
    if (newPos.row >= 0 && newPos.row < rows && newPos.col >= 0 && newPos.col < cols &&
        grid[newPos.row * cols + newPos.col]->isPassable()) {
      auto tile = grid[newPos.row * cols + newPos.col].get();

      // Handle special tiles.
      if (FoodTile *foodTile = dynamic_cast<FoodTile *>(tile)) {
        player.pickFood(foodTile->getWeight());
      }

      // Move the player and consume food.
      playerPos = newPos;
      return player.consumeFood(1);
    }

    return true;
  }

  bool isFinished() const { return playerPos == endPos; }

  Tile &getTile(int row, int col) const {
    return *grid[row * cols + col];
  }

  bool isPlayerAt(int row, int col) const {
    return row == playerPos.row && col == playerPos.col;
  }

  bool isStartAt(int row, int col) const {
    return row == startPos.row && col == startPos.col;
  }

  bool isEndAt(int row, int col) const {
    return row == endPos.row && col == endPos.col;
  }

  bool isInBounds(int row, int col) const {
    return row >= 0 && row < rows && col >= 0 && col < cols;
  }

  int getRows() const {
    return rows;
  }

  int getCols() const {
    return cols;
  }

  int getPlayerCurrentFood() const {
    return player.getCurrentFood();
  }

  bool isSolvable() {
    try {
      solve();
      return true;
    } catch (const std::runtime_error &) {
      return false;
    }
  }

  std::vector<Move> solve() {
    std::priority_queue<std::pair<int, Coordinates>, std::vector<std::pair<int, Coordinates>>, std::greater<>> openSet;
    std::unordered_set<Coordinates, std::hash<Coordinates>> closedSet;
    std::unordered_map<Coordinates, Coordinates, std::hash<Coordinates>> cameFrom;
    std::unordered_map<Coordinates, int, std::hash<Coordinates>> gScore;
    std::unordered_map<Coordinates, int, std::hash<Coordinates>> foodMap;

    openSet.push({manhattanDistance(startPos, endPos) + player.getCurrentFood(), startPos});
    gScore[startPos] = 0;
    foodMap[startPos] = player.getCurrentFood();

    while (!openSet.empty()) {
      Coordinates current = openSet.top().second;
      openSet.pop();

      if (current == endPos) {
        std::vector<Move> path;
        while (cameFrom.count(current) > 0) {
          Coordinates previous = cameFrom[current];
          path.push_back(getMoveFromCoords(previous, current));
          current = previous;
        }

        std::reverse(path.begin(), path.end());
        return path;
      }

      if (closedSet.count(current) > 0) {
        continue;
      }

      closedSet.insert(current);

      for (const auto &neighbor : getNeighbors(current)) {
        if (closedSet.count(neighbor) == 0) {
          int tentativeGScore = gScore[current] + 1;
          int food = foodMap[current] - 1;

          auto tile = grid[neighbor.row * cols + neighbor.col].get();
          if (FoodTile *foodTile = dynamic_cast<FoodTile *>(tile)) {
            food += foodTile->getWeight();
          }

          if (food <= 0) {
            continue;
          }

          if (gScore.count(neighbor) == 0 || tentativeGScore < gScore[neighbor]) {
            cameFrom[neighbor] = current;
            gScore[neighbor] = tentativeGScore;
            foodMap[neighbor] = food;
            int fScore = tentativeGScore + manhattanDistance(neighbor, endPos) + food;
            openSet.push({fScore, neighbor});
          }
        }
      }
    }

    throw std::runtime_error("Maze is not solvable");
  }

 private:
  Move getMoveFromCoords(const Coordinates &from, const Coordinates &to) {
    if (from.row == to.row) {
      if (from.col < to.col) {
        return Move::RIGHT;
      } else {
        return Move::LEFT;
      }
    } else {
      if (from.row < to.row) {
        return Move::DOWN;
      } else {
        return Move::UP;
      }
    }
  }

  void generateMaze(float difficulty) {
    // Fill the grid with wall tiles.
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
          if (j == 0 || i == 0 || j == cols - 1 || i == rows - 1) {
            grid[i * cols + j] = std::make_unique<WallTile>();
          } else {
            grid[i * cols + j] = std::make_unique<EmptyTile>();
          }
        }
    }

    // Lambda function for recursive depth-first search maze generation.
    std::function<void(int, int)> dfs = [&](int row, int col) {
                                          // Set the current cell as an empty tile.
                                          grid[row * cols + col] = std::make_unique<EmptyTile>();

                                          // Randomly choose the order in which to visit neighbors.
                                          std::vector<std::pair<int, int>> neighbors = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
                                          std::shuffle(neighbors.begin(), neighbors.end(), std::mt19937(std::random_device()()));

                                          // Visit neighbors.
                                          for (const auto &[dr, dc] : neighbors) {
                                            int newRow = row + dr;
                                            int newCol = col + dc;
                                            if (newRow > 0 && newRow < rows - 1 && newCol > 0 && newCol < cols - 1 && dynamic_cast<WallTile *>(&getTile(newRow, newCol))) {
                                              // Remove the wall between cells.
                                              grid[(row + newRow) / 2 * cols + (col + newCol) / 2] = std::make_unique<EmptyTile>();

                                              // Continue generating the maze from the neighbor.
                                              dfs(newRow, newCol);
                                            }
                                          }
                                        };

    // Generate the maze layout.
    dfs(1, 1);

    // Add random walls based on the difficulty.
    int numWallsToAdd = static_cast<int>(difficulty * (rows - 2) * (cols - 2) / 5);
    for (int i = 0; i < numWallsToAdd; i++) {
        int row = 1 + rand() % (rows - 2);
        int col = 1 + rand() % (cols - 2);
        if (dynamic_cast<EmptyTile *>(&getTile(row, col))) {
            grid[row * cols + col] = std::make_unique<WallTile>();
        }
    }

    // Place special tiles.
    int numFoodItems = static_cast<int>((1 - difficulty) * (rows - 2) * (cols - 2) / 5);
    for (int i = 0; i < numFoodItems; i++) {
        int row, col;
        do {
            row = 1 + rand() % (rows - 2);
            col = 1 + rand() % (cols - 2);
        } while (!dynamic_cast<EmptyTile *>(&getTile(row, col)));
        grid[row * cols + col] = std::make_unique<FoodTile>(10 + rand() % 11);
    }

    // Place random doors based on difficulty.
    int numDoors = static_cast<int>(difficulty * (rows + cols) / 4);
    for (int i = 0; i < numDoors; i++) {
        int row, col;
        do {
            row = 1 + rand() % (rows - 2);
            col = 1 + rand() % (cols - 2);
        } while (!dynamic_cast<EmptyTile *>(&getTile(row, col)));
        grid[row * cols + col] = std::make_unique<DoorTile>();
    }

    // Set random start and end positions on outer walls (excluding corners).
    std::vector<Coordinates> candidatePositions;
    for (int col = 1; col < cols - 1; col++) {
      candidatePositions.push_back({0, col});
      candidatePositions.push_back({rows - 1, col});
    }
    for (int row = 1; row < rows - 1; row++) {
      candidatePositions.push_back({row, 0});
      candidatePositions.push_back({row, cols - 1});
    }

    // Shuffle candidate positions.
    std::shuffle(candidatePositions.begin(), candidatePositions.end(), std::mt19937(std::random_device()()));

    startPos = {0, 0};
    endPos = {0, 0};

    // Assign start and end positions.
    for (const auto& pos : candidatePositions) {
      if (startPos.row == 0 && startPos.col == 0 && dynamic_cast<WallTile*>(&getTile(pos.row, pos.col))) {
        startPos = pos;
        grid[startPos.row * cols + startPos.col] = std::make_unique<EmptyTile>();
      } else if (endPos.row == 0 && endPos.col == 0 && dynamic_cast<WallTile*>(&getTile(pos.row, pos.col)) && pos != startPos) {
        endPos = pos;
        grid[endPos.row * cols + endPos.col] = std::make_unique<EmptyTile>();
        break;
      }
    }

    // Place the player at the start position.
    playerPos = startPos;
  }

  std::vector<Coordinates> getNeighbors(const Coordinates &pos) {
    std::vector<Coordinates> neighbors;
    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (const auto &[dr, dc] : directions) {
      int newRow = pos.row + dr;
      int newCol = pos.col + dc;

      if (isInBounds(newRow, newCol) && getTile(newRow, newCol).isPassable()) {
        neighbors.push_back({newRow, newCol});
      }
    }

    return neighbors;
  }

  int manhattanDistance(const Coordinates &a, const Coordinates &b) {
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
  }

  int rows;
  int cols;
  std::vector<std::unique_ptr<Tile>> grid;
  Player player;
  Coordinates startPos;
  Coordinates endPos;
  Coordinates playerPos;
};

void printMaze(const Maze &maze) {
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
        if (dynamic_cast<FoodTile *>(&maze.getTile(i, j))) {
          std::cout << "F";
        } else if (dynamic_cast<DoorTile *>(&maze.getTile(i, j))) {
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

Maze getSolvableMaze(uint32_t rows, uint32_t cols, float difficulty, uint32_t max_tries) {
  srand(time(0));
  uint32_t tries = 0;

  do {
    Maze maze(rows, cols, difficulty);
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

std::string moveToString(Maze::Move move) {
  switch (move) {
  case Maze::Move::LEFT:
    return "LEFT";
  case Maze::Move::RIGHT:
    return "RIGHT";
  case Maze::Move::UP:
    return "UP";
  case Maze::Move::DOWN:
    return "DOWN";
  }
  return "<>";
}

int main() {
  Maze maze = getSolvableMaze(20, 20, 0.2, 10);
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
