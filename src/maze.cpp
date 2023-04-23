#include <maze/maze.hpp>

// Standard
#include <cmath>
#include <stdexcept>

namespace maze {

namespace {
int squaredDistance(int x1, int y1, int x2, int y2) {
  int deltaX = x2 - x1;
  int deltaY = y2 - y1;
  return deltaX * deltaX + deltaY * deltaY;
}
} // namespace

Maze::Maze(uint32_t rows, uint32_t cols, double difficulty)
  : rows_(rows), cols_(cols), player_(100) {
  grid_.resize(static_cast<std::vector<int>::size_type>(rows) * cols);
  generateMaze(difficulty);
}

Maze::Maze(const std::vector<std::vector<PerceivedTile>>& maze_layout)
  : player_(100) {
  // Sanity check rows and cols counts.
  const uint32_t rows = maze_layout.size();
  if (rows == 0) {
    throw std::invalid_argument("A maze needs to have at least one row.");
  }
  const uint32_t cols = maze_layout.front().size();
  if (cols == 0) {
    throw std::invalid_argument("A maze needs to have at least one column.");
  }
  for (const std::vector<PerceivedTile>& col : maze_layout) {
    if (col.size() != cols) {
      throw std::invalid_argument("All rows need to have the same amount of columns.");
    }
  }

  rows_ = rows;
  cols_ = cols;

  grid_.resize(static_cast<std::vector<int>::size_type>(rows) * cols);
  for (const std::vector<PerceivedTile>& col : maze_layout) {
    for (const PerceivedTile& tile : col) {
      // ...
    }
  }
}

bool Maze::isFinished() const {
  return player_pos_ == end_pos_;
}

Tile& Maze::getTile(uint32_t row, uint32_t col) const {
  return *grid_[row * cols_ + col];
}

bool Maze::isPlayerAt(uint32_t row, uint32_t col) const {
  return row == player_pos_.row && col == player_pos_.col;
}

bool Maze::isStartAt(uint32_t row, uint32_t col) const {
  return row == start_pos_.row && col == start_pos_.col;
}

bool Maze::isEndAt(uint32_t row, uint32_t col) const {
  return row == end_pos_.row && col == end_pos_.col;
}

bool Maze::isInBounds(uint32_t row, uint32_t col) const {
  return row >= 0 && row < rows_ && col >= 0 && col < cols_;
}

uint32_t Maze::getRows() const {
  return rows_;
}

uint32_t Maze::getCols() const {
  return cols_;
}

uint32_t Maze::getPlayerCurrentFood() const {
  return player_.getCurrentFood();
}

bool Maze::movePlayer(Maze::Move move) {
  // Calculate the new position based on the move.
  Coordinates newPos = player_pos_;
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
  if (newPos.row >= 0 && newPos.row < rows_ && newPos.col >= 0 && newPos.col < cols_ &&
      grid_[newPos.row * cols_ + newPos.col]->isPassable()) {
    auto tile = grid_[newPos.row * cols_ + newPos.col].get();

    // Handle special tiles.
    if (FoodTile* foodTile = dynamic_cast<FoodTile*>(tile)) {
      player_.pickFood(foodTile->getWeight());
      grid_[newPos.row * cols_ + newPos.col] = std::make_shared<EmptyTile>();
    }

    // Move the player and consume food.
    player_pos_ = newPos;
    return player_.consumeFood(1);
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
  std::vector<std::shared_ptr<Tile>> localGrid;
  for (const auto &tile : grid_) {
    localGrid.push_back(tile->clone());
  }

  // Use local versions of player_, player_pos_, and start_pos_
  Player localPlayer = player_;
  Coordinates localPlayerPos = player_pos_;
  Coordinates start_pos = localPlayerPos;

  std::priority_queue<std::pair<int, Coordinates>, std::vector<std::pair<int, Coordinates>>, std::greater<>> openSet;
  std::unordered_set<Coordinates, std::hash<Coordinates>> closedSet;
  std::unordered_map<Coordinates, Coordinates, std::hash<Coordinates>> cameFrom;
  std::unordered_map<Coordinates, int, std::hash<Coordinates>> gScore;
  std::unordered_map<Coordinates, int, std::hash<Coordinates>> foodMap;

  openSet.push({manhattanDistance(start_pos, end_pos_) + localPlayer.getCurrentFood(), start_pos});
  gScore[start_pos] = 0;
  foodMap[start_pos] = localPlayer.getCurrentFood();

  while (!openSet.empty()) {
    Coordinates current = openSet.top().second;
    openSet.pop();

    if (current == end_pos_) {
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

        auto tile = localGrid[neighbor.row * cols_ + neighbor.col].get();
        if (FoodTile* foodTile = dynamic_cast<FoodTile*>(tile)) {
          food += foodTile->getWeight();
          // Update the tile to an empty tile after the food is consumed
          localGrid[neighbor.row * cols_ + neighbor.col] = std::make_shared<EmptyTile>();
        }

        if (food <= 0) {
          continue;
        }

        if (gScore.count(neighbor) == 0 || tentativeGScore < gScore[neighbor]) {
          cameFrom[neighbor] = current;
          gScore[neighbor] = tentativeGScore;
          foodMap[neighbor] = food;
          int fScore = tentativeGScore + manhattanDistance(neighbor, end_pos_) + food;
          openSet.push({fScore, neighbor});
        }
      }
    }
  }

  throw std::runtime_error("Maze is not solvable");
}

bool Maze::blocksLineOfSight(const std::shared_ptr<Tile>& tile) const {
  return std::dynamic_pointer_cast<WallTile>(tile) != nullptr;
}

bool Maze::lineOfSight(int startX, int startY, int endX, int endY) const {
  int diffX = abs(endX - startX);
  int diffY = abs(endY - startY);
  int stepX = (startX < endX) ? 1 : -1;
  int stepY = (startY < endY) ? 1 : -1;
  int error = diffX - diffY;

  while (true) {
    if (blocksLineOfSight(grid_[startY * cols_ + startX])) {
      return false;
    }

    if (startX == endX && startY == endY) {
      break;
    }

    int error2 = error * 2;
    if (error2 > -diffY) {
      error -= diffY;
      startX += stepX;
    }
    if (error2 < diffX) {
      error += diffX;
      startY += stepY;
    }
  }

  return true;
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

Coordinates Maze::getStartPosition() const {
  return start_pos_;
}

Coordinates Maze::getEndPosition() const {
  return end_pos_;
}

Coordinates Maze::getPlayerPosition() const {
  return player_pos_;
}

std::vector<std::vector<Maze::PerceivedTile>> Maze::perceiveTiles(uint32_t radius) {
  const uint32_t vector_size = radius * 2 + 1;
  std::vector<std::vector<PerceivedTile>> perceived_rows;
  perceived_rows.resize(vector_size);
  for (std::vector<PerceivedTile>& perceived_col : perceived_rows) {
    perceived_col.resize(vector_size, PerceivedTile::UNKNOWN);
  }

  const uint32_t squaredRadius = radius * radius;
  for (int32_t row = player_pos_.row - radius; row <= player_pos_.row + radius; ++row) {
    if (row < 0 || row >= rows_) {
      continue;
    }
    for (int32_t col = player_pos_.col - radius; col <= player_pos_.col; ++col) {
      if (col < 0 || col >= cols_) {
        continue;
      }

      if (squaredDistance(player_pos_.col, player_pos_.row, col, row) > squaredRadius) {
        continue;
      }

      if (!lineOfSight(player_pos_.col, player_pos_.row, col, row)) {
        continue;
      }

      if (start_pos_.row == row && start_pos_.col == col) {
        perceived_rows[row][col] = PerceivedTile::START;
      } else if (end_pos_.row == row && end_pos_.col == col) {
        perceived_rows[row][col] = PerceivedTile::END;
      } else {
        const std::shared_ptr<Tile> tile = grid_[row * cols_ + col];
        if (std::dynamic_pointer_cast<WallTile>(tile)) {
          perceived_rows[row][col] = PerceivedTile::WALL;
        } else if (std::dynamic_pointer_cast<DoorTile>(tile)) {
          perceived_rows[row][col] = PerceivedTile::DOOR;
        } else if (std::dynamic_pointer_cast<FoodTile>(tile)) {
          perceived_rows[row][col] = PerceivedTile::FOOD;
        } else if (std::dynamic_pointer_cast<EmptyTile>(tile)) {
          perceived_rows[row][col] = PerceivedTile::EMPTY;
        }
      }
    }
  }

  return perceived_rows;
}

void Maze::generateMaze(double difficulty) {
  // Fill the grid with wall tiles.
  for (uint32_t i = 0; i < rows_; i++) {
    for (uint32_t j = 0; j < cols_; j++) {
      if (j == 0 || i == 0 || j == cols_ - 1 || i == rows_ - 1) {
        grid_[i * cols_ + j] = std::make_shared<WallTile>();
      } else {
        grid_[i * cols_ + j] = std::make_shared<EmptyTile>();
      }
    }
  }

  // Lambda function for recursive depth-first search maze generation.
  const std::function<void(uint32_t, uint32_t)> dfs =
      [&](uint32_t row, uint32_t col) {
        // Set the current cell as an empty tile.
        grid_[row * cols_ + col] = std::make_shared<EmptyTile>();

        // Randomly choose the order in which to visit neighbors.
        std::vector<std::pair<uint32_t, uint32_t>> neighbors = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
        std::shuffle(neighbors.begin(), neighbors.end(), std::mt19937(std::random_device()()));

        // Visit neighbors.
        for (const auto &[dr, dc] : neighbors) {
          const uint32_t newRow = row + dr;
          const uint32_t newCol = col + dc;
          if (newRow > 0 && newRow < rows_ - 1 && newCol > 0 && newCol < cols_ - 1 &&
              dynamic_cast<WallTile*>(&getTile(newRow, newCol))) {
            // Remove the wall between cells.
            grid_[(row + newRow) / 2 * cols_ + (col + newCol) / 2] = std::make_shared<EmptyTile>();

            // Continue generating the maze from the neighbor.
            dfs(newRow, newCol);
          }
        }
      };

  // Generate the maze layout.
  dfs(1, 1);

  // Add random walls based on the difficulty.
  const uint32_t numWallsToAdd = static_cast<uint32_t>(difficulty * (rows_ - 2) * (cols_ - 2) / 5);
  std::random_device random_device;
  std::mt19937 rng(random_device());
  for (uint32_t i = 0; i < numWallsToAdd; i++) {
    uint32_t row = 1 + rng() % (rows_ - 2);
    uint32_t col = 1 + rng() % (cols_ - 2);
    if (dynamic_cast<EmptyTile*>(&getTile(row, col))) {
      grid_[row * cols_ + col] = std::make_shared<WallTile>();
    }
  }

  // Place special tiles.
  const uint32_t numFoodItems = static_cast<int>((1 - difficulty) * (rows_ - 2) * (cols_ - 2) / 5);
  for (uint32_t i = 0; i < numFoodItems; i++) {
    uint32_t row, col;
    do {
      row = 1 + rng() % (rows_ - 2);
      col = 1 + rng() % (cols_ - 2);
    } while (!dynamic_cast<EmptyTile*>(&getTile(row, col)));
    grid_[row * cols_ + col] = std::make_shared<FoodTile>(10 + rng() % 11);
  }

  // Place random doors based on difficulty.
  const uint32_t numDoors = static_cast<int>(difficulty * (rows_ + cols_) / 4);
  for (uint32_t i = 0; i < numDoors; i++) {
    uint32_t row, col;
    do {
      row = 1 + rng() % (rows_ - 2);
      col = 1 + rng() % (cols_ - 2);
    } while (!dynamic_cast<EmptyTile*>(&getTile(row, col)));
    grid_[row * cols_ + col] = std::make_shared<DoorTile>();
  }

  // Set random start and end positions on outer walls (excluding corners).
  std::vector<Coordinates> candidatePositions;
  for (uint32_t col = 1; col < cols_ - 1; col++) {
    candidatePositions.push_back({0, col});
    candidatePositions.push_back({rows_ - 1, col});
  }
  for (uint32_t row = 1; row < rows_ - 1; row++) {
    candidatePositions.push_back({row, 0});
    candidatePositions.push_back({row, cols_ - 1});
  }

  // Shuffle candidate positions.
  std::shuffle(candidatePositions.begin(), candidatePositions.end(), std::mt19937(random_device()));

  start_pos_ = {0, 0};
  end_pos_ = {0, 0};

  // Assign start and end positions.
  for (const auto& pos : candidatePositions) {
    if (start_pos_.row == 0 && start_pos_.col == 0 &&
        dynamic_cast<WallTile*>(&getTile(pos.row, pos.col))) {
      start_pos_ = pos;
      grid_[start_pos_.row * cols_ + start_pos_.col] = std::make_shared<EmptyTile>();
    } else if (end_pos_.row == 0 && end_pos_.col == 0 &&
               dynamic_cast<WallTile*>(&getTile(pos.row, pos.col)) && pos != start_pos_) {
      end_pos_ = pos;
      grid_[end_pos_.row * cols_ + end_pos_.col] = std::make_shared<EmptyTile>();
      break;
    }
  }

  // Place the player at the start position.
  player_pos_ = start_pos_;
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
