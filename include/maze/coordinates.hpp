#ifndef MAZE_COORDINATES_HPP_
#define MAZE_COORDINATES_HPP_

// Standard
#include <cstdint>
#include <functional>

namespace maze {

struct Coordinates {
  uint32_t row;
  uint32_t col;

  bool operator==(const Coordinates& other) const {
    return other.row == row && other.col == col;
  }

  bool operator!=(const Coordinates& other) const {
    return other.row != row || other.col != col;
  }

  bool operator<(const Coordinates &other) const {
    return (row < other.row) || (row == other.row && col < other.col);
  }
};

}  // namespace maze

namespace std {
  template <>
  struct hash<maze::Coordinates> {
    size_t operator()(const maze::Coordinates &c) const {
      return hash<int>()(c.row) ^ (hash<int>()(c.col) << 1);
    }
  };
}

#endif  // MAZE_COORDINATES_HPP_
