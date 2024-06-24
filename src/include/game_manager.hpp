#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "snake.hpp"
#include <string>
#include <vector>

namespace snaze {
class SnazeManager {
    SnakeBot m_snake_bot;
    Maze m_maze;
    std::vector<std::string>
        m_game_levels_files; //!<- A list containing all the game levels

  public:
    /// Constructor
    SnazeManager(const std::string &game_levels_directory,
                 const std::string &ini_config_path);
    void process();
    void update();
    void render() const;
};
} // namespace snaze

#endif // !GAME_MANAGER_HPP
