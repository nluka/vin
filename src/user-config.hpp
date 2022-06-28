#ifndef VIN_USER_CONFIG_HPP
#define VIN_USER_CONFIG_HPP

// User configuation.
namespace usrconf {

void load();

// Navigation.
namespace nav {
  char key_move_up();
  char key_move_down();
  char key_move_left();
  char key_move_right();
  // char key_move_bol();
  // char key_move_eol();
  // char key_move_word_forward();
  // char key_move_word_backward();
  // char key_page_forward();
  // char key_page_backward();
} // namespace nav

// Manipulation.
// namespace manip {
//   char key_duplicate_line();
//   char key_delete_line();
// } // namespace manip

} // namespace usrconf

#endif // VIN_USER_CONFIG_HPP
