///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_EVENT_SCAN_HPP
#define ALGORITHM_EVENT_SCAN_HPP

#include <algorithm/event.hpp>

#include <algorithm>
#include <ostream>

namespace exp { namespace algorithm {

inline std::ostream& operator<<(std::ostream& os, event_type t)
{
  switch (t)
  {
  case event_type::start:
    return os << "start";
    break;
  case event_type::end:
    return os << "end";
    break;
  default:
    return os << "unknown";
    break;
  }
}

template <typename ActiveContainer, typename Container, typename Open, typename Close>
void scan_events (ActiveContainer&& actives, Container const& c, Open&& open, Close&& close)
{
  std::less<typename Container::value_type> compare;
  for (auto&& i : c)
  {
    if (i == event_type::start)
    {
      actives.push_back (i);
      open (actives, i);
    }
    else
    {
      // to be faster... lower_bound on position
      auto it = std::lower_bound (actives.begin(), actives.end(), i, compare);
      // std::remove_if (actives.begin(), actives.end(), [&i] (auto&& other) { return match_event_edges (other, i); });
      // actives.pop_back ();
      //while (
      actives.erase (it);
      close (actives, i);
    }
  }
}
    
} }

#endif
