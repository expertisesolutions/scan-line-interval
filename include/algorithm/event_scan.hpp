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
#include <cassert>
#include <iostream>

namespace exp { namespace algorithm {

template <typename ActiveContainer, typename Container, typename Open, typename Close>
void scan_events (ActiveContainer&& actives, Container const& c, Open&& open, Close&& close)
{
  std::less<typename Container::value_type> compare;
  for (auto&& i : c)
  {
    using algorithm::event_api::is_begin_event;
    using algorithm::event_api::is_end_event;
    using algorithm::event_api::get_opposite_event;
    if (is_begin_event(i))
    {
      actives.push_back (i);
      open (actives, i);
    }
    else if (is_end_event(i))
    {
      auto opposite = get_opposite_event(i);
      //std::cout << "searching for " << opposite << std::endl;
      auto it = std::lower_bound (actives.begin(), actives.end(), opposite, compare);
      while (it != actives.end() && *it != opposite)
        ++it;
      assert (it != actives.end());
      actives.erase (it);
      close (actives, i);
    }
  }
}

template <typename ActiveContainer, typename Container, typename Close>
void scan_events (ActiveContainer&& actives, Container const& c, std::nullptr_t, Close&& close)
{
  std::less<typename Container::value_type> compare;
  for (auto&& i : c)
  {
    using algorithm::event_api::is_begin_event;
    using algorithm::event_api::is_end_event;
    using algorithm::event_api::get_opposite_event;
    if (is_begin_event(i))
    {
      actives.push_back (i);
    }
    else if (is_end_event(i))
    {
      auto opposite = get_opposite_event(i);
      //std::cout << "searching for " << opposite << std::endl;
      auto it = std::lower_bound (actives.begin(), actives.end(), opposite, compare);
      while (it != actives.end() && *it != opposite)
        ++it;
      assert (it != actives.end());
      actives.erase (it);
      close (actives, i);
    }
  }
}
    
} }

#endif
