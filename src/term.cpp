#include <stdio.h>
#include <sstream>
#if defined(_WIN32)
#include <windows.h>
#endif
#include "term.hpp"

// super useful reference:
// https://www2.math.upenn.edu/~kazdan/210/computer/ansi.html

using term::ColorText;

static ColorText g_defaultColorText = ColorText::DEFAULT;

void term::set_color_text(ColorText const color) {
  printf("\033[%dm", static_cast<int>(color));
}

void term::cursor_hide() {
  printf("\33[?25l");
}
void term::cursor_show() {
  printf("\33[?25h");
}
void term::cursor_disable_blinking() {
  printf("\33[?12l");
}
void term::cursor_set_height(size_t percent) {
#ifdef _WIN32
  if (percent < 1) {
    percent = 1;
  } else if (percent > 100) {
    percent = 100;
  }
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO cursorInfo{};
  cursorInfo.dwSize = static_cast<DWORD>(percent);
  if (!SetConsoleCursorInfo(hOut, &cursorInfo)) {
    std::exit(1);
  }
#else
  throw "term::cursor_set_height is only supported on Windows";
#endif
}
void term::cursor_goto(size_t x, size_t y) {
  // row;col
  // ^^^ starts from 1 rather than 0 so we must add 1 to `y`
  printf("\33[%zu;%zuH", y + 1, x);
}
void term::cursor_move_up(size_t const n) {
  printf("\33[%zuA", n);
}
void term::cursor_move_down(size_t const n) {
  printf("\33[%zuB", n);
}
void term::cursor_move_right(size_t const n) {
  printf("\33[%zuC", n);
}
void term::cursor_move_left(size_t const n) {
  printf("\33[%zuD", n);
}
void term::cursor_move_to_top_left() {
  printf("\33[;H");
}
void term::cursor_save_pos() {
  printf("\33[s");
}
void term::cursor_restore_last_saved_pos() {
  printf("\33[u");
}
// Top-leftmost position is (0, 0).
term::CursorPos term::cursor_get_pos() {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO consoleInfo{};
  if (GetConsoleScreenBufferInfo(hOut, &consoleInfo)) {
    return {
      static_cast<size_t>(consoleInfo.dwCursorPosition.X),
      static_cast<size_t>(consoleInfo.dwCursorPosition.Y)
    };
  } else {
    std::exit(1);
  }
}

void term::clear_screen() {
  printf("\033[2J");
}
void term::clear_curr_line() {
  printf("\33[2K\r");
}
void term::clear_to_end_of_line() {
  printf("\033[K");
}

void term::printf_colored(
  ColorText const color,
  char const *const format,
  ...
) {
  va_list args;
  va_start(args, format);
  set_color_text(color);
  vprintf(format, args);
  set_color_text(g_defaultColorText);
  va_end(args);
}

void term::set_color_text_default(ColorText const color) {
  g_defaultColorText = color;
}

void term::remove_scrollbar() {
#if defined(_WIN32)
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo{};
  GetConsoleScreenBufferInfo(hOut, &screenBufferInfo);

  short const windowWidth =
    screenBufferInfo.srWindow.Right - screenBufferInfo.srWindow.Left + 1;
  short const windowHeight =
    screenBufferInfo.srWindow.Bottom - screenBufferInfo.srWindow.Top + 1;

  short const screenBufferWidth = screenBufferInfo.dwSize.X;
  short const screenBufferHeight = screenBufferInfo.dwSize.Y;

  COORD newSize{};
  newSize.X = screenBufferWidth;
  newSize.Y = windowHeight;

  if (SetConsoleScreenBufferSize(hOut, newSize) == 0) {
    std::stringstream ss{};
    ss << "term::remove_scrollbar failed - error code " << GetLastError();
    throw ss.str();
  }
#else
  throw "term::remove_scrollbar is only supported on Windows";
#endif
}
