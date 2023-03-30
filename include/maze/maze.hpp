/**
 * @file maze.hpp
 * @brief Defines the Maze class used in the maze game.
 */

#ifndef MAZE_MAZE_HPP_
#define MAZE_MAZE_HPP_

// Standard
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

// Private
#include "coordinates.hpp"
#include "player.hpp"
#include "tiles.hpp"

namespace maze {

/**
 * @brief The Maze class represents the maze in the maze game.
 */
class Maze {
 public:
  /**
   * @brief The possible moves that the player can make.
   */
  enum class Move { LEFT, RIGHT, UP, DOWN };

  /**
   * @brief Constructs a new Maze with the given number of rows, columns, and difficulty.
   * @param rows The number of rows in the maze.
   * @param cols The number of columns in the maze.
   * @param difficulty The difficulty of the maze, represented as a value between 0 and 1.
   */
  Maze(int rows, int cols, float difficulty);

  /**
   * @brief Moves the player in the specified direction.
   * @param move The direction to move the player.
   * @return True if the move was successful, false otherwise.
   */
  bool movePlayer(Move move);

  /**
   * @brief Returns whether or not the maze has been completed.
   * @return True if the maze has been completed, false otherwise.
   */
  bool isFinished() const;

  /**
   * @brief Returns the tile at the specified position in the maze.
   * @param row The row of the tile to retrieve.
   * @param col The column of the tile to retrieve.
   * @return A reference to the tile at the specified position.
   */
  Tile& getTile(int row, int col) const;

  /**
   * @brief Returns whether or not the player is at the specified position in the maze.
   * @param row The row to check.
   * @param col The column to check.
   * @return True if the player is at the specified position, false otherwise.
   */
  bool isPlayerAt(int row, int col) const;

  /**
   * @brief Returns whether or not the start of the maze is at the specified position.
   * @param row The row to check.
   * @param col The column to check.
   * @return True if the start of the maze is at the specified position, false otherwise.
   */
  bool isStartAt(int row, int col) const;

  /**
   * @brief Returns whether or not the end of the maze is at the specified position.
   * @param row The row to check.
   * @param col The column to check.
   * @return True if the end of the maze is at the specified position, false otherwise.
   */
  bool isEndAt(int row, int col) const;

  /**
   * @brief Returns whether or not the specified position is within the bounds of the maze.
   * @param row The row to check.
   * @param col The column to check.
   * @return True if the position is within the bounds of the maze, false otherwise.
   */
  bool isInBounds(int row, int col) const;

  /**
   * @brief Returns the number of rows in the maze.
   * @return The number of rows in the maze.
   */
  int getRows() const;

  /**
   * @brief Returns the number of columns in the maze.
   * @return The number of columns in the maze.
   */
  int getCols() const;

  /**
   * @brief Returns the current amount of food in the player's inventory.
   * @return The current amount of food in the player's inventory.
   */
  int getPlayerCurrentFood() const;

  /**
   * @brief Determines whether or not the maze is solvable.
   * @return True if the maze is solvable, false otherwise.
   */
  bool isSolvable();

  /**
   * @brief Solves the maze and returns a vector of moves to get from start to end.
   * @return A vector of moves to get from start to end.
   */
  std::vector<Move> solve();

 private:
  /**
   * @brief Returns the move that goes from the "from" position to the "to" position.
   * @param from The starting position.
   * @param to The ending position.
   * @return The move that goes from the "from" position to the "to" position.
   */
  Move getMoveFromCoords(const Coordinates &from, const Coordinates &to);

  /**
   * @brief Generates the maze with the specified difficulty.
   * @param difficulty The difficulty of the maze, represented as a value between 0 and 1.
   */
  void generateMaze(float difficulty);

  /**
   * @brief Returns a vector of the neighboring positions of the specified position.
   * @param pos The position to find neighbors for.
   * @return A vector of the neighboring positions of the specified position.
   */
  std::vector<Coordinates> getNeighbors(const Coordinates &pos);

  /**
   * @brief Returns the Manhattan distance between two positions.
   * @param a The first position.
   * @param b The second position.
   * @return The Manhattan distance between the two positions.
   */
  int manhattanDistance(const Coordinates &a, const Coordinates &b);

  int rows; /**< The number of rows in the maze. */
  int cols; /**< The number of columns in the maze. */
  std::vector<std::unique_ptr<Tile>> grid; /**< The grid of tiles that make up the maze. */
  Player player; /**< The player object. */
  Coordinates startPos; /**< The starting position of the maze. */
  Coordinates endPos; /**< The ending position of the maze. */
  Coordinates playerPos; /**< The current position of the player in the maze. */
};

}  // namespace maze

#endif  // MAZE_MAZE_HPP_

