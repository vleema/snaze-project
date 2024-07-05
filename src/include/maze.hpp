#ifndef MAZE_HPP
#define MAZE_HPP

#include <array>
#include <cstdio>
#include <deque>
#include <list>
#include <string>
#include <vector>

namespace snaze {
/// A enum to represent directions in a cartesian style
enum class Direction { Up = 'w', Down = 's', Left = 'a', Right = 'd', None };
/// Data structure that represents a cartesian coordinate
struct Position {
    size_t coord_x;
    size_t coord_y;
    /// Default constructor
    Position() = default;
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
        return {coord_x + rhs.coord_x, coord_y + rhs.coord_y};
    }
    /// Overload to go in certain direction
    Position operator+(const Direction &dir) const {
        Position offset;
        switch (dir) {
        case Direction::Down:
            offset = Position(0, 1);
            break;
        case Direction::Up:
            offset = Position(0, -1);
            break;
        case Direction::Left:
            offset = Position(-1, 0);
            break;
        case Direction::Right:
            offset = Position(1, 0);
            break;
        case Direction::None:
        default:
            offset = Position(0, 0);
            break;
        }
        return *this + offset;
    }
    Position operator+=(const Direction &dir) const { return *this + dir; }
    /// 'less than' overload
    bool operator<(const Position &other) const {
        return coord_x < other.coord_x or (coord_x == other.coord_x and coord_y < other.coord_y);
    }
    /// Hasher overload
    struct Hash {
        size_t operator()(const Position &pos) const {
            return std::hash<size_t>()(pos.coord_x) ^ std::hash<size_t>()(pos.coord_y);
        }
    };
};

/// The class that represents a maze as an array, and offers a interface to work
/// like a puzzle manager.
class Maze {
  public:
    /// Struct that represents what a element of maze array represents
    enum class Cell {
        Free = ' ',
        Wall = '#',
        InvisibleWall = '.',
        Spawn = '&',
        Food,
        SnakeBody,
        SnakeHead,
    };
    /// Construct empty maze
    Maze() { resize_maze(); }
    /// Constructor with filename
    explicit Maze(const std::string &filename);
    /// Copy Constructor
    Maze(const Maze &rhs) = default;
    /// Assign operator
    Maze &operator=(const Maze &rhs) = default;
    /// Return the height of the Maze
    [[nodiscard]] size_t height() const { return m_height; }
    /// Return the width of the Maze
    [[nodiscard]] size_t width() const { return m_width; }
    /// Given a Position `pos` tells if `pos` is in bounds
    [[nodiscard]] bool in_bound(const Position &pos) const {
        return (pos.coord_y < m_height and pos.coord_x < m_width);
    }
    [[nodiscard]] bool is_wall(const Position &pos) const {
        return m_maze[pos.coord_y][pos.coord_x] == Cell::Wall;
    }
    [[nodiscard]] Position start() const { return m_spawn; }
    /// Given a Position `pos` tells if `pos` is the finish or not
    [[nodiscard]] bool found_food(const Position &pos) const { return pos == m_food; }
    /// Given a Position `pos` and a direction `dir` see tells if the subsequent
    /// position is acessible or not.
    [[nodiscard]] bool blocked(const Position &pos, const Direction &dir) const {
        auto move = pos + dir;
        return in_bound(move) and m_maze[move.coord_y][move.coord_x] == Cell::Wall;
    }
    /// Method to append the Maze to string, only showing the spawn position
    [[nodiscard]] std::string str_spawn() const;
    /// Method to append the Maze to string, showing the snake
    //  - snake head: ⸯ (vertical), ~ (horizontal)
    [[nodiscard]] std::string str_in_game(const std::deque<Position> &snake_body,
                                          const Direction &snake_head_direction) const;
    [[nodiscard]] std::string str_debug(const std::deque<Direction> &solution) const;
    /// Generates a random food position
    void random_food_position();

  private:
    std::vector<std::vector<Cell>> m_maze; //!< The actual Maze
    size_t m_height{10};                //!< The height of the maze array, i.e. his number of rows.
    size_t m_width{10};                 //!< The width of the maze array, i.e. his number of lines.
    Position m_spawn{};                 //!< Where is the start position of the maze puzzle.
    Position m_food{};                  //!< Where is the end to be found of the maze puzzle.
    std::vector<Position> m_free_cells; //!< Used for more efficiently generating a food position

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
