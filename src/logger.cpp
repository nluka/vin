#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdarg>
#include "logger.hpp"

static std::string s_outPathname{};
void logger::set_out_pathname(char const *const pathname) {
  s_outPathname = pathname;
}
void logger::set_out_pathname(std::string const &pathname) {
  s_outPathname = pathname;
}

static size_t s_maxMsgLen = 100;
void logger::set_max_msg_len(size_t const maxLen) {
  s_maxMsgLen = maxLen;
}

static char const *s_delim = "\n";
void logger::set_delim(char const *const delim) {
  s_delim = delim;
}

static bool s_autoFlush = false;
void logger::set_autoflush(bool const b) {
  s_autoFlush = b;
}

using logger::EventType;

static
char const *event_type_to_str(EventType const evType) {
  switch (evType) {
    case EventType::INF: return "INFO";
    case EventType::WRN: return "WARNING";
    case EventType::ERR: return "ERROR";
    case EventType::FTL: return "FATAL";
    default: throw "bad EventType";
  }
}

class Event {
private:
  EventType const m_type;
  std::string const m_msg;
  std::chrono::system_clock::time_point const m_timepoint =
    std::chrono::system_clock::now();

public:
  Event(EventType const type, char const *const msg)
    : m_type{type}, m_msg{msg} {}

  std::string stringify() const {
    std::stringstream ss{};

    ss << '[' << event_type_to_str(m_type) << "] ";

    { // timestamp
      std::time_t const time =
        std::chrono::system_clock::to_time_t(m_timepoint);
      struct tm const *local = localtime(&time);

      ss << '('
        << (local->tm_year + 1900) << '-'
        << local->tm_mon << '-'
        << local->tm_mday << ' '
        << std::setfill('0')
        << local->tm_hour << ':'
        << std::setw(2) << local->tm_min << ':'
        << std::setw(2) << local->tm_sec
      << ") ";
    }

    ss << m_msg;

    return ss.str();
  }
};

static std::vector<Event> s_events{};
static bool s_isFileReady = false;

static
void assert_file_opened(std::ofstream const &file) {
  if (!file.is_open()) {
    std::stringstream ss{};
    ss << "failed to open file `" << s_outPathname << '`';
    throw ss.str();
  }
}

void logger::write(EventType const evType, char const *const fmt, ...) {
  {
    static std::mutex mutex;
    std::scoped_lock const lock{mutex};

    if (!s_isFileReady) {
      std::ofstream file(s_outPathname); // clear file
      assert_file_opened(file);
      s_isFileReady = true;
    }

    va_list varArgs;
    va_start(varArgs, fmt);
    size_t const len = s_maxMsgLen + 1; // +1 for NUL
    char *const msg = new char[len];
    vsnprintf(msg, len, fmt, varArgs);
    va_end(varArgs);

    s_events.emplace_back(evType, msg);
    delete[] msg;
  }

  if (s_autoFlush) {
    logger::flush();
  }
}

void logger::flush() {
  static std::mutex mutex;
  std::scoped_lock const lock{mutex};

  std::ofstream file(s_outPathname, std::ios_base::app);
  assert_file_opened(file);

  for (auto const &evt : s_events) {
    if (!file.good()) {
      throw "bad file";
    }
    file << evt.stringify() << s_delim;
  }

  s_events.clear();
}
