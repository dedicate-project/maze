#ifndef MAZE_PLAYER_HPP_
#define MAZE_PLAYER_HPP_

namespace maze {

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

}  // namespace maze

#endif  // MAZE_PLAYER_HPP_
