#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "maze.hpp"
#include "snake.hpp"
#include <string>
#include <vector>

namespace snaze {

/// Snaze runnings opts that are cone be read from a ini file
struct Settings {
        size_t fps;
        size_t lives;
        size_t food_amount;
        std::string player_type;
};

/// Class keeps track of the Snaze as whole, and follows GameLoop design
/// pattern: https://gameprogrammingpatterns.com/game-loop.html
class SnazeManager {
    /// Enum that represents the possibles game states
    enum class SnazeState {
        Init,
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
        Play = 1,
        Quit,
        Undefined,
    };
    /// Enum that represents the mode that snaze will be played
    enum class SnazeMode {
        Player = 1,
        Bot,
        Undefined,
    };

    // Back-end related
    SnazeState m_snaze_state{SnazeState::Init};   //!< The current state of the SnazeManager
    Settings m_settings{};                        //!< Snaze running opts
    bool m_game_over{false};                      //!< Boolean to tell if the game as ended or not
    SnakeBot m_snake_bot;                         //!< A bot that autoplays the game
    Snake m_snake;                                //!< The actual snake that are being moved
    Maze m_maze;                                  //!< Representation of the maze
    std::vector<std::string> m_game_levels_files; //!<- A list containing all the game levels

    // Render related variables and methods
    std::string m_screen_title;
    std::string m_main_content;
    std::string m_system_msg;
    std::string m_interaction_msg;

    /* All screens may have up to 4 components:
     *  (1) title (st),
     *  (2) main content (mc),
     *  (3) a system message (sm), used for errors in process
     *  (4) an interaction message (im).
     */
    /// Gets the m_screen_title variable value
    [[nodiscard]] std::string screen_title() const;
    /// Gets the m_main_content variable value
    [[nodiscard]] std::string main_content() const;
    /// Gets the m_system_msg variable value
    [[nodiscard]] std::string system_msg() const;
    /// Gets the m_interaction_msg variable value
    [[nodiscard]] std::string interaction_msg() const;
    /// Sets the m_screen_title variable value
    void screen_title(std::string new_screen_title);
    /// Sets the m_main_content variable value
    void main_content(std::string new_main_content);
    /// Sets the m_system_msg variable value
    void system_msg(std::string new_system_msg);
    /// Sets the m_interaction_msg variable value
    void interaction_msg(std::string new_interaction_msg);
    /// The main menu screen of the snaze
    [[nodiscard]] static std::string main_menu_mc();
    /// Quit screen
    [[nodiscard]] static std::string quit_mc();
    /// SnazeMode screen
    [[nodiscard]] static std::string snaze_mode_mc();

    // Process related variables and methods
    SnazeMode m_snaze_mode{SnazeMode::Undefined}; //!< How the snaze will be played
    MainMenuOption m_menu_option{
        MainMenuOption::Undefined}; //!< The selected menu option in Main menu state
    bool m_asked_to_quit{false};
    /// Reads the user main menu selected option
    MainMenuOption read_menu_option();
    /// Reads the user SnazeModeselected option
    SnazeMode read_snaze_option();
    /// Reads starting direction, when the game starts
    [[nodiscard]] static Direction read_starting_direction();

    // Update related variables and methods
    void change_state_by_selected_menu_option();

  public:
    /// Constructor
    SnazeManager(const std::string &game_levels_directory, const std::string &ini_config_path);
    /// Function to process user o bot inputs
    void process();
    /// Function to update the snaze state
    void update();
    /// Function to display graphical things in the screen
    void render();
    /// Function that tells if the game will quit or not
    [[nodiscard]] bool quit() const { return m_asked_to_quit; }
};
} // namespace snaze

#endif // !GAME_MANAGER_HPP
