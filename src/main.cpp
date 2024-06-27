#include "game_manager.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    snaze::SnazeManager snaze("assets/", "conf/snaze_config.ini");
    while (not snaze.quit()) {
        snaze.process();
        snaze.update();
        snaze.render();
    }
    return 0;
}
