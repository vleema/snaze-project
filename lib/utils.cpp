#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

void cin_clear() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool read_yes_no_confirmation(/* bool yes_preferred = true */) {
    // TODO: Add default option functionality
    // FIX: Handle when user has inputed something other the "y/Y".
    // FIX: Make a default option, and handle cases where the user inputed anything but "yes" or
    // "no"
    std::string choice;
    std::cin >> choice;
    std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
    return choice == "y";
}

void read_enter_to_proceed() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}
