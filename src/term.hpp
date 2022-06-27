#ifndef CPPLIB_TERM_HPP
#define CPPLIB_TERM_HPP

// Module for doing fancy things in the terminal via ANSI escape sequences.
// Make sure your terminal supports ANSI escape sequences when using this module!
namespace term {

struct CursorPos {
  // starts from 0
  size_t m_x, m_y;
};

void cursor_hide();
void cursor_show();
void cursor_disable_blinking();
void cursor_set_height(size_t percent);
void cursor_goto(size_t col, size_t row);
void cursor_move_up(size_t n);
void cursor_move_down(size_t n);
void cursor_move_right(size_t n);
void cursor_move_left(size_t n);
void cursor_move_to_top_left();
void cursor_save_pos();
void cursor_restore_last_saved_pos();
CursorPos cursor_get_pos();

void clear_screen();
void clear_curr_line();
void clear_to_end_of_line();

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

// Changes the default text color applied after calling `term::printf_colored`.
void set_color_text_default(ColorText color);

void remove_scrollbar();

} // namespace term

#endif // CPPLIB_TERM_HPP
