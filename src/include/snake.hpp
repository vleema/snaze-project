#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <unordered_set>

#include "maze.hpp"

namespace snaze {
struct Snake {
    std::deque<Position> body;
    Direction head_direction{Direction::None};
    /// Copy constructor
    Snake(const Snake &rhs) = default;
    /// Method to verify if a position corresponds to the snake body
    [[nodiscard]] bool is_snake_body(const Position &position) const {
        return std::find(body.cbegin() + 1, body.cend(), position) != body.cend();
    }
    /// Resets the snake to it's defaults
    void reset() {
        body.clear();
        head_direction = Direction::None;
    }
    /// Moves the snake in some direction, and returns the head position
    Position move_snake(const Direction &direction) {
        body.push_front(body.front() + direction);
        body.pop_back();
        return body.front();
    }
};
/// Class responsible for finding the shortest path to the finish
class SnakeBot {
  public:
    using MaybeDirectionList = std::optional<std::list<Direction>>;
    using PositionUSet = std::unordered_set<Position, Position::Hash>;

    /// Method to solve the maze
    static MaybeDirectionList solve(const Maze &maze, const Snake &snake);

    /// Method to print the solution from start to finish
    static std::string str(const Maze &maze, std::list<Direction> &solution);

  private:
    /// Method to get the opposite direction
    static Direction opposite(const Direction &dir) {
        switch (dir) {
        case Direction::Up:
            return Direction::Down;
        case Direction::Down:
            return Direction::Up;
        case Direction::Left:
            return Direction::Right;
        case Direction::Right:
            return Direction::Left;
        default:
            return Direction::None;
        }
    }
    /// Method to reconstruct the shorter path from start to the food position
    static std::list<Direction> reconstruct_path(std::map<Position, Direction> &came_from,
                                                 Position start, Position end) {
        std::list<Direction> path;
        Position current = end;
        while (current != start) {
            path.push_front(came_from[current]);
            current = current + opposite(came_from[current]);
        }
        return path;
    }
    /// Method to verify that a Position was already visited
    static bool already_visited(const Position &pos, const PositionUSet &visited) {
        return visited.find(pos) != visited.cend();
    }
    /// Method to visit a position and save that o Solver variables
    static void to_visit_position_append(const Position &pos, std::queue<Position> &to_visit,
                                         PositionUSet &visited) {
        to_visit.push(pos);
        visited.insert(pos);
    }

    /// Method that given a position on a maze returns the available moves
    static std::vector<Direction> positions_available(Position current, const Maze &maze);
    /// Method to play the snake randomly, when there's no solution
    static MaybeDirectionList play_random(const Maze &maze);
};
} // namespace snaze
#endif // !SNAKE_HPP
