///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_RECTANGLES_PARTITION_HPP
#define ALGORITHM_RECTANGLES_PARTITION_HPP

#include <algorithm/rectangle.hpp>
#include <algorithm/event.hpp>
#include <algorithm/event_scan.hpp>

#include <set>
#include <compare>

namespace exp { namespace algorithm {

namespace detail {

template <typename Rectangle>
struct interval0
{
  Rectangle rectangle;
  std::weak_ordering operator<=> (interval0<Rectangle> const& other) const
  {
    return rectangle.i0 < other.rectangle.i0
         ? std::weak_ordering::less
         : other.rectangle.i0 < rectangle.i0
         ? std::weak_ordering::greater
         : std::weak_ordering::equivalent
         ;
  }

  bool operator== (interval0<Rectangle> const& other) const
  {
    return !(rectangle.i0 < other.rectangle.i0)
      && !(other.rectangle.i0 < rectangle.i0);
};

template <typename Rectangle>
struct interval1
{
  Rectangle rectangle;
};
  
template <typename Event>
algorithm::sweep_interrupt handle_close_1 (std::vector<Event> const& open_1, Event close_1
                                           , std::vector<Event> const& open_0, Event close_0)
{
  auto current_1 = open_1.begin ();

  while (current_1 != open_1.end ())
  {
    auto current_0 = open_0.begin ();
    bool before_0 = true, before_1 = true;

    if (current_1->interval != close_1.interval)
      ++current_1, before_1 = false;

    if (current_1 == open_1.end ())
      break;

    while (current_1->interval != current_0->interval)
    {
      if (current_0->interval == current_1->interval)
        before_0 = false;
      ++current_0;
    }

    auto rect1 = current_1->interval.rectangle;
    auto rect2 = close_0.interval.rectangle;
    if (rect1.x2 <= rect2.x1 || rect1.y2 <= rect2.y1 || rect1.x1 >= rect2.x2 || rect1.y1 >= rect2.y2)
      ++current_1;
    else
      break;
  }

  if (current_1 == open_1.end ())
    return sweep_interrupt::continue_;

  //detail::erase_rectangle (set, close_1.interval.rectangle);

  //
  return sweep_interrupt::break_;
}

template <typename Event>
algorithm::sweep_interrupt handle_close_0 (std::vector<Event> const& open_0, Event close)
{
  //overlapped_0.clear ();
  std::vector<Event> overlapped_0;

  std::copy (open_0.begin (), open_0.end (), algorithm::interval_inserter<Event> (overlapped_0));

  // actives_1.clear ();
  using std::placeholders::_1;
  using std::placeholders::_2;
  std::vector<Event> actives_1;
  auto r = algorithm::scan_events (actives_1, overlapped_0, nullptr, std::bind(&handle_close_1<Event>, _1, _2, std::ref(open_0), std::ref(close)));
  return r;
}

}
    
template <typename Container>
Container rectangle_partition (Container rects)
{
  typedef typename Container::value_type rectangle;
  typedef detail::interval0<rectangle> interval0;
  typedef exp::algorithm::event<interval0> event;
  std::multiset<event> set;
  std::vector<event> actives;

  
  algorithm::sweep_interrupt interrupt = algorithm::sweep_interrupt::break_;
  while (interrupt == algorithm::sweep_interrupt::break_)
  {
    auto r = algorithm::scan_events (actives, set, nullptr, & detail::handle_close_0<event>);

  }
  
  return rects;
}

    
} }

#endif
