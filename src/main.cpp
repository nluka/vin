#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "term.hpp"

void render(std::vector<std::string> const &lines) {
  term::cursor_save_pos();
  term::cursor_hide();
  term::cursor_move_to_top_left();
  for (std::string const &line : lines) {
    printf("%s", line.c_str());
    term::clear_to_end_of_line();
    printf("\r");
  }
  term::cursor_restore_last_saved_pos();
  term::cursor_show();
}

int main() {
  std::vector<std::string> lines{};

  // read sample file
  {
    std::ifstream file("test_files/simple.txt");
    if (!file.is_open() || !file.good()) {
      std::exit(1);
    }
    std::string line{};
    while (std::getline(file, line)) {
      lines.emplace_back(line + '\n');
    }
  }

  term::remove_scrollbar();
  term::cursor_disable_blinking();
  term::cursor_set_height(100);

  render(lines);

  while (true) {
    int c = _getch();

    if (c == 'q') {
      return 0; // exit
    }

    // in most cases we will we want to re-render,
    // so this flag is defaults to true.
    // there are however a few edge cases where we don't want to re-render,
    // in those cases we set this flag false.
    bool shouldRender = true;

    switch (c) {
      // left 1
      case 'd':
        term::cursor_move_left(1);
        break;
      // right 1
      case 'f': {
        term::CursorPos const cursorPos = term::cursor_get_pos();
        std::string const &currLine = lines[cursorPos.m_y];
        if (cursorPos.m_x < currLine.length() - 1) {
          term::cursor_move_right(1);
        } else {
          // we are at end of line, can't go any further right
          shouldRender = false;
        }
        break;
      }
      // down 1
      case 'j': {
        term::CursorPos const cursorPos = term::cursor_get_pos();
        size_t const lastLineIdx = lines.empty() ? 0 : lines.size() - 1;
        if (cursorPos.m_y == lastLineIdx) {
          // we are at the bottom, can't go any lower
          shouldRender = false;
          break;
        }
        std::string const &currLine = lines[cursorPos.m_y];
        std::string const &lineBelow = lines[cursorPos.m_y + 1];
        if (cursorPos.m_x <= lineBelow.length() - 1) {
          term::cursor_move_down(1);
        } else {
          // line below is shorter than current line,
          // we need to move cursor left also
          term::cursor_goto(
            lineBelow.length(),
            static_cast<size_t>(cursorPos.m_y) + static_cast<size_t>(1)
          );
        }
        break;
      }
      // up 1
      case 'k': {
        term::CursorPos const cursorPos = term::cursor_get_pos();
        if (cursorPos.m_y == 0) {
          // we are at the top, can't go any higher
          shouldRender = false;
          break;
        }
        std::string const &currLine = lines[cursorPos.m_y];
        std::string const &lineAbove = lines[cursorPos.m_y - 1];
        if (cursorPos.m_x <= lineAbove.length() - 1) {
          term::cursor_move_up(1);
        } else {
          // line above is shorter than current line,
          // we need to move cursor left also
          term::cursor_goto(
            lineAbove.length(),
            static_cast<size_t>(cursorPos.m_y) - static_cast<size_t>(1)
          );
        }
        break;
      }
      default:
        #ifdef _WIN32
        // make beep sound
        printf("%c", static_cast<char>(7));
        #endif
        break;
    }

    if (shouldRender) {
      render(lines);
    }
  }
}
