#ifndef MAZE_HPP
#define MAZE_HPP

#include <array>
#include <cstdio>
#include <list>
#include <string>
#include <vector>

// TODO: Add 'invisible walls' field for Cell enum
//  - Do the necessary changes in Maze constructor to support this changes
// TODO: Change symbols for maze in file to '#' (wall), '-' (invisible wall), '
// ' (Free), 'S' (Start), 'F' (Finish)

namespace snaze {
/// A enum to represent directions in a cartesian style
enum class Direction { Up, Down, Left, Right, None };
/// Data structure that represents a cartesian coordinate
struct Position {
    size_t coord_x;
    size_t coord_y;
    /// Constructor
    Position(size_t x, size_t y) : coord_x(x), coord_y(y) {}
    /// Assign overload
    Position &operator=(const Position &rhs) {
        if (this != &rhs) {
            coord_x = rhs.coord_x;
            coord_y = rhs.coord_y;
        }
        return *this;
    }
    /// Equality overload
    bool operator==(const Position &rhs) const {
        return coord_x == rhs.coord_x and coord_y == rhs.coord_y;
    }
    /// Inequality overload
    bool operator!=(const Position &rhs) const { return not(*this == rhs); }
    /// (+) overload
    Position operator+(const Position &rhs) const {
        return Position(coord_x + rhs.coord_x, coord_y + rhs.coord_y);
    }
    /// Overload to go in certain direction
    Position operator+(const Direction &dir) const {
        const std::array<Position, 4> directions = {
            Position(0, 1), Position(0, -1), Position(-1, 0),
            Position(1, 0)}; //!< Up, Down, Left, Right
        return *this + directions[(size_t)dir];
    }
    /// 'less than' overload
    bool operator<(const Position &other) const {
        return coord_x < other.coord_x or
               (coord_x == other.coord_x and coord_y < other.coord_y);
    }
    /// Hasher overload
    struct Hash {
        size_t operator()(const Position &pos) const {
            return std::hash<size_t>()(pos.coord_x) ^
                   std::hash<size_t>()(pos.coord_y);
        }
    };
};

/// The class that represents a maze as an array, and offers a interface to work
/// like a puzzle manager.
class Maze {
  public:
    /// Struct that represents what a element of maze array represents
    enum class Cell {
        Free = 0,
        Wall,
        Start,
        Finish,
        Path,
    };
    /// Constructor
    Maze(const std::string &filename);
    /// Copy Constructor
    Maze(const Maze &rhs) = default;
    /// Return the height of the Maze
    size_t height() const { return m_height; }
    /// Return the width of the Maze
    size_t width() const { return m_width; }
    /// Given a Position `pos` tells if `pos` is in bounds
    [[nodiscard]] bool in_bound(const Position &pos) const {
        return (pos.coord_y < m_height and pos.coord_x < m_width);
    }
    [[nodiscard]] Position start() const { return m_start; }
    /// Given a Position `pos` tells if `pos` is the finish or not
    [[nodiscard]] bool found_finish(const Position &pos) const {
        return pos == m_finish;
    }
    /// Given a Position `pos` and a direction `dir` see tells if the subsequent
    /// position is acessible or not.
    bool blocked(const Position &pos, const Direction &dir) const {
        auto move = pos + dir;
        return in_bound(move) and
               m_maze[move.coord_y][move.coord_x] == Cell::Wall;
    }
    /// Method to print the Maze, without highlighting his position.
    [[nodiscard]] std::string str() const;
    /// Method to print the Maze, with his solution highlighted.
    [[nodiscard]] std::string str(std::list<Direction> &solution) const;

  private:
    std::vector<std::vector<Cell>> m_maze; //!< The actual Maze
    size_t m_height; //!< The height of the maze array, i.e. his number of rows.
    size_t m_width;  //!< The width of the maze array, i.e. his number of lines.
    Position m_start;  //!< Where is the start position of the maze puzzle.
    Position m_finish; //!< Where is the end to be found of the maze puzzle.

    /// Resizes every row of the maze array, it's used as an auxiliary for
    /// Constructing a object of this class.
    void resize_maze() {
        m_maze.resize(m_height);
        for (auto &row : m_maze) {
            row.resize(m_width);
        }
    }
};
} // namespace snaze
#endif // !MAZE_HPP
