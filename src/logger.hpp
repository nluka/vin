#ifndef CPPLIB_LOGGER_HPP
#define CPPLIB_LOGGER_HPP

#include <string>

// Simple module for writing logs to a file.
namespace logger {

// Sets the pathname of the file for the `logger` module to write logs to.
void set_out_pathname(char const *);
// Sets the pathname of the file for the `logger` module to write logs to.
void set_out_pathname(std::string const &);

// Sets the maximum allowed message length.
// Messages longer than set limit will be cut short.
void set_max_msg_len(size_t);

// Sets the character sequence used to delimit individual logs.
// The default is "\n".
void set_delim(char const *);

// If set true, logs will be flushed as soon as they are written via
// `logger::write`.
void set_autoflush(bool);

enum class EventType {
  // Info
  INF = 0,
  // Warning
  WRN,
  // Error
  ERR,
  // Fatal
  FTL
};

// Writes an event (with formatted message) to the log in a threadsafe manner.
void write(EventType, char const *fmt, ...);

// Flushes the log buffer in a threadsafe manner.
void flush();

} // namespace log

#endif // CPPLIB_LOGGER_HPP
