#ifndef MAZE_HPP
#define MAZE_HPP

#include <array>
#include <cstdio>
#include <list>
#include <string>
#include <vector>

namespace maze {
enum class Direction { Up, Down, Left, Right, None };
struct Position {
    size_t coord_x;
    size_t coord_y;
    /// Constructor
    Position(size_t x, size_t y) : coord_x(x), coord_y(y) {}
    /// Assing overload
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
    /// Less than overload
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
class Maze {
  public:
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
    /// position is acessible or not
    bool blocked(const Position &pos, const Direction &dir) const {
        auto move = pos + dir;
        return in_bound(move) and
               m_maze[move.coord_y][move.coord_x] == Cell::Wall;
    }
    /// Converts the maze into a readable string
    [[nodiscard]] std::string str() const;
    /// Method to print the Solution of the Maze
    [[nodiscard]] std::string str(std::list<Direction> &solution) const;

  private:
    std::vector<std::vector<Cell>> m_maze;
    size_t m_height;
    size_t m_width;
    Position m_start;
    Position m_finish;

    /// For initializing the maze
    void resize_maze() {
        m_maze.resize(m_height);
        for (auto &row : m_maze) {
            row.resize(m_width);
        }
    }
};
} // namespace maze
#endif // !MAZE_HPP
