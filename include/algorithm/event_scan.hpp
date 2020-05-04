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

enum class sweep_interrupt
{
  continue_, break_
};

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
      close (actives, i);
      auto opposite = get_opposite_event(i);
      auto it = std::lower_bound (actives.begin(), actives.end(), opposite, compare);
      while (it != actives.end() && *it != opposite)
        ++it;
      assert (it != actives.end());
      actives.erase (it);
    }
  }
}

template <typename ActiveContainer, typename Container, typename Close>
std::enable_if<std::is_same<void, typename std::invoke_result<Close&&, ActiveContainer&&, typename Container::value_type&&>::type>::value>::type
  scan_events (ActiveContainer&& actives, Container const& c, std::nullptr_t, Close&& close)
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
      close (actives, i);
      auto opposite = get_opposite_event(i);
      auto it = std::lower_bound (actives.begin(), actives.end(), opposite, compare);
      while (it != actives.end() && *it != opposite)
        ++it;
      assert (it != actives.end());
      actives.erase (it);
    }
  }
}

template <typename ActiveContainer, typename Container, typename Close>
std::enable_if<std::is_same<sweep_interrupt, typename std::invoke_result<Close&&, ActiveContainer&&, typename Container::value_type&&>::type>::value, sweep_interrupt>::type
  scan_events (ActiveContainer&& actives, Container const& c, std::nullptr_t, Close&& close)
{
  typedef typename algorithm::interval_api::interval_position_type<typename Container::value_type::interval_type>::type position_type;
  position_type position = std::numeric_limits<position_type>::min();
  std::less<typename Container::value_type> const compare;
  auto it = c.begin(), last = c.end();
  while (it != last)
  {
    using algorithm::event_api::get_position;
    assert(std::is_sorted (actives.begin(), actives.end(), compare));
    auto i = *it;
    assert (it == last || get_position(i) <= get_position(*it) );
    assert (position <= get_position(i));
    position = get_position(i);
    assert(std::is_sorted (actives.begin(), actives.end(), compare));
    // if (Container::value_type::interval_type::dim::value)
    std::cout << "POSITION " << get_position(i) << " event to handle from set " << i << " (next is " << *it << ")" << std::endl;
    using algorithm::event_api::is_begin_event;
    using algorithm::event_api::is_end_event;
    using algorithm::event_api::get_opposite_event;
    if (is_begin_event(i))
    {
      // if (Container::value_type::interval_type::dim::value)
      // std::cout << "pushing to actives container of dim(" << Container::value_type::interval_type::dim::value << ") is " << i << std::endl;
      assert(std::is_sorted (actives.begin(), actives.end(), compare));
      actives.push_back (i);
      // if(!std::is_sorted (actives.begin(), actives.end(), compare))
      // {
      //   std::cout << "dumping actives:" << std::endl;
      //   for (auto&& a : actives)
      //   {
      //     std::cout << "    active: " << a << std::endl;
      //   }
      //   std::cout << "dumping current set:" << std::endl;
      //   for (auto&& a : c)
      //   {
      //     if (i == a)
      //       std::cout << "====>";
      //     std::cout << "    active: " << a << std::endl;
      //   }
      //   std::cout << "trying to add " << i << " at end" << std::endl;
      // }
      assert(std::is_sorted (actives.begin(), actives.end(), compare));
    }
    else if (is_end_event(i))
    {
      {
        assert(std::is_sorted (actives.begin(), actives.end(), compare));
        auto opposite = get_opposite_event(i);
        //std::cout << "event is " << i << " opposite is " << opposite << std::endl;
        auto cop = std::find (actives.begin(), actives.end(), opposite);
        //assert (cop != actives.end());
        if (cop != actives.end())
        {
          // std::cout << "found opposite from close in active container " << *cop << std::endl;
        }
        else
        {
          // std::cout << "Not found opposite in active container actives size " << actives.size() << std::endl;
          ++it;
          continue;
        }
        auto pair = std::equal_range (actives.begin(), actives.end(), opposite, compare);
        while (pair.first != pair.second && *pair.first != opposite)
        {
          // std::cout << "searching " << opposite << " checking equal range for " << *pair.first << " but is false, trying next" << std::endl;
          ++pair.first;
        }
        assert (pair.first == pair.second || *pair.first == opposite);
        if (pair.first != pair.second)
        {
          // std::cout << "calling close to " << opposite << " found " << *pair.first << " with normal " << i << std::endl;
        }
        else
        {
          // std::cout << "not found opposite " << opposite << " from " << i  << std::endl;
          // std::cout << "dumping actives:" << std::endl;
          // for (auto&& a : actives)
          // {
          //   std::cout << "    active: " << a << std::endl;
          // }
        }
        assert (pair.first != pair.second);
      }
      assert(std::is_sorted (actives.begin(), actives.end(), compare));
      if (close (actives, i) == sweep_interrupt::break_)
        return sweep_interrupt::break_;
      assert(std::is_sorted (actives.begin(), actives.end(), compare));
      auto opposite = get_opposite_event(i);
      auto pair = std::equal_range (actives.begin(), actives.end(), opposite, compare);
      while (pair.first != pair.second && *pair.first != opposite)
        ++pair.first;
      assert (pair.first == pair.second || *pair.first == opposite);
      if (pair.first != pair.second)
      {
        // std::cout << "erasing from actives " << *pair.first << " suppose to erase " << opposite << std::endl;
        actives.erase (pair.first);
      }
      // else
      //   std::cout << "not found " << opposite << std::endl;
      assert (pair.first != pair.second);
      assert(std::is_sorted (actives.begin(), actives.end(), compare));
    }
      assert(std::is_sorted (actives.begin(), actives.end(), compare));
    ++it;
  }
  return sweep_interrupt::continue_;
}
    
} }

#endif
