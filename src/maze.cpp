#include <maze/maze.hpp>

// Standard
#include <cmath>
#include <stdexcept>

namespace maze {

Maze::Maze(uint32_t rows, uint32_t cols, double difficulty)
  : rows(rows), cols(cols), player(100) {
  grid.resize(static_cast<std::vector<int>::size_type>(rows) * cols);
  generateMaze(difficulty);
}

bool Maze::isFinished() const {
  return playerPos == endPos;
}

Tile& Maze::getTile(uint32_t row, uint32_t col) const {
  return *grid[row * cols + col];
}

bool Maze::isPlayerAt(uint32_t row, uint32_t col) const {
  return row == playerPos.row && col == playerPos.col;
}

bool Maze::isStartAt(uint32_t row, uint32_t col) const {
  return row == startPos.row && col == startPos.col;
}

bool Maze::isEndAt(uint32_t row, uint32_t col) const {
  return row == endPos.row && col == endPos.col;
}

bool Maze::isInBounds(uint32_t row, uint32_t col) const {
  return row >= 0 && row < rows && col >= 0 && col < cols;
}

uint32_t Maze::getRows() const {
  return rows;
}

uint32_t Maze::getCols() const {
  return cols;
}

uint32_t Maze::getPlayerCurrentFood() const {
  return player.getCurrentFood();
}

bool Maze::movePlayer(Maze::Move move) {
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
    if (FoodTile* foodTile = dynamic_cast<FoodTile*>(tile)) {
      player.pickFood(foodTile->getWeight());
      grid[newPos.row * cols + newPos.col] = std::make_unique<EmptyTile>();
    }

    // Move the player and consume food.
    playerPos = newPos;
    return player.consumeFood(1);
  }

  return true;
}

bool Maze::isSolvable() {
  try {
    solve();
    return true;
  } catch (const std::runtime_error &) {
    return false;
  }
}

std::vector<Maze::Move> Maze::solve() {
  // Deep copy grid
  std::vector<std::unique_ptr<Tile>> localGrid;
  for (const auto &tile : grid) {
    localGrid.push_back(tile->clone());
  }

  // Use local versions of player, playerPos, and startPos
  Player localPlayer = player;
  Coordinates localPlayerPos = playerPos;
  Coordinates startPos = localPlayerPos;

  std::priority_queue<std::pair<int, Coordinates>, std::vector<std::pair<int, Coordinates>>, std::greater<>> openSet;
  std::unordered_set<Coordinates, std::hash<Coordinates>> closedSet;
  std::unordered_map<Coordinates, Coordinates, std::hash<Coordinates>> cameFrom;
  std::unordered_map<Coordinates, int, std::hash<Coordinates>> gScore;
  std::unordered_map<Coordinates, int, std::hash<Coordinates>> foodMap;

  openSet.push({manhattanDistance(startPos, endPos) + localPlayer.getCurrentFood(), startPos});
  gScore[startPos] = 0;
  foodMap[startPos] = localPlayer.getCurrentFood();

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

        auto tile = localGrid[neighbor.row * cols + neighbor.col].get();
        if (FoodTile* foodTile = dynamic_cast<FoodTile*>(tile)) {
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

Maze::Move Maze::getMoveFromCoords(const Coordinates &from, const Coordinates &to) {
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

void Maze::generateMaze(double difficulty) {
  // Fill the grid with wall tiles.
  for (uint32_t i = 0; i < rows; i++) {
    for (uint32_t j = 0; j < cols; j++) {
      if (j == 0 || i == 0 || j == cols - 1 || i == rows - 1) {
        grid[i * cols + j] = std::make_unique<WallTile>();
      } else {
        grid[i * cols + j] = std::make_unique<EmptyTile>();
      }
    }
  }

  // Lambda function for recursive depth-first search maze generation.
  const std::function<void(uint32_t, uint32_t)> dfs =
      [&](uint32_t row, uint32_t col) {
        // Set the current cell as an empty tile.
        grid[row * cols + col] = std::make_unique<EmptyTile>();

        // Randomly choose the order in which to visit neighbors.
        std::vector<std::pair<uint32_t, uint32_t>> neighbors = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
        std::shuffle(neighbors.begin(), neighbors.end(), std::mt19937(std::random_device()()));

        // Visit neighbors.
        for (const auto &[dr, dc] : neighbors) {
          const uint32_t newRow = row + dr;
          const uint32_t newCol = col + dc;
          if (newRow > 0 && newRow < rows - 1 && newCol > 0 && newCol < cols - 1 &&
              dynamic_cast<WallTile*>(&getTile(newRow, newCol))) {
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
  const uint32_t numWallsToAdd = static_cast<uint32_t>(difficulty * (rows - 2) * (cols - 2) / 5);
  std::random_device random_device;
  std::mt19937 rng(random_device());
  for (uint32_t i = 0; i < numWallsToAdd; i++) {
    uint32_t row = 1 + rng() % (rows - 2);
    uint32_t col = 1 + rng() % (cols - 2);
    if (dynamic_cast<EmptyTile*>(&getTile(row, col))) {
      grid[row * cols + col] = std::make_unique<WallTile>();
    }
  }

  // Place special tiles.
  const uint32_t numFoodItems = static_cast<int>((1 - difficulty) * (rows - 2) * (cols - 2) / 5);
  for (uint32_t i = 0; i < numFoodItems; i++) {
    uint32_t row, col;
    do {
      row = 1 + rng() % (rows - 2);
      col = 1 + rng() % (cols - 2);
    } while (!dynamic_cast<EmptyTile*>(&getTile(row, col)));
    grid[row * cols + col] = std::make_unique<FoodTile>(10 + rng() % 11);
  }

  // Place random doors based on difficulty.
  const uint32_t numDoors = static_cast<int>(difficulty * (rows + cols) / 4);
  for (uint32_t i = 0; i < numDoors; i++) {
    uint32_t row, col;
    do {
      row = 1 + rng() % (rows - 2);
      col = 1 + rng() % (cols - 2);
    } while (!dynamic_cast<EmptyTile*>(&getTile(row, col)));
    grid[row * cols + col] = std::make_unique<DoorTile>();
  }

  // Set random start and end positions on outer walls (excluding corners).
  std::vector<Coordinates> candidatePositions;
  for (uint32_t col = 1; col < cols - 1; col++) {
    candidatePositions.push_back({0, col});
    candidatePositions.push_back({rows - 1, col});
  }
  for (uint32_t row = 1; row < rows - 1; row++) {
    candidatePositions.push_back({row, 0});
    candidatePositions.push_back({row, cols - 1});
  }

  // Shuffle candidate positions.
  std::shuffle(candidatePositions.begin(), candidatePositions.end(), std::mt19937(random_device()));

  startPos = {0, 0};
  endPos = {0, 0};

  // Assign start and end positions.
  for (const auto& pos : candidatePositions) {
    if (startPos.row == 0 && startPos.col == 0 &&
        dynamic_cast<WallTile*>(&getTile(pos.row, pos.col))) {
      startPos = pos;
      grid[startPos.row * cols + startPos.col] = std::make_unique<EmptyTile>();
    } else if (endPos.row == 0 && endPos.col == 0 &&
               dynamic_cast<WallTile*>(&getTile(pos.row, pos.col)) && pos != startPos) {
      endPos = pos;
      grid[endPos.row * cols + endPos.col] = std::make_unique<EmptyTile>();
      break;
    }
  }

  // Place the player at the start position.
  playerPos = startPos;
}

std::vector<Coordinates> Maze::getNeighbors(const Coordinates &pos) {
  std::vector<Coordinates> neighbors;
  std::vector<std::pair<int32_t, int32_t>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  for (const auto &[dr, dc] : directions) {
    const int32_t newRow_signed = pos.row + dr;
    const int32_t newCol_signed = pos.col + dc;

    if (newRow_signed < 0 || newCol_signed < 0) {
      continue;
    }

    const uint32_t newRow = static_cast<uint32_t>(newRow_signed);
    const uint32_t newCol = static_cast<uint32_t>(newCol_signed);

    if (isInBounds(newRow, newCol) && getTile(newRow, newCol).isPassable()) {
      neighbors.push_back({newRow, newCol});
    }
  }

  return neighbors;
}

uint32_t Maze::manhattanDistance(const Coordinates &a, const Coordinates &b) {
  return
      static_cast<uint32_t>(std::abs(static_cast<int32_t>(a.row - b.row)) +
                            std::abs(static_cast<int32_t>(a.col - b.col)));
}

}  // namespace maze
