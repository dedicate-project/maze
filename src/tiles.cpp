#include <maze/tiles.hpp>

namespace maze {

bool EmptyTile::isPassable() const { return true; }

std::unique_ptr<Tile> EmptyTile::clone() const {
  return std::make_unique<EmptyTile>(*this);
}

bool WallTile::isPassable() const { return false; }

std::unique_ptr<Tile> WallTile::clone() const {
  return std::make_unique<WallTile>(*this);
}

bool DoorTile::isPassable() const { return true; }

std::unique_ptr<Tile> DoorTile::clone() const {
  return std::make_unique<DoorTile>(*this);
}

FoodTile::FoodTile(uint32_t weight) : weight(weight) {}

bool FoodTile::isPassable() const { return true; }

uint32_t FoodTile::getWeight() const { return weight; }

std::unique_ptr<Tile> FoodTile::clone() const {
  return std::make_unique<FoodTile>(*this);
}

}  // namespace maze

