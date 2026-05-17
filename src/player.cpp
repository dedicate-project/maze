#include <maze/player.hpp>

// Standard
#include <algorithm>
#include <limits>

namespace maze {

Player::Player(uint32_t maxWeight)
  : maxWeight(maxWeight), currentWeight(maxWeight)
{
}

void Player::pickFood(uint32_t weight) {
  // Saturate at maxWeight. The previous implementation did `currentWeight += weight;` first
  // (which can wrap a uint32_t at INT_MAX), then assigned `maxWeight - weight` on overflow,
  // which is not "cap at max" -- e.g. with currentWeight=99, weight=10, maxWeight=100, the
  // old code would end up at 90 instead of 100. We now compute headroom first so neither
  // direction can underflow or wrap.
  const uint32_t headroom = maxWeight - std::min(currentWeight, maxWeight);
  currentWeight += std::min(weight, headroom);
}

bool Player::consumeFood(uint32_t amount) {
  // Saturating subtraction. The previous code did `currentWeight -= amount` unconditionally
  // which underflows uint32_t to ~4 billion when `amount > currentWeight`, then returned
  // `currentWeight > 0` (true!), leaving callers under the impression starvation never
  // happens. We now clamp at zero and return whether any food remains.
  if (amount >= currentWeight) {
    currentWeight = 0;
    return false;
  }
  currentWeight -= amount;
  return currentWeight > 0;
}

uint32_t Player::getCurrentFood() const {
  return currentWeight;
}

} // namespace maze
