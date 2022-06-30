#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <stdio.h>
#include <conio.h>
#include "term.hpp"
#include "logger.hpp"
#include "user-config.hpp"

void render(
  std::vector<std::string> const &lines,
  size_t const firstLineToRenderIdx,
  bool const unhideCursorAfterRender = true
) {
  term::cursor::save_pos();
  term::cursor::hide();
  term::cursor::move_to_top_left();

  // how many lines of text are we expecting to render?
  size_t const numLinesToRender = term::height_in_lines() - 1;

  // index of the last line we want to render
  size_t const lastLineToRenderIdx =
    firstLineToRenderIdx + numLinesToRender - 1;

  size_t numLinesRendered = 0;

  for (
    size_t i = firstLineToRenderIdx;
    (i <= lastLineToRenderIdx) && (i < lines.size());
    ++i, ++numLinesRendered
  ) {
    std::string const &line = lines[i];
    printf("%s", line.c_str());
    term::clear_to_end_of_line();
    printf("\n");
  }

  term::cursor::restore_last_saved_pos();
  if (unhideCursorAfterRender) {
    term::cursor::show();
  }
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

  logger::write(logger::EventType::INF, "program started");

  usrconf::load();

  size_t firstVisibleLineIdx = 0;
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
      lines.emplace_back(line.c_str());
    }
  }

  term::remove_scrollbar();
  term::cursor::disable_blinking();
  term::cursor::set_size(100);

  render(lines, firstVisibleLineIdx);

  while (true) {
    int c = _getch();

    if (c == 'q') {
      std::exit(0);
    }

    if (c == usrconf::nav::key_move_left())
    {
      // the terminal will prevent us from moving beyond the left boundary,
      // so we don't need to do any checks for that - we can always move
      term::cursor::move_left(1);
    }
    else if (c == usrconf::nav::key_move_right())
    {
      term::cursor::Position const cursorPos = term::cursor::get_pos();

      // y-index of the cursor
      size_t const cursorYPosInFile =
        term::cursor::get_pos().m_y + firstVisibleLineIdx;

      std::string const &currLine = lines[cursorYPosInFile];

      bool const areWeAtEndOfCurrLine = cursorPos.m_x == currLine.length();

      if (!areWeAtEndOfCurrLine) {
        term::cursor::move_right(1);
      }
    }
    else if (c == usrconf::nav::key_move_down())
    {
      term::cursor::Position const cursorPos = term::cursor::get_pos();

      // y-index of the very last line in the file
      size_t const lastLineIdx = lines.empty() ? 0 : lines.size() - 1;

      // y-index of the cursor
      size_t const cursorYPosInFile =
        term::cursor::get_pos().m_y + firstVisibleLineIdx;

      bool const areWeOnLastLine = cursorYPosInFile == lastLineIdx;

      if (!areWeOnLastLine) {
        size_t const firstLineBelowViewportIdx =
          firstVisibleLineIdx + term::height_in_lines();

        bool const areWeAtBottomOfViewport =
          cursorYPosInFile == (firstLineBelowViewportIdx - 1);

        std::string const &lineBelow = lines[cursorYPosInFile + 1];

        bool const isLineBelowShorterThanCurrLine =
          cursorPos.m_x > lineBelow.length();

        if (!areWeAtBottomOfViewport) {
          // move cursor down, and if the line below is shorter than current
          // line then move cursor leftwards to the end of the line below
          term::cursor::move(
            isLineBelowShorterThanCurrLine
              ? lineBelow.length()
              : cursorPos.m_x,
            static_cast<size_t>(cursorPos.m_y) + static_cast<size_t>(1)
          );
        } else {
          // we are at the bottom of the viewport,
          // shift viewport down one line and re-render
          ++firstVisibleLineIdx;
          render(lines, firstVisibleLineIdx, false);

          if (isLineBelowShorterThanCurrLine) {
            term::cursor::move(lineBelow.length(), cursorPos.m_y);
          }
          term::cursor::show();
        }
      }
    }
    else if (c == usrconf::nav::key_move_up())
    {
      term::cursor::Position const cursorPos = term::cursor::get_pos();

      // y-index of the cursor
      size_t const cursorYPosInFile =
        term::cursor::get_pos().m_y + firstVisibleLineIdx;

      bool const areWeOnFirstLine = cursorYPosInFile == 0;

      if (!areWeOnFirstLine) {
        size_t const firstLineAboveViewportIdx = firstVisibleLineIdx - 1;

        bool const areWeAtTopOfViewport =
          cursorYPosInFile == (firstLineAboveViewportIdx + 1);

        std::string const &lineAbove = lines[cursorYPosInFile - 1];

        bool const isLineAboveShorterThanCurrLine =
          cursorPos.m_x > lineAbove.length();

        if (!areWeAtTopOfViewport) {
          // move cursor up, and if the line above is shorter than the current
          // line then move cursor leftwards to the end of the line above
          term::cursor::move(
            isLineAboveShorterThanCurrLine
              ? lineAbove.length()
              : cursorPos.m_x,
            static_cast<size_t>(cursorPos.m_y) - static_cast<size_t>(1)
          );
        } else {
          // we are at the top of viewport,
          // shift viewport up one line and re-render
          --firstVisibleLineIdx;
          render(lines, firstVisibleLineIdx, false);

          if (isLineAboveShorterThanCurrLine) {
            term::cursor::move(lineAbove.length(), cursorPos.m_y);
          }
          term::cursor::show();
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
  }
}
