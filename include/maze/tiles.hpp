#ifndef MAZE_TILES_HPP_
#define MAZE_TILES_HPP_

// Standard
#include <memory>

namespace maze {

class Tile {
public:
  virtual bool isPassable() const = 0;
  virtual std::unique_ptr<Tile> clone() const = 0;
  virtual ~Tile() = default;
};

class EmptyTile : public Tile {
public:
  bool isPassable() const override { return true; }
  std::unique_ptr<Tile> clone() const override { return std::make_unique<EmptyTile>(*this); }
};

class WallTile : public Tile {
public:
  bool isPassable() const override { return false; }
  std::unique_ptr<Tile> clone() const override { return std::make_unique<WallTile>(*this); }
};

class DoorTile : public Tile {
public:
  bool isPassable() const override { return true; }
  std::unique_ptr<Tile> clone() const override { return std::make_unique<DoorTile>(*this); }
};

class FoodTile : public Tile {
public:
  FoodTile(int weight) : weight(weight) {}

  bool isPassable() const override { return true; }
  int getWeight() const { return weight; }
  std::unique_ptr<Tile> clone() const override { return std::make_unique<FoodTile>(*this); }

private:
  int weight;
};

}  // namespace maze

#endif  // MAZE_TILES_HPP_
