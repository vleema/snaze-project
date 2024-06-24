#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "snake.hpp"
#include <string>
#include <vector>

/*
 * TODO: Represent the snake in some way in the maze
 * TODO: Impl kb_motions so that a player can play, there's a test file for that
 * `tests/kb_motions.cpp`
 */

namespace snaze {
/// Class keeps track of the Snaze as whole, and follows GameLoop design
/// pattern: https://gameprogrammingpatterns.com/game-loop.html
class SnazeManager {
    /// Snaze runnings opts that are cone be read from a ini file
    struct Settings {
        size_t fps;
        size_t lives;
        size_t n_foods;
    };
    /// Enum that represents the possibles game states
    enum class SnazeState {
        MainMenu,
        SnazeMode,
        GameStart,
        On,
        Damage,
        Won,
        Lost,
        Quit,
    };
    /// Enum that represents the possible options that can be selected in Main
    /// Menu
    enum class MainMenuOption {
        Play,
        Quit,
    };
    /// Enum that represents the mode that snaze will be played
    enum class SnazeMode {
        Player,
        Bot,
    };

    SnazeState m_snaze_state; //!< The current state of the SnazeManager
    SnazeMode m_snaze_mode;   //!< How the snaze will be played
    Settings m_settings;      //!< Snaze running opts
    bool m_game_over;         //!< Boolean to tell if the game as ended or not
    SnakeBot m_snake_bot;     //!< A bot that autoplays the game
    Maze m_maze;              //!< Representation of the maze
    std::vector<std::string>
        m_game_levels_files; //!<- A list containing all the game levels

  public:
    /// Constructor
    SnazeManager(const std::string &game_levels_directory,
                 const std::string &ini_config_path);
    /// Function to process user o bot inputs
    void process();
    /// Function to update the snaze state
    void update();
    /// Function to display graphical things in the screen
    void render() const;
};
} // namespace snaze

#endif // !GAME_MANAGER_HPP
