///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_INTERVAL_HPP
#define ALGORITHM_INTERVAL_HPP

#include <algorithm/event.hpp>

#include <algorithm>
#include <ostream>

namespace exp { namespace algorithm {

template <typename T>
T get_interval_begin (std::pair<T, T> i) { return i.first; }

template <typename T>
T get_interval_end (std::pair<T, T> i) { return i.second; }
    
template <typename Event, typename Container>
struct interval_insert_iterator
{
  typedef Event value_type;
  typedef Event* pointer;
  typedef Event& reference;
  typedef std::input_iterator_tag iterator_category;
  typedef std::ptrdiff_t difference_type;
  
  
  typedef interval_insert_iterator<Event, Container> self_type;
  
  self_type& operator*()
  {
    return *this;
  }

  template <typename Interval>
  Interval const& operator=(Interval const& i)
  {
    container->insert ({event_type::start, i});
    container->insert ({event_type::end, i});
    return i;
  }

  self_type& operator++() { return *this; }
  self_type& operator++(int) { return *this; }

  Container* container;
};
    
template <typename Event, typename Container>
interval_insert_iterator<Event, Container> interval_inserter (Container& c)
{
  return {&c};
}
    
} }

#endif
