#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <iostream>
#include "logger.hpp"
#include "user-config.hpp"

static char s_keyMoveUp = 'k';
char usrconf::nav::key_move_up() {
  return s_keyMoveUp;
}
static char s_keyMoveDown = 'j';
char usrconf::nav::key_move_down() {
  return s_keyMoveDown;
}
static char s_keyMoveLeft = 'd';
char usrconf::nav::key_move_left() {
  return s_keyMoveLeft;
}
static char s_keyMoveRight = 'f';
char usrconf::nav::key_move_right() {
  return s_keyMoveRight;
}

void usrconf::load() {
  std::vector<std::string> lines{};
  // extract lines
  {
    char const *const fPathname = "vin.cfg";
    std::ifstream file(fPathname);

    if (!file.is_open()) {
      logger::write(
        logger::EventType::WRN,
        "failed to open file `%s`, using default config",
        fPathname
      );
      return;
    } else if (!file.good()) {
      logger::write(
        logger::EventType::WRN,
        "failed to read file `%s`, using default config",
        fPathname
      );
      return;
    }

    lines.reserve(4);

    char temp[512];
    // extract lines
    while (file.getline(temp, sizeof(temp))) {
      lines.emplace_back(temp);
    }
  }

  std::regex const whitespace("[ \t]+");
  std::regex const adjoiningEqualSigns("={2,}");
  size_t lineNum = 0;

  for (std::string &line : lines) {
    ++lineNum;

    if (
      line.empty() ||
      // is comment
      line.front() == '#' ||
      line.front() == ';') {
      continue;
    }

    // remove any whitespace
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

    size_t const delimPos = line.find_first_of('=');
    if (delimPos == std::string::npos) {
      logger::write(
        logger::EventType::WRN,
        "missing '=' on line %zu",
        lineNum
      );
      continue;
    }

    std::string_view const setting(line.data(), delimPos);
    std::string_view const value(
      line.data() + delimPos + 1,
      line.length() - delimPos
    );

    if (setting == "moveUp") {
      s_keyMoveUp = value[0];
    } else if (setting == "moveDown") {
      s_keyMoveDown = value[0];
    } else if (setting == "moveLeft") {
      s_keyMoveLeft = value[0];
    } else if (setting == "moveRight") {
      s_keyMoveRight = value[0];
    } else {
      logger::write(
        logger::EventType::WRN,
        "unknown setting `%s`",
        setting
      );
    }
  }
}
