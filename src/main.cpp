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
#include "movement.hpp"
#include "util.hpp"

#pragma region state
std::vector<std::string> s_lines{};
size_t s_idxFirstVisibleLine = 0;
char const *s_filePathname = nullptr;
#pragma endregion state

static char const *const s_lineNumFmt = " %4zu ";
static size_t const s_lineNumTextWidth = 6;

enum class RenderMode {
  INFO_BAR_ONLY,
  TEXT_AND_INFO_BAR,
};
void render(RenderMode);

#define REPEAT(n) for (size_t i = 1; i <= n; ++i)

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

  // read lines from specified file
  {
    char const *const fPathname = argv[1];
    std::ifstream file(fPathname);
    if (!file.is_open()) {
      logger::write(
        logger::EventType::FTL,
        "failed to open file `%s`", fPathname
      );
      std::exit(1);
    }
    if (!file.good()) {
      logger::write(
        logger::EventType::FTL,
        "bad file `%s`", fPathname
      );
      std::exit(1);
    }
    std::string line{};
    while (std::getline(file, line)) {
      s_lines.emplace_back(line.c_str());
    }
  }

  s_filePathname = argv[1];

  term::remove_scrollbar();
  term::cursor::disable_blinking();
  term::cursor::set_size(100);
  term::cursor::move_right(s_lineNumTextWidth);

  static term::Dimensions s_prevTermDimensions = term::dimensions();

  render(RenderMode::TEXT_AND_INFO_BAR);

  while (true) {
    int input = _getch();

    if (static_cast<int>(input) == usrconf::key_exit()) {
      std::exit(0);
    } else if (!usrconf::is_keybind(static_cast<char>(input))) {
      #ifdef _WIN32
      // make beep sound
      printf("%c", static_cast<char>(7));
      #endif

      continue;
    }

    // check if terminal has changed size since last time,
    // and reset the cursor if it has - we do this because Windows
    // sometimes messes with the cursor when resizing the window.
    {
      term::Dimensions const termDimensions = term::dimensions();
      if (
        termDimensions.m_width != s_prevTermDimensions.m_width ||
        termDimensions.m_height != s_prevTermDimensions.m_height
      ) {
        term::cursor::move(s_lineNumTextWidth, 0);
      }
      s_prevTermDimensions = termDimensions;
    }

    // relative to the terminal window, not the file
    auto const cursorPos = term::cursor::get_pos();

    move::StartLoc const startLoc{
      s_lines,
      // convert relative-to-terminal-window positions into relative-to-file
      cursorPos.m_y + s_idxFirstVisibleLine,
      cursorPos.m_x - s_lineNumTextWidth
    };

    // relative-to-file
    move::Destination const dest = ([input, &startLoc]() -> move::Destination {
      if (input == usrconf::nav::key_move_left()) {
        return move::left(startLoc);
      } if (input == usrconf::nav::key_move_right()) {
        return move::right(startLoc);
      } else if (input == usrconf::nav::key_move_up()) {
        return move::up(startLoc);
      } else if (input == usrconf::nav::key_move_down()) {
        return move::down(startLoc);
      } else if (input == usrconf::nav::key_move_line_start()) {
        return move::line_start(startLoc);
      } else if (input == usrconf::nav::key_move_line_end()) {
        return move::line_end(startLoc);
      } else {
        throw "movement failed - bad destination";
      }
    })();

    int verticalPosDiff =
      static_cast<int>(dest.m_lineIdx) - static_cast<int>(startLoc.m_lineIdx);

    if (
      // should scroll viewport up?
      (verticalPosDiff < 0) &&
      (cursorPos.m_y == usrconf::scroll_offset()) &&
      (startLoc.m_lineIdx > usrconf::scroll_offset())
    ) {
      --s_idxFirstVisibleLine;
      render(RenderMode::TEXT_AND_INFO_BAR);
    } else if (
      // should scroll viewport down?
      (verticalPosDiff > 0) &&
      ((term::height_in_lines() - cursorPos.m_y) == usrconf::scroll_offset() + 2) &&
      !(s_idxFirstVisibleLine + term::height_in_lines() - 1 ==
        s_lines.size()) // is last line visible?
    ) {
      ++s_idxFirstVisibleLine;
      render(RenderMode::TEXT_AND_INFO_BAR);
    } else {
      // no need to move the viewport, just move the cursor
      term::cursor::move(
        // convert relative-to-file positions into relative-to-terminal-window
        dest.m_pos + s_lineNumTextWidth,
        dest.m_lineIdx - s_idxFirstVisibleLine
      );
      render(RenderMode::INFO_BAR_ONLY);
    }
  }
}

void render(RenderMode renderMode) {
  struct Location {
    size_t m_lineNum;
    size_t m_colNum;
  };

  // relative-to-file
  Location const currLoc = ([]() -> Location {
    auto const cursorPos = term::cursor::get_pos();
    return {
      cursorPos.m_y + s_idxFirstVisibleLine + 1,
      (cursorPos.m_x + 1) - s_lineNumTextWidth
    };
  })();

  size_t const termHeight = term::height_in_lines();
  static size_t s_prevTermHeight = termHeight;
  if (s_prevTermHeight != termHeight) {
    // make sure to render everything if the terminal height has changed,
    // if we only render the info bar then visual bugs happen
    renderMode = RenderMode::TEXT_AND_INFO_BAR;
  }

  term::cursor::save_pos();
  term::cursor::hide();
  term::cursor::move(0, termHeight - 1); // bottom left

  // no matter what, we always render the info bar
  printf(
    " \"%s\" Ln %zu Col %zu ",
    s_filePathname,
    currLoc.m_lineNum,
    currLoc.m_colNum
  );

  if (renderMode == RenderMode::INFO_BAR_ONLY) {
    term::cursor::restore_last_saved_pos();
    term::cursor::show();
    return;
  }

  size_t const numLinesToRender = termHeight - 1;
  size_t const numLinesWithinViewport =
    std::min(s_lines.size() - s_idxFirstVisibleLine, numLinesToRender);

  if (numLinesWithinViewport < numLinesToRender) {
    size_t const lineDeficiency = numLinesToRender - numLinesWithinViewport;
    REPEAT(lineDeficiency) {
      term::cursor::move_up(1);
      printf("\r");
      term::printf_colored(
        term::ColorFG::BLUE,
        term::ColorBG::BLACK,
        "~"
      );
      term::clear_to_end_of_line();
    }
  }

  size_t const lastLineNum =
    s_idxFirstVisibleLine +
    std::min(numLinesWithinViewport, numLinesToRender);

  size_t currLineNum = lastLineNum;
  REPEAT(numLinesWithinViewport) {
    term::cursor::move_up(1);
    printf("\r");
    term::printf_colored(
      term::ColorFG::LIGHT_YELLOW,
      term::ColorBG::BLACK,
      " %4zu ", currLineNum
    );
    printf(s_lines[currLineNum - 1].c_str());
    term::clear_to_end_of_line();
    --currLineNum;
  }

  term::cursor::restore_last_saved_pos();
  term::cursor::show();
}
