/**
 * @file player.hpp
 * @brief Defines the Player class used in the maze game.
 */

#ifndef MAZE_PLAYER_HPP_
#define MAZE_PLAYER_HPP_

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
  Player(int maxWeight);

  /**
   * @brief Adds the weight of food to the player's inventory.
   * @param weight The weight of the food to add to the player's inventory.
   */
  void pickFood(int weight);

  /**
   * @brief Consumes a specified amount of food from the player's inventory.
   * @param amount The amount of food to consume.
   * @return True if the player has enough food to consume, false otherwise.
   */
  bool consumeFood(int amount);

  /**
   * @brief Returns the current weight of food in the player's inventory.
   * @return The current weight of food.
   */
  int getCurrentFood() const;

 private:
  int maxWeight; /**< The maximum weight of food that the player can carry. */
  int currentWeight; /**< The current weight of food in the player's inventory. */
};

}  // namespace maze

#endif  // MAZE_PLAYER_HPP_

