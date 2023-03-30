/**
 * @file tiles.hpp
 * @brief Defines the Tile classes used in the maze game.
 */

#ifndef MAZE_TILES_HPP_
#define MAZE_TILES_HPP_

// Standard
#include <memory>

namespace maze {

/**
 * @brief The base class for all tiles in the maze game.
 */
class Tile {
public:
  /**
   * @brief Returns whether or not the tile can be passed through.
   * @return True if the tile can be passed through, false otherwise.
   */
  virtual bool isPassable() const = 0;

  /**
   * @brief Creates a copy of the tile.
   * @return A unique_ptr to a new tile that is a copy of the current tile.
   */
  virtual std::unique_ptr<Tile> clone() const = 0;

  /**
   * @brief Virtual destructor for the Tile class.
   */
  virtual ~Tile() = default;
};

/**
 * @brief A tile that can be passed through.
 */
class EmptyTile : public Tile {
public:
  /**
   * @brief Returns true, indicating that this tile can always be passed through.
   * @return True
   */
  bool isPassable() const override;

  /**
   * @brief Creates a copy of the EmptyTile.
   * @return A unique_ptr to a new EmptyTile that is a copy of the current tile.
   */
  std::unique_ptr<Tile> clone() const override;
};

/**
 * @brief A tile that cannot be passed through.
 */
class WallTile : public Tile {
public:
  /**
   * @brief Returns false, indicating that this tile can never be passed through.
   * @return False
   */
  bool isPassable() const override;

  /**
   * @brief Creates a copy of the WallTile.
   * @return A unique_ptr to a new WallTile that is a copy of the current tile.
   */
  std::unique_ptr<Tile> clone() const override;
};

/**
 * @brief A tile that can be passed through, but only when a condition is met.
 */
class DoorTile : public Tile {
public:
  /**
   * @brief Returns true, indicating that this tile can be passed through.
   * @return True
   */
  bool isPassable() const override;

  /**
   * @brief Creates a copy of the DoorTile.
   * @return A unique_ptr to a new DoorTile that is a copy of the current tile.
   */
  std::unique_ptr<Tile> clone() const override;
};

/**
 * @brief A tile that represents food, which can be passed through and has a weight.
 */
class FoodTile : public Tile {
public:
  /**
   * @brief Constructs a new FoodTile with the given weight.
   * @param weight The weight of the food represented by the tile.
   */
  FoodTile(int weight);

  /**
   * @brief Returns true, indicating that this tile can always be passed through.
   * @return True
   */
  bool isPassable() const override;

  /**
   * @brief Returns the weight of the food represented by the tile.
   * @return The weight of the food.
   */
  int getWeight() const;

  /**
   * @brief Creates a copy of the FoodTile.
   * @return A unique_ptr to a new FoodTile that is a copy of the current tile.
   */
  std::unique_ptr<Tile> clone() const override;

private:
  int weight; /**< The weight of the food represented by the tile. */
};

}  // namespace maze

#endif  // MAZE_TILES_HPP_
