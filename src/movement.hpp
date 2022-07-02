#ifndef VIN_MOVEMENT_HPP
#define VIN_MOVEMENT_HPP

#include <vector>
#include <string>

namespace move {

// A location from which a movement is initiated.
struct StartLoc {
  std::vector<std::string> const &m_lines;
  size_t m_lineIdx;
  size_t m_pos;
};

// The result of a movement.
struct Destination {
  size_t m_lineIdx;
  size_t m_pos;
};

Destination up(StartLoc const &);
Destination down(StartLoc const &);

Destination left(StartLoc const &);
Destination right(StartLoc const &);

Destination line_start(StartLoc const &);
Destination line_end(StartLoc const &);

} // namespace move

#endif // VIN_MOVEMENT_HPP
