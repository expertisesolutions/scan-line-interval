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
#include <algorithm/split_rectangles.hpp>
#include <algorithm/event.hpp>
#include <algorithm/event_scan.hpp>

#include <set>
#include <compare>

namespace exp { namespace algorithm {

namespace detail {

template <typename Event, typename Rectangle>
void erase_rectangle (std::multiset<Event>& set, Rectangle r)
{
  std::cout << "removing rectangle " << r << std::endl;
  using algorithm::event_type;
  auto range = set.equal_range ({event_type::begin, {r}});
  while (range.first != range.second && range.first->interval.rectangle != r)
    ++range.first;
  assert (range.first != range.second);
  set.erase (range.first);

  range = set.equal_range ({event_type::end, {r}});
  while (range.first != range.second && range.first->interval.rectangle != r)
    ++range.first;
  assert (range.first != range.second);
  set.erase (range.first);
}
  
template <typename Rectangle, std::size_t N>
struct interval_n
{
  typedef std::integral_constant<std::size_t, N> dim;
  typedef Rectangle rectangle_type;
  Rectangle rectangle;
  std::weak_ordering operator<=> (interval_n<Rectangle, N> const& other) const
  {
    return rectangle.i0 < other.rectangle.i0
         ? std::weak_ordering::less
         : other.rectangle.i0 < rectangle.i0
         ? std::weak_ordering::greater
         : std::weak_ordering::equivalent
         ;
  }

  bool operator== (interval_n<Rectangle, N> const& other) const
  {
    return !(rectangle.i0 < other.rectangle.i0)
      && !(other.rectangle.i0 < rectangle.i0);
  }
  friend std::ostream& operator<< (std::ostream& os, interval_n<Rectangle, N> const& i)
  {
    return os << "[ interval dim-" << N << " rectangle: " << i.rectangle << "]" << std::endl;
  }
};

}

namespace interval_api {

template <typename Rectangle>
struct interval_position_type<algorithm::detail::interval_n<Rectangle, 0>>
  : interval_position_type<typename Rectangle::i0_type>
 {};

template <typename Rectangle>
struct interval_position_type<algorithm::detail::interval_n<Rectangle, 1>>
  : interval_position_type<typename Rectangle::i1_type>
 {};
  
}

namespace detail {

template <typename Rectangle>
typename algorithm::interval_api::interval_position_type<typename Rectangle::i0_type>::type
get_interval_begin (interval_n<Rectangle, 0> const& i0)
{
  using algorithm::interval_api::get_interval_begin;
  return get_interval_begin (i0.rectangle.i0);
}

template <typename Rectangle>
typename algorithm::interval_api::interval_position_type<typename Rectangle::i1_type>::type
get_interval_begin (interval_n<Rectangle, 1> const& i1)
{
  using algorithm::interval_api::get_interval_begin;
  return get_interval_begin (i1.rectangle.i1);
}
  
template <typename Rectangle>
typename algorithm::interval_api::interval_position_type<typename Rectangle::i0_type>::type
get_interval_end (interval_n<Rectangle, 0> const& i0)
{
  using algorithm::interval_api::get_interval_end;
  return get_interval_end (i0.rectangle.i0);
}

template <typename Rectangle>
typename algorithm::interval_api::interval_position_type<typename Rectangle::i0_type>::type
get_interval_end (interval_n<Rectangle, 1> const& i1)
{
  using algorithm::interval_api::get_interval_end;
  return get_interval_end (i1.rectangle.i1);
}

template <typename Event1, typename Event0>
algorithm::sweep_interrupt handle_close_1 (std::vector<Event1> const& open_1, Event1 last_close_1
                                           , std::vector<Event0> const& open_0, Event0 first_close_0
                                           , std::multiset<Event0>& set)
{
  using algorithm::interval_api::get_interval_end;
  using algorithm::interval_api::get_interval_begin;

  std::cout << "handle_close_1 last_close_1 " << last_close_1 << " open_1 size " << open_1.size() << " first_close_0 " << first_close_0
            << " open_0 size " << open_0.size() << std::endl;
  auto current_1 = open_1.begin ();
  //Event0 const last_close_0 {last_close_1.type, {last_close_1.interval.rectangle}};

  // We have all open in dim-0 and in dim-1 and we're checking close_0 and last_close_1
  // which are not necessarily the same
  // The open in dim-1 is a subset of the open in dim-0
  // As a consequence, in open_1 they overlap with last_close_1.

  auto get_x1 = [] (auto&& e) { return detail::rget_x1(e.interval.rectangle); };
  auto get_x2 = [] (auto&& e) { return detail::rget_x2(e.interval.rectangle); };
  auto get_y1 = [] (auto&& e) { return detail::rget_y1(e.interval.rectangle); };
  auto get_y2 = [] (auto&& e) { return detail::rget_y2(e.interval.rectangle); };
  // The algorithm is: go through each element in open_1
  // for each one, see if it really overlaps and it is not the same as close_1
  // Then run rectangle splitting
  while (current_1 != open_1.end ()
         && (current_1->interval.rectangle == last_close_1.interval.rectangle
             || get_x2(*current_1) <= get_x1(last_close_1) // current_1 comes before last_close_1 begin dim-0
             || get_y2(*current_1) <= get_y1(last_close_1) // current_1 comes before last_close_1 begin dim-1
             || get_x2(last_close_1) <= get_x1(*current_1) // last_close_1 comes before current_1 begin dim-0
             || get_y2(last_close_1) <= get_y1(*current_1) // last_close_1 comes before current_1 begin dim-1
            ))
  {
    std::cout << "rect not usable (skipping) " << *current_1 << std::endl;
    ++current_1;
  }

  if (current_1 == open_1.end ())
  {
    std::cout << "no rectangle that overlap was found" << std::endl;
    return sweep_interrupt::continue_;
  }
  else
    std::cout << "found usable rectangle " << *current_1 << std::endl;

  auto dividend = last_close_1.interval.rectangle
    , divisor = current_1->interval.rectangle;

  detail::erase_rectangle (set, dividend);

  std::cout << "dividend " << dividend << " by divisor " << divisor << std::endl;
  
  using algorithm::interval_api::get_interval_begin;
  using algorithm::interval_api::get_interval_end;
  // divisor ends after dividend in dim-0

  bool closes_after_0 = rget_x2 (divisor) >= rget_x2 (dividend);
  bool opens_before_0 = rget_x1 (divisor) <= rget_x1 (dividend);
  bool opens_before_1 = rget_y1 (divisor) <= rget_y1 (dividend);
  
  std::cout << "closes_after_0 " << closes_after_0 << " opens_before_0 " << opens_before_0 << " opens_before_1 " << opens_before_1 << std::endl;

  std::vector<decltype(dividend)> split_rectangles;
  if (closes_after_0)
  {
    switch (static_cast<int>(opens_before_0) << 1 | static_cast<int>(opens_before_1))
    {
    case 0:
      std::cout << "!opens_before_0 and !opens_before_1 dividend " << dividend << " divisor " << divisor << std::endl;
      split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_after_t{}, algorithm::overlap_disposition_after_t{});
      break;
    case 1:
      std::cout << "!opens_before_0 and opens_before_1 dividend " << dividend << " divisor " << divisor << std::endl;
      split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_after_t{}, algorithm::overlap_disposition_after_t{});
      break;
    case 2:
      std::cout << "opens_before_0 and !opens_before_1 dividend " << dividend << " divisor " << divisor << std::endl;
      split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_across_t{}, algorithm::overlap_disposition_after_t{});
      break;
    case 3:
      // completely covers rectangle, just remove it
      break;
    }
  }
  else
  {
    switch (static_cast<int>(opens_before_0) << 1 | static_cast<int>(opens_before_1))
    {
    case 0:
      std::cout << "!opens_before_0 and !opens_before_1 dividend " << dividend << " divisor " << divisor << std::endl;
      split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_middle_t{}, algorithm::overlap_disposition_after_t{});
      break;
    case 1:
      std::cout << "!opens_before_0 and opens_before_1 dividend " << dividend << " divisor " << divisor << std::endl;
      split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_middle_t{}, algorithm::overlap_disposition_across_t{});
      break;
    case 2:
      std::cout << "opens_before_0 and !opens_before_1 dividend " << dividend << " divisor " << divisor << std::endl;
      split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_before_t{}, algorithm::overlap_disposition_after_t{});
      break;
    case 3:
      std::cout << "opens_before_0 and opens_before_1 dividend " << dividend << " divisor " << divisor << std::endl;
      split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_before_t{}, algorithm::overlap_disposition_across_t{});
      break;
    }
  }

  for (auto&& r : split_rectangles)
  {
    std::cout << "rectangle split " << r << std::endl;
    using exp::algorithm::event_api::get_opposite_event;
    Event0 e {event_type::begin, r};
    set.insert (e);
    set.insert (get_opposite_event (e));
  }
  
  return sweep_interrupt::break_;
}

template <typename Event0>
algorithm::sweep_interrupt handle_close_0 (std::vector<Event0> const& open_0, Event0 close
                                           , std::multiset<Event0>& set)
{
  std::cout << "handle_close_0" << std::endl;
  typedef typename Event0::interval_type::rectangle_type rectangle_type;
  typedef detail::interval_n<rectangle_type, 1> interval1;
  typedef exp::algorithm::event<interval1> event1;

  std::vector<event1> overlapped_at_0;
  for (auto&& e : open_0) // add all events from open intervals but in the next dimension
  {
    using exp::algorithm::event_api::get_position;
    // ignore any intervals that open in the same coordinate as this closes
    //if (get_position (e) != get_position (close) && e.interval.rectangle != close.interval.rectangle)
    *exp::algorithm::interval_inserter<event1> (overlapped_at_0) = interval1{e.interval.rectangle};
  }
  
  using std::placeholders::_1;
  using std::placeholders::_2;
  std::vector<event1> actives_1;
  auto r = algorithm::scan_events (actives_1, overlapped_at_0, nullptr
                                   , [open_0, close, &set] (auto&& a1, auto&& a2)
                                     { return handle_close_1<event1, Event0>(a1, a2, open_0, close, set); });
  std::cout << "returned to handle_close_0" << std::endl;
  return r;
}

}
    
template <typename Container>
Container rectangle_partition (Container rects)
{
  typedef typename Container::value_type rectangle;
  typedef detail::interval_n<rectangle, 0> interval0;
  typedef exp::algorithm::event<interval0> event;
  using exp::algorithm::event_type;
  std::multiset<event> set;
  std::copy (rects.begin(), rects.end(), exp::algorithm::interval_inserter<event> (set));
  
  std::vector<event> actives;

  algorithm::sweep_interrupt interrupt = algorithm::sweep_interrupt::break_;
  while (interrupt == algorithm::sweep_interrupt::break_)
  {
    actives.clear();
    interrupt = algorithm::scan_events (actives, set, nullptr,
                                        [&set] (auto&& a1, auto&& a2)
                                        { return detail::handle_close_0<event>(a1, a2, set); });
  }

  rects.clear();
  for (auto && s : set)
  {
    if (s.type == event_type::begin)
      rects.push_back (s.interval.rectangle);
  }
  
  return rects;
}

    
} }

#endif
