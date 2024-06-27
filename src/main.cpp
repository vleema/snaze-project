#include "game_manager.hpp"
#include "maze.hpp"

int main(int argc, char *argv[]) {
    snaze::SnazeManager snaze("assets/", "conf/snaze_config.ini");
    snaze::Maze debug_maze("assets/levels_problem.dat");
    // while (not snaze.quit()) {
    //     snaze.process();
    //     snaze.update();
    //     snaze.render();
    // }
    return 0;
}
