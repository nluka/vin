#ifndef VIN_UTIL_HPP
#define VIN_UTIL_HPP

namespace util {

bool is_whitespace(char c);
bool is_letter(char c);
bool is_number(char c);
bool is_alphanumeric(char c);
bool is_symbol(char c);

int ascii_digit_to_int(char c);

template<typename T>
size_t count_digits(T num) {
  size_t count = 0;
  for (; num > 0; ++count) {
    num /= 10;
  }
  return count;
}

} // namespace util

#endif // VIN_UTIL_HPP
