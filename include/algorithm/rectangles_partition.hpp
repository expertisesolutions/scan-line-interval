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

template <typename Event0, typename Open0, typename Open1, typename Overlapped, typename Rectangle>
void erase_rectangle (std::multiset<Event0>& set, Open0& open0, Open1& open1, Overlapped& overlapped_at_0, Rectangle r)
{
  // std::cout << "erasing " << r << std::endl;
  using algorithm::event_type;
  auto range = set.equal_range ({event_type::begin, {r}});
  while (range.first != range.second && range.first->interval.rectangle != r)
    ++range.first;
  assert (range.first != range.second);
  assert (range.first->interval.rectangle == r);
  auto s = set.size();
  set.erase (range.first);
  assert (set.size() + 1 == s);

  range = set.equal_range ({event_type::end, {r}});
  while (range.first != range.second && range.first->interval.rectangle != r)
    ++range.first;
  assert (range.first != range.second);
  assert (range.first->interval.rectangle == r);
  s = set.size();
  // std::cout << "erasing from set " << *range.first << std::endl;
  set.erase (range.first);
  assert (set.size() + 1 == s);

  {
    typename Open0::value_type event0 {event_type::begin, r};
    // std::cout << "searching dim-0 " << event0 << std::endl;
    assert (std::find (open0.begin(), open0.end(), event0) != open0.end());
    auto it = std::lower_bound (open0.begin(), open0.end(), event0);
    assert (it != open0.end());
    while (it != open0.end() && it->interval.rectangle != r)
      ++it;
    assert (it != open0.end());
    // std::cout << "it: " << *it << " searching " << event0 << " old size open0 " << open0.size() << std::endl;
    assert (*it == event0);
    open0.erase (it);
    // std::cout << " open0 new size " << open0.size() << std::endl;
  }

  {
    typename Overlapped::value_type event1 {event_type::begin, r};
    assert (std::find (overlapped_at_0.begin(), overlapped_at_0.end(), event1) != overlapped_at_0.end());
    auto it = std::lower_bound (overlapped_at_0.begin(), overlapped_at_0.end(), event1);
    assert (it != overlapped_at_0.end());
    while (it != overlapped_at_0.end() && it->interval.rectangle != r)
      ++it;
    assert (it != overlapped_at_0.end());
    // std::cout << "overlap open it: " << *it << " searching " << event1 << " old size overlap " << overlapped_at_0.size() << std::endl;
    assert (*it == event1);
    overlapped_at_0.erase (it);
    // std::cout << " overlap new size " << open0.size() << std::endl;

    using event_api::get_opposite_event;
    auto op_event1 = get_opposite_event(event1);
    auto end_it = std::lower_bound (overlapped_at_0.begin(), overlapped_at_0.end(), op_event1);
    assert (end_it != overlapped_at_0.end());
    while (end_it != overlapped_at_0.end() && end_it->interval.rectangle != r)
      ++end_it;
    assert (end_it != overlapped_at_0.end());
    // std::cout << "overlap close it: " << *end_it << " searching " << op_event1 << " old size overlap " << overlapped_at_0.size() << std::endl;
    assert (*end_it == op_event1);
    overlapped_at_0.erase (end_it);
    // std::cout << " overlap new size " << open0.size() << std::endl;
  }

  {
    typename Open1::value_type event1 {event_type::begin, r};
    // std::cout << "searching dim-1 " << event1 << std::endl;
    assert (std::find (open1.begin(), open1.end(), event1) != open1.end());
    auto it = std::lower_bound (open1.begin(), open1.end(), event1);
    assert (it != open1.end());
    while (it != open1.end() && it->interval.rectangle != r)
      ++it;
    assert (*it == event1);
    // std::cout << "it: " << *it << " searching " << event1 << " open1 old size " << open1.size() << std::endl;
    assert (*it == event1);
    open1.erase (it);
    // std::cout << " open1 new size " << open1.size() << std::endl;

    // std::cout << "new (after erase) open_1:" << std::endl;
    // for (auto& no1 : open1)
    //   std::cout << "     elem: " << no1 << std::endl;
  }
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
    return rectangle.i0 == other.rectangle.i0
      && rectangle.i1 == other.rectangle.i1;
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
algorithm::sweep_interrupt handle_close_1 (std::vector<Event1>& open_1, Event1 last_close_1
                                           , std::vector<Event0>& open_0, Event0 first_close_0
                                           , std::multiset<Event0>& set, std::multiset<Event1>& overlapped_at_0)
{
  using algorithm::interval_api::get_interval_end;
  using algorithm::interval_api::get_interval_begin;

  // std::cout << "last_close_1 " << last_close_1 << " open_1.size() " << open_1.size() << " open0.size() " << open_0.size() << std::endl;
  
  //auto current_1 = open_1.begin ();
  std::size_t current_index_1 = 0;

  auto current_1 = [&] { return open_1[current_index_1]; };

  // std::cout << "open_0(" << open_0.size() << "): " << std::endl;
  // for (auto& o0 : open_0)
  //   std::cout << "     elem: " << o0 << std::endl;
  // std::cout << "open_1:" << std::endl;
  // for (auto& o1 : open_1)
  //   std::cout << "     elem: " << o1 << std::endl;
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
  while (current_index_1 != open_1.size())
  {
  while (current_index_1 != open_1.size()
         && (current_1().interval.rectangle == last_close_1.interval.rectangle
             || get_x2(current_1()) <= get_x1(last_close_1) // current_1 comes before last_close_1 begin dim-0
             || get_y2(current_1()) <= get_y1(last_close_1) // current_1 comes before last_close_1 begin dim-1
             || get_x2(last_close_1) <= get_x1(current_1()) // last_close_1 comes before current_1 begin dim-0
             || get_y2(last_close_1) <= get_y1(current_1()) // last_close_1 comes before current_1 begin dim-1
            ))
  {
    ++current_index_1;
  }

  if (current_index_1 == open_1.size ())
  {
    return sweep_interrupt::continue_;
  }

  assert (std::find_if (open_1.begin(), open_1.end()
                        , [&] (Event1 const& e1)
                          { return e1.interval.rectangle == last_close_1.interval.rectangle; }) != open_1.end());

  auto divisor = last_close_1.interval.rectangle
    , dividend = current_1().interval.rectangle;

  // std::cout << "dividend " << dividend << " divisor " << divisor << std::endl;

  using algorithm::interval_api::get_interval_begin;
  using algorithm::interval_api::get_interval_end;
  // divisor ends after dividend in dim-0

  bool closes_after_0 = rget_x2 (divisor) >= rget_x2 (dividend);
  bool opens_before_0 = rget_x1 (divisor) <= rget_x1 (dividend);
  bool opens_before_1 = rget_y1 (divisor) <= rget_y1 (dividend);
  bool closes_after_1 = rget_y2 (divisor) >= rget_y2 (dividend);
  
  std::vector<decltype(dividend)> split_rectangles;
  switch (static_cast<int>(closes_after_0) << 3 | static_cast<int>(closes_after_1) << 2 | static_cast<int>(opens_before_0) << 1 | static_cast<int>(opens_before_1))
  {
  case 0b0000:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_middle_t{}, algorithm::overlap_disposition_middle_t{});
    break;
  case 0b0001:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_middle_t{}, algorithm::overlap_disposition_before_t{});
    break;
  case 0b0010:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_before_t{}, algorithm::overlap_disposition_middle_t{});
    break;
  case 0b0011:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_before_t{}, algorithm::overlap_disposition_before_t{});
    break;
  case 0b0100:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_middle_t{}, algorithm::overlap_disposition_after_t{});
    break;
  case 0b0101:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_middle_t{}, algorithm::overlap_disposition_across_t{});
    break;
  case 0b0110:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_before_t{}, algorithm::overlap_disposition_after_t{});
    break;
  case 0b0111:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_before_t{}, algorithm::overlap_disposition_across_t{});
    break;
  case 0b1000:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_after_t{}, algorithm::overlap_disposition_middle_t{});
    break;
  case 0b1001:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_after_t{}, algorithm::overlap_disposition_before_t{});
    break;
  case 0b1010:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_across_t{}, algorithm::overlap_disposition_middle_t{});
    break;
  case 0b1011:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_across_t{}, algorithm::overlap_disposition_before_t{});
    break;
  case 0b1100:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_after_t{}, algorithm::overlap_disposition_after_t{});
    break;
  case 0b1101:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_after_t{}, algorithm::overlap_disposition_across_t{});
    break;
  case 0b1110:
    split_rectangles = algorithm::split_rectangle (dividend, divisor, algorithm::overlap_disposition_across_t{}, algorithm::overlap_disposition_after_t{});
    break;
  case 0b1111:
    // completely covers rectangle, just remove it
    break;
  }
  detail::erase_rectangle (set, open_0, open_1, overlapped_at_0, dividend);
  
  for (auto&& r : split_rectangles)
  {
    // std::cout << "    split rectangle " << r << std::endl;
    using exp::algorithm::event_api::get_opposite_event;
    Event0 e0 {event_type::begin, r};
    Event1 e1 {event_type::begin, r};
    set.insert (e0);
    set.insert (get_opposite_event (e0));
    overlapped_at_0.insert (e1);
    overlapped_at_0.insert (get_opposite_event(e1));

    std::less<Event0> const e0_compare;
    if (e0_compare(e0, first_close_0))
    {
      // std::cout << "adding open0 " << e0 << " while compared to " << first_close_0 << std::endl;
      auto it = std::lower_bound (open_0.begin(), open_0.end(), e0, e0_compare);
      open_0.insert (it, e0);
      std::less<Event1> const e1_compare;
      if (e1_compare(e1, last_close_1))
      {
        // std::cout << "adding open1 " << e1 << " while compared to " << last_close_1 << std::endl;
        auto it = std::lower_bound (open_1.begin(), open_1.end(), e1, e1_compare);
        open_1.insert (it, e1);
      }
    }
  }


  }  
  //return sweep_interrupt::break_;
  return sweep_interrupt::continue_;
}

template <typename Event0>
algorithm::sweep_interrupt handle_close_0 (std::vector<Event0>& open_0, Event0 close
                                           , std::multiset<Event0>& set)
{
  // std::cout << "handle_close_0 close_0 " << close << std::endl;
  // std::cout << "open_0 (" << open_0.size() << ":" << std::endl;
  // for (auto& o0 : open_0)
  //   std::cout << "     elem: " << o0 << std::endl;
  using algorithm::event_api::get_opposite_event;
  assert (std::find (open_0.begin(), open_0.end(), get_opposite_event(close)) != open_0.end());
  typedef typename Event0::interval_type::rectangle_type rectangle_type;
  typedef detail::interval_n<rectangle_type, 1> interval1;
  typedef exp::algorithm::event<interval1> event1;

  std::multiset<event1> overlapped_at_0;
  for (auto&& e : open_0) // add all events from open intervals but in the next dimension
  {
    using exp::algorithm::event_api::get_position;
    // ignore any intervals that open in the same coordinate as this closes
    //if (get_position (e) != get_position (close) && e.interval.rectangle != close.interval.rectangle)
    *exp::algorithm::interval_inserter<event1> (overlapped_at_0) = interval1{e.interval.rectangle};
  }

  // std::cout << "overlapped_at_0 (" << overlapped_at_0.size() << "): " << std::endl;
  // for (auto& o0 : overlapped_at_0)
  //   std::cout << "     elem: " << o0 << std::endl;
  
  using std::placeholders::_1;
  using std::placeholders::_2;
  std::vector<event1> actives_1;
  auto r = algorithm::scan_events (actives_1, overlapped_at_0, nullptr
                                   , [&open_0, close, &set, &overlapped_at_0] (auto&& a1, auto&& a2)
                                     { return handle_close_1<event1, Event0>(a1, a2, open_0, close, set, overlapped_at_0); });
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
      rects.insert (s.interval.rectangle);
  }
  
  return rects;
}

    
} }

#endif
