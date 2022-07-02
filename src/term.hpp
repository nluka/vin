#ifndef CPPLIB_TERM_HPP
#define CPPLIB_TERM_HPP

// Module for doing fancy things in the terminal via ANSI escape sequences.
// Make sure your terminal supports ANSI escape sequences when using this module!
namespace term {

// Foreground.
enum class ColorFG {
  DEFAULT       = 0,
  RED           = 31,
  GREEN         = 32,
  YELLOW        = 33,
  BLUE          = 34,
  MAGENTA       = 35,
  CYAN          = 36,
  GRAY          = 90,
  GREY          = 90,
  LIGHT_RED     = 91,
  LIGHT_GREEN   = 92,
  LIGHT_YELLOW  = 93,
  LIGHT_BLUE    = 94,
  LIGHT_MAGENTA = 95,
  LIGHT_CYAN    = 96,
  WHITE         = 97,
};
// Sets stdout foreground (text) color.
void set_color_fg(ColorFG);

// Background
enum class ColorBG {
  BLACK     = 40,
  RED       = 41,
  GREEN     = 42,
  YELLOW    = 43,
  BLUE      = 44,
  MAGENTA   = 45,
  CYAN      = 46,
  WHITE     = 47,
};
// Sets stdout background color.
void set_color_bg(ColorBG);

// Wrapper for `printf` allowing for colored printing.
// After being called, this function sets the stdout foreground color to
// `term::ColorFG::DEFAULT` and background color to `term::ColorFG::BLACK`.
void printf_colored(ColorFG, ColorBG, char const *fmt, ...);

void clear_screen();
void clear_curr_line();
void clear_to_end_of_line();
void remove_scrollbar();

struct Dimensions {
  size_t m_width;
  size_t m_height;

  bool operator!=(Dimensions const &other) {
    return m_width == other.m_width && m_height == other.m_height;
  }
};

Dimensions dimensions();
size_t width_in_cols();
size_t height_in_lines();

namespace cursor {

  struct Position {
    // starts from 0
    size_t m_x, m_y;
  };

  void hide();
  void show();
  void disable_blinking();
  void set_size(size_t percent);
  void move(size_t col, size_t row);
  void move_up(size_t n);
  void move_down(size_t n);
  void move_right(size_t n);
  void move_left(size_t n);
  void move_to_top_left();
  void save_pos();
  void restore_last_saved_pos();
  Position get_pos();

} // namespace cursor

} // namespace term

#endif // CPPLIB_TERM_HPP
