#include "game_manager.hpp"
#include "maze.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    snaze::SnazeManager snaze("assets/", "conf/snaze_config.ini");
    snaze::Maze debug_maze("assets/level0.dat");
    std::cout << debug_maze.str();
    // while (not snaze.quit()) {
    //     snaze.process();
    //     snaze.update();
    //     snaze.render();
    // }
    return 0;
}
