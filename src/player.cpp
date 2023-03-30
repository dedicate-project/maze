#include <maze/player.hpp>

namespace maze {

Player::Player(int maxWeight) : maxWeight(maxWeight), currentWeight(0) {}

void Player::pickFood(int weight) {
  currentWeight += weight;
  if (currentWeight > maxWeight) {
    currentWeight = maxWeight - weight;
  }
}

bool Player::consumeFood(int amount) {
  currentWeight -= amount;
  return currentWeight > 0;
}

int Player::getCurrentFood() const {
  return currentWeight;
}

} // namespace maze
