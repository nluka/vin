#include <algorithm>
#include "util.hpp"
#include "movement.hpp"

using move::StartLoc, move::Destination;

Destination move::up(StartLoc const &startLoc) {
  if (bool const areWeOnFirstLine = startLoc.m_lineIdx == 0) {
    // don't move
    return {
      startLoc.m_lineIdx,
      startLoc.m_pos
    };
  }

  auto const &lineAbove = startLoc.m_lines[startLoc.m_lineIdx - 1];

  return {
    startLoc.m_lineIdx - 1,
    lineAbove.length() < startLoc.m_pos
      ? lineAbove.length()
      : startLoc.m_pos
  };
}

Destination move::down(StartLoc const &startLoc) {
  if (bool const areWeOnLastLine =
    startLoc.m_lineIdx == startLoc.m_lines.size() - 1
  ) {
    // don't move
    return {
      startLoc.m_lineIdx,
      startLoc.m_pos
    };
  }

  auto const &lineBelow = startLoc.m_lines[startLoc.m_lineIdx + 1];

  return {
    startLoc.m_lineIdx + 1,
    lineBelow.length() < startLoc.m_pos
      ? lineBelow.length()
      : startLoc.m_pos
  };
}

Destination move::left(StartLoc const &startLoc) {
  bool const areWeAtStartOfCurrLine = startLoc.m_pos == 0;

  return {
    // line index
    startLoc.m_lineIdx,
    // pos
    startLoc.m_pos + (-1 * !areWeAtStartOfCurrLine)
  };
}

Destination move::right(StartLoc const &startLoc) {
  auto const &currLine = startLoc.m_lines[startLoc.m_lineIdx];
  bool const areWeAtEndOfCurrLine = startLoc.m_pos == currLine.length();

  return {
    // line index
    startLoc.m_lineIdx,
    // pos
    startLoc.m_pos + (1 * !areWeAtEndOfCurrLine)
  };
}

Destination move::line_start(StartLoc const &startLoc) {
  auto const &currLine = startLoc.m_lines[startLoc.m_lineIdx];

  size_t const firstNonWhitespaceCharPos = ([&currLine]() -> size_t {
    if (currLine.empty()) {
      return 0;
    }

    for (size_t i = 0; i < currLine.length(); ++i) {
      char const c = currLine[i];
      if (!util::is_whitespace(c)) {
        return i;
      }
    }

    return currLine.length() - 1;
  })();

  return {
    // line index
    startLoc.m_lineIdx,
    // pos
    startLoc.m_pos > firstNonWhitespaceCharPos
      ? firstNonWhitespaceCharPos
      : 0
  };
}

Destination move::line_end(StartLoc const &startLoc) {
  auto const &currLine = startLoc.m_lines[startLoc.m_lineIdx];

  return {
    startLoc.m_lineIdx, // line index
    currLine.length()   // pos
  };
}
