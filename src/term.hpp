#ifndef CPPLIB_TERM_HPP
#define CPPLIB_TERM_HPP

// Module for doing fancy things in the terminal via ANSI escape sequences.
// Make sure your terminal supports ANSI escape sequences when using this module!
namespace term {

enum class ColorText {
  DEFAULT = 0,
  RED = 31,
  GREEN = 32,
  YELLOW = 33,
  BLUE = 34,
  MAGENTA = 35,
  CYAN = 36,
  GRAY = 90,
  GREY = 90,
};

// Sets stdout text color.
void set_color_text(ColorText);

// Wrapper for `printf` allowing for colored printing.
// After being called, this function sets the stdout text color to whatever
// `term::set_color_text_default` set it to, which is `ColorText::DEFAULT` by default.
void printf_colored(ColorText, char const *fmt, ...);

void clear_screen();
void clear_curr_line();
void clear_to_end_of_line();
void remove_scrollbar();
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
