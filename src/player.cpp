#include <maze/player.hpp>

namespace maze {

Player::Player(uint32_t maxWeight)
  : maxWeight(maxWeight), currentWeight(0)
{
}

void Player::pickFood(uint32_t weight) {
  currentWeight += weight;
  if (currentWeight > maxWeight) {
    currentWeight = maxWeight - weight;
  }
}

bool Player::consumeFood(uint32_t amount) {
  currentWeight -= amount;
  return currentWeight > 0;
}

uint32_t Player::getCurrentFood() const {
  return currentWeight;
}

} // namespace maze
