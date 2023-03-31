/**
 * @file player.hpp
 * @brief Defines the Player class used in the maze game.
 */

#ifndef MAZE_PLAYER_HPP_
#define MAZE_PLAYER_HPP_

// Standard
#include <cstdint>

namespace maze {

/**
 * @brief The Player class represents the player in the maze game.
 */
class Player {
 public:
  /**
   * @brief Constructs a new Player with the given maximum weight of food it can carry.
   * @param maxWeight The maximum weight of food that the player can carry.
   */
  Player(uint32_t maxWeight);

  /**
   * @brief Adds the weight of food to the player's inventory.
   * @param weight The weight of the food to add to the player's inventory.
   */
  void pickFood(uint32_t weight);

  /**
   * @brief Consumes a specified amount of food from the player's inventory.
   * @param amount The amount of food to consume.
   * @return True if the player has enough food to consume, false otherwise.
   */
  bool consumeFood(uint32_t amount);

  /**
   * @brief Returns the current weight of food in the player's inventory.
   * @return The current weight of food.
   */
  uint32_t getCurrentFood() const;

 private:
  uint32_t maxWeight; /**< The maximum weight of food that the player can carry. */
  uint32_t currentWeight; /**< The current weight of food in the player's inventory. */
};

}  // namespace maze

#endif  // MAZE_PLAYER_HPP_

