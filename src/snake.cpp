#include "snake.hpp"
#include "maze.hpp"

namespace snaze {
SnakeBot::MaybeDirectionList SnakeBot::solve(const Maze &maze) {
    // TODO: Account for snake body
    std::queue<Position> to_visit;
    std::unordered_set<Position, Position::Hash> meta_visited;
    std::map<Position, Direction> came_from;
    constexpr std::array directions{Direction::Up, Direction::Down, Direction::Left,
                                    Direction::Right};
    auto start_pos = maze.start();
    visit_position(start_pos, to_visit, meta_visited);
    while (not to_visit.empty()) {
        auto current_pos = to_visit.front();
        to_visit.pop();
        if (maze.found_finish(current_pos))
            return reconstruct_path(came_from, start_pos, current_pos);
        for (const auto &dir : directions) {
            auto next_pos = current_pos + dir;
            if (maze.in_bound(next_pos) and not(maze.blocked(current_pos, dir)) and
                not already_visited(next_pos, meta_visited)) {
                came_from[next_pos] = dir;
                visit_position(next_pos, to_visit, meta_visited);
            }
        }
    }
    return std::nullopt;
}
} // namespace snaze
