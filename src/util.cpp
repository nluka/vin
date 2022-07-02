#include "util.hpp"

// https://www.asciitable.com/

bool util::is_whitespace(char const c) {
  return c == ' ' || c == '\t';
}
bool util::is_letter(char const c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool util::is_number(char const c) {
  return c >= '0' && c <= '9';
}
bool util::is_alphanumeric(char const c) {
  return util::is_letter(c) || util::is_number(c);
}
bool util::is_symbol(char const c) {
  return
    (c >= '!' && c <= '/') ||
    (c >= ':' && c <= '@') ||
    (c >= '[' && c <= '`') ||
    (c >= '{' && c <= '~');
}

int util::ascii_digit_to_int(char const c) {
  return static_cast<int>(c) - 48;
}
