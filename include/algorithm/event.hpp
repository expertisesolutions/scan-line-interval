///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_EVENT_HPP
#define ALGORITHM_EVENT_HPP

#include <algorithm/interval.hpp>

namespace exp { namespace algorithm {

enum class event_type
{
  begin, end
};

inline std::ostream& operator<<(std::ostream& os, event_type t)
{
  switch (t)
  {
  case event_type::begin:
    return os << "begin";
    break;
  case event_type::end:
    return os << "end";
    break;
  default:
    return os << "unknown";
    break;
  }
}
inline event_type operator~(event_type t)
{
  return t == event_type::begin ? event_type::end : event_type::begin;
}

template <typename I>
struct event
{
  event_type type;
  I interval;
};

namespace event_api {

template <typename I>
event<I> get_opposite_event (event<I> const& e)
{
  return {~e.type, e.interval};
}
template <typename I>
bool is_begin_event (event<I> const& e)
{
  return e.type == event_type::begin;
}
template <typename I>
bool is_end_event (event<I> const& e)
{
  return e.type == event_type::end;
}
template <typename I>
event_type get_event_type (event<I> const& e)
{
  return e.type;
}
    
template <typename I>
typename algorithm::interval_api::event_position_type<I>::type
get_position (event<I> const& e)
{
  using algorithm::interval_api::get_interval_begin;
  using algorithm::interval_api::get_interval_end;
  return e.type == event_type::begin ? get_interval_begin (e.interval)
    : get_interval_end (e.interval);
}

}

template <typename I>
bool operator<(event<I> const& l, event<I> const& r)
{
  return event_api::get_position(l) < event_api::get_position(r);
}
template <typename I>
bool operator>(event<I> const& l, event<I> const& r)
{
  return r < l;
}
template <typename I>
bool operator<=(event<I> const& l, event<I> const& r)
{
  return !(l > r);
}
template <typename I>
bool operator>=(event<I> const& l, event<I> const& r)
{
  return !(l < r);
}
template <typename I>
bool operator==(event<I> const& l, event<I> const& r)
{
  return l.type == r.type && l.interval == r.interval;
}
template <typename I>
bool operator!=(event<I> const& l, event<I> const& r)
{
  return !(l == r);
}
template <typename I>
std::ostream& operator<<(std::ostream& os, event<I> e)
{
  return os << "[type: " << e.type << " pos: " << event_api::get_position(e) << " interval " << e.interval << "]";
}
    
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
    container->insert ({event_type::begin, i});
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
