#include "snake.hpp"
#include "maze.hpp"

#include <experimental/random>
namespace snaze {
SnakeBot::MaybeDirectionList SnakeBot::solve(const Maze &maze, const Snake &snake) {
    // TODO: Account for snake body
    std::queue<Position> to_visit;
    std::unordered_set<Position, Position::Hash> meta_visited;
    std::map<Position, Direction> came_from;
    constexpr std::array directions{Direction::Up, Direction::Down, Direction::Left,
                                    Direction::Right};
    auto start_pos = maze.start();
    to_visit_position_append(start_pos, to_visit, meta_visited);
    while (not to_visit.empty()) {
        auto current_pos = to_visit.front();
        to_visit.pop();
        if (maze.found_food(current_pos)) {
            return reconstruct_path(came_from, start_pos, current_pos);
        }
        for (const auto &dir : directions) {
            auto next_pos = current_pos + dir;
            Snake snake_copy(snake);
            snake_copy.move_snake(dir);
            if (snake_copy.is_snake_body(next_pos)) {
                continue;
            }
            if (maze.in_bound(next_pos) and not(maze.blocked(current_pos, dir)) and
                not already_visited(next_pos, meta_visited)) {
                came_from[next_pos] = dir;
                to_visit_position_append(next_pos, to_visit, meta_visited);
            }
        }
    }
    return std::nullopt;
}

std::vector<Direction> SnakeBot::positions_available(Position current, const Maze &maze) {
    std::vector<Direction> moves;
    for (const auto &dir : {Direction::Up, Direction::Down, Direction::Right, Direction::Left}) {
        if (!maze.blocked(current, dir)) {
            moves.push_back(dir);
        }
    }
    return moves;
}

SnakeBot::MaybeDirectionList SnakeBot::play_random(const Maze &maze) {
    MaybeDirectionList moves;
    auto current_pos = maze.start();
    auto first_move = positions_available(current_pos, maze);
    auto head_dir = first_move[std::experimental::randint(0, (int)first_move.size() - 1)];

    while (!maze.found_food(current_pos) or maze.is_wall(current_pos)) {
        std::vector<Direction> possible_moves = positions_available(current_pos, maze);

        if (possible_moves.size() == 0) { // The snake entered a dead end
            current_pos += Direction::Right;
            moves->push_back(Direction::Right);
            head_dir = Direction::Right;
        } else {
            for (const auto &move : possible_moves) {
                if (maze.found_food(current_pos + move) and
                    opposite(head_dir) != move) { // Snake is next to the food
                    moves->push_back(move);
                    current_pos += move;
                    head_dir = move;

                    break;
                }
            }

            int pick_index;
            Direction pick;

            do {
                pick_index = std::experimental::randint(0, (int)possible_moves.size() - 1);
                pick = possible_moves[pick_index];
            } while (opposite(pick) == head_dir); // To make sure to dont go backwards

            moves->push_back(pick);
            current_pos += pick;
            head_dir = pick;
        }
    }

    return moves;
}
} // namespace snaze
