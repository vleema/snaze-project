#ifndef INI_FILE_PARSER_H
#define INI_FILE_PARSER_H

#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include "../../src/include/game_manager.hpp"

namespace ini {

// === Sugar Aliases

class Parser {
  public:
    using Section = std::string;
    using Var = std::string;
    using Value = std::string;
    using IniUMap = std::unordered_map<Section, std::unordered_map<Var, Value>>;

    /// Search for a file a maybe return the address a maybe return the address
    [[nodiscard]] static std::optional<std::string> find_file(const std::string &file_name);
    /**
     * @brief Parses an INI file and returns a map of its sections and key-value
     * pairs.
     *
     * This function opens an INI file specified by the `settings_path`
     * parameter, reads it line by line, and parses its content into a map of
     * sections and key-value pairs. Each section is represented by a key in the
     * map, and its value is another map representing the key-value pairs within
     * that section. Lines starting with ';' or '#' are treated as comments and
     * ignored. Lines enclosed in square brackets are treated as section
     * headers. Lines containing an equals sign are treated as key-value pairs.
     *
     * @param settings_path A string representing the path to the INI file to be
     * parsed.
     * @return A map of string to map of string to string, representing the
     * sections and key-value pairs in the INI file.
     */
    [[nodiscard]] static snaze::Settings file(const std::string &settings_path);

  private:
    /// Function to trim leading and trailing whitespaces from a string.
    static std::string trim(std::string line);
    /// Function to open a file
    [[nodiscard]] static std::ifstream open_file(const std::string &file_path);
};
} // namespace ini

#endif // !INI_FILE_PARSER_H
