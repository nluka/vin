#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "term.hpp"
#include "logger.hpp"
#include "user-config.hpp"

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

int main(int const argc, char const *const *const argv) {
  if (argc != 2) {
    std::cout << "usage: <file_pathname>\n";
    std::exit(0);
  }

  // configure logger
  logger::set_out_pathname("./vin.log");
  logger::set_max_msg_len(500);
  logger::set_autoflush(true);

  usrconf::load();

  std::vector<std::string> lines{};
  // read lines from specified file
  {
    char const *const fPathname = argv[1];
    std::ifstream file(fPathname);
    if (!file.is_open()) {
      logger::write(
        logger::EventType::FTL,
        "failed to open file `%s`",
        fPathname
      );
      std::exit(1);
    }
    if (!file.good()) {
      logger::write(
        logger::EventType::FTL,
        "bad file `%s`",
        fPathname
      );
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
    // so this flag defaults to true.
    // there are however a few edge cases where we don't want to re-render,
    // in those cases we set this flag false.
    bool shouldRender = true;

    if (c == usrconf::nav::key_move_left())
    {
      term::cursor_move_left(1);
    }
    else if (c == usrconf::nav::key_move_right())
    {
      term::CursorPos const cursorPos = term::cursor_get_pos();
      std::string const &currLine = lines[cursorPos.m_y];
      if (cursorPos.m_x < currLine.length() - 1) {
        term::cursor_move_right(1);
      } else {
        // we are at end of line, can't go any further right
        shouldRender = false;
      }
    }
    else if (c == usrconf::nav::key_move_down())
    {
      term::CursorPos const cursorPos = term::cursor_get_pos();
      size_t const lastLineIdx = lines.empty() ? 0 : lines.size() - 1;
      if (cursorPos.m_y == lastLineIdx) {
        // we are at the bottom, can't go any lower
        shouldRender = false;
      } else {
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
      }
    }
    else if (c == usrconf::nav::key_move_up())
    {
      term::CursorPos const cursorPos = term::cursor_get_pos();
      if (cursorPos.m_y == 0) {
        // we are at the top, can't go any higher
        shouldRender = false;
      } else {
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
      }
    }
    else
    {
      #ifdef _WIN32
      // make beep sound
      printf("%c", static_cast<char>(7));
      #endif
    }

    if (shouldRender) {
      render(lines);
    }
  }
}
