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
  typedef I interval_type;
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
typename algorithm::interval_api::interval_position_type<I>::type
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
  using algorithm::interval_api::get_interval_begin;
  using algorithm::interval_api::get_interval_end;
  return l.type == event_type::begin && r.type == event_type::begin
    ? ( 
       get_interval_begin (l.interval) == get_interval_begin (r.interval)
       ? get_interval_end (l.interval) < get_interval_end (r.interval)
       : get_interval_begin (l.interval) < get_interval_begin (r.interval)
      )
    : event_api::get_position(l) == event_api::get_position(r)
    ? l.type < r.type
    : event_api::get_position(l) < event_api::get_position(r);
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

template <typename Container, typename Derived>
struct interval_insert_iterator_base
{
  typedef Derived self_type;

  self_type& operator*()
  {
    return self();
  }

  interval_insert_iterator_base (Container* container)
    : container (container) {}

  self_type& operator++() { return self(); }
  self_type& operator++(int) { return self(); }

  self_type& self () { return static_cast<self_type&>(*this); }
  self_type const& self () const { return static_cast<self_type const&>(*this); }

  Container* container;
};
    
template <typename Event, typename Container>
struct associative_interval_insert_iterator : interval_insert_iterator_base<Container, associative_interval_insert_iterator<Event, Container>>
{
  typedef Event value_type;
  typedef Event* pointer;
  typedef Event& reference;
  typedef std::input_iterator_tag iterator_category;
  typedef std::ptrdiff_t difference_type;
  typedef interval_insert_iterator_base<Container, associative_interval_insert_iterator<Event, Container>> base_type;

  associative_interval_insert_iterator (Container* container)
    : base_type(container) {}
  
  template <typename Interval>
  Interval const& operator=(Interval const& i)
  {
    this->container->insert ({event_type::begin, i});
    this->container->insert ({event_type::end, i});
    return i;
  }
};

template <typename Event, typename Container>
struct sequence_interval_insert_iterator : interval_insert_iterator_base<Container, sequence_interval_insert_iterator<Event, Container>>
{
  typedef Event value_type;
  typedef Event* pointer;
  typedef Event& reference;
  typedef std::input_iterator_tag iterator_category;
  typedef std::ptrdiff_t difference_type;
  typedef interval_insert_iterator_base<Container, sequence_interval_insert_iterator<Event, Container>> base_type;

  sequence_interval_insert_iterator (Container* container)
    : base_type(container) {}
  
  template <typename Interval>
  Interval const& operator=(Interval const& i)
  {
    Event begin {event_type::begin, i};
    Event end {event_type::end, i};
    auto begin_it = std::lower_bound (this->container->begin(), this->container->end(), begin);
    this->container->insert (begin_it, std::move(begin));
    auto end_it = std::lower_bound (this->container->begin(), this->container->end(), end);
    this->container->insert (end_it, std::move(end));
    return i;
  }
};
    
namespace detail {

template <typename Container, typename Enable = std::void_t<>>
struct has_key_compare : std::false_type {};

template <typename Container>
struct has_key_compare<Container, std::void_t<typename Container::key_compare>>
  : std::true_type {};

}
    
template <typename Event, typename Container>
associative_interval_insert_iterator<Event, Container> interval_inserter
  (Container& c, typename std::enable_if<detail::has_key_compare<Container>::value>::type* = nullptr)
{
  return {&c};
}

template <typename Event, typename Container>
sequence_interval_insert_iterator<Event, Container> interval_inserter
  (Container& c, typename std::enable_if<!detail::has_key_compare<Container>::value>::type* = nullptr)
{
  return {&c};
}
    
} }

#endif
