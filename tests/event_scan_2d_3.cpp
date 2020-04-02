///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm/event_scan.hpp>
#include <algorithm/interval.hpp>

#include <set>
#include <vector>
#include <iostream>
#include <variant>
#include <iterator>
#include <chrono>

struct vertical_line
{
  int x;
  std::pair<int, int> y_range;
};
// struct y_line
// {
//   int y;
//   std::pair<int, int> x_range;
// };
bool operator==(vertical_line l, vertical_line r)
{
  return l.x == r.x && l.y_range == r.y_range;
}
bool operator!=(vertical_line l, vertical_line r)
{
  return !(l == r);
}
bool operator<(vertical_line l, vertical_line r)
{
  return l.x == r.x
    ? l.y_range < r.y_range
    : l.x < r.x;
}
// bool operator==(y_line l, y_line r)
// {
//   return l.y == r.y && l.x_range == r.x_range;
// }
// bool operator!=(y_line l, y_line r)
// {
//   return !(l == r);
// }
// bool operator<(y_line l, y_line r)
// {
//   return l.y == r.y
//     ? l.x_range < r.x_range
//     : l.y < r.y;
// }

struct rectangle
{
  int x1, x2;
  int y1, y2;
};

bool operator<(rectangle l, rectangle r)
{
  return l.x1 == r.x1
    ? l.x2 == r.x2
    ? l.y1 == r.y1
    ? l.y2 < r.y2
    : l.y1 < r.y1
    : l.x2 < r.x2
    : l.x1 < r.x1
    ;
}
std::ostream& operator<<(std::ostream& os, rectangle r)
{
  return os << "rectangle [x1: " << r.x1 << " x2: " << r.x2 << " y1: " << r.y1 << " y2: " << r.y2 << "]";
}
bool operator==(rectangle const& l, rectangle const& r)
{
  return l.x1 == r.x1 && l.x2 == r.x2 && l.y1 == r.y1 && l.y2 == r.y2;
}
bool operator!=(rectangle l, rectangle r)
{
  return !(l == r);
}

template <std::size_t Dim>
struct interval
{
  struct rectangle rectangle;
};
template <std::size_t Dim>
bool operator==(interval<Dim> r, interval<Dim> l)
{
  return r.rectangle == l.rectangle;
}
template <std::size_t Dim>
bool operator!=(interval<Dim> r, interval<Dim> l)
{
  return r.rectangle != l.rectangle;
}
template <std::size_t Dim>
std::ostream& operator<<(std::ostream& os, interval<Dim> i)
{
  return os << "interval_" << Dim << "[" << i.rectangle << "]";
}

namespace exp { namespace algorithm { namespace interval_api {

template <std::size_t Dim>
struct interval_position_type<::interval<Dim>>
{
  typedef int type;
};
      
} } }

int get_interval_begin (interval<0> i)
{
  return i.rectangle.x1;
}
int get_interval_end (interval<0> i)
{
  return i.rectangle.x2;
}
int get_interval_begin (interval<1> i)
{
  return i.rectangle.y1;
}
int get_interval_end (interval<1> i)
{
  return i.rectangle.y2;
}

void erase_rectangle (std::multiset<exp::algorithm::event<interval<0>>>& set
                      , rectangle r)
{
  auto range = set.equal_range ({exp::algorithm::event_type::begin, {r}});
  while (range.first != range.second && range.first->interval.rectangle != r)
    ++range.first;
  assert (range.first != range.second);
  set.erase (range.first);

  range = set.equal_range ({exp::algorithm::event_type::end, {r}});
  while (range.first != range.second && range.first->interval.rectangle != r)
    ++range.first;
  assert (range.first != range.second);
  set.erase (range.first);
}

void add_rectangle (std::multiset<exp::algorithm::event<interval<0>>>& set
                    , rectangle r)
{
  if (r.x1 == r.x2 || r.y1 == r.y2)
    return;

  if (!(r.x1 < r.x2 && r.y1 < r.y2))
  {
    std::cout << "invalid rect " << r << std::endl;
  }
  assert (r.x1 < r.x2 && r.y1 < r.y2);
  
  typedef interval<0> interval_0;
  typedef exp::algorithm::event<interval_0> event_0;
  using exp::algorithm::event_api::get_opposite_event;
  auto e = event_0{exp::algorithm::event_type::begin, {r}};
  auto size = set.size();
  assert (size % 2 == 0);
  set.insert (e);
  set.insert (get_opposite_event(e));
  assert (size + 2 == set.size());
}

void add_rectangle_subtraction (std::multiset<exp::algorithm::event<interval<0>>>& set
                                , rectangle l, rectangle r)
{
  
}

void fill (std::vector<rectangle>& out, std::vector<rectangle> left, std::vector<rectangle> right)
{
  auto cmp = [] (rectangle r, rectangle l)
             {
               return r.y1 == l.y1
                 ? r.y2 < l.y2
                 : r.y1 < l.y1;
             };
  for (auto&& a : left)
  {
    auto it = std::lower_bound (right.begin(), right.end(), a, cmp);
    if (it != right.end() && a == *it)
    {
      out.push_back (a);
    }
  }
}

void handle_before_before (std::vector<rectangle> v
                           , bool until_end_0
                           , rectangle rold
                           , std::multiset<exp::algorithm::event<interval<0>>>& set)
{
  //std::cout << "before_before: " << v.size() << std::endl;
  // for (auto&& r : v)
  //   std::cout << "   " << r << std::endl;
  if (until_end_0)
  {
    //std::cout << "== completely overrides" << std::endl;
  }
  else 
  {
    int new_x1 = rold.x1;
    for (auto&& r : v)
      new_x1 = std::max (new_x1, r.x2);

    if (new_x1 < rold.x2)
    {
      // |-------------|
      // |    v[i]     |
      // |             |
      // |  |-------|  |
      // |  |xxxxxxx|  |
      // |--+-------+--|
      //    | rold  |
      //    |-------|
      // splits horizontal line, covering the top

      //std::cout << "== must redo rectangle making x1 be bigger, replacing for a single rectangle" << std::endl;
      rectangle rnew = {new_x1, rold.x2, rold.y1, rold.y2};
      add_rectangle (set, rnew);
    }
    // else ; // completely overrides it

  }
}

void handle_before_after (std::vector<rectangle> v
                          , bool until_end_0
                          , rectangle rold
                          , std::multiset<exp::algorithm::event<interval<0>>>& set)
{
  using exp::algorithm::event_api::get_opposite_event;
  //std::cout << "before_after: " << v.size() << std::endl;
  assert (!v.empty());
  // for (auto&& r : v)
  //   std::cout << "   " << r << std::endl;
  if (until_end_0)
  {
    //     |------|
    //     | rold |
    // |---|------|--|
    // |   |xxxxxx|  |
    // |   |xxxxxx|  |
    // |   |--+---|  |
    // |     v[i]    |
    // |-------------|
    // splits with horizontal line, covering the bottom

    int new_y2 = rold.y2;
    for (auto&& r : v)
      new_y2 = std::min(new_y2, r.y1);
    rectangle rnew {rold.x1, rold.x2, rold.y1, new_y2};
    add_rectangle (set, rnew);
    //std::cout << "== must redo rectangle making y2 be smaller, replacing for the rectangle " << rnew << std::endl;
  }
  else
  {
    //     |------|
    //     | rold |
    // |---|--|   |
    // |   |xx|   |
    // |   |xx|   |
    // |   |--+---|
    // | v[0] |
    // |------|
    // covers rectangle in bottom-left

    auto new_x1 = v[0].x2;
    auto new_y2 = v[0].y1;

    rectangle r1 {rold.x1, rold.x2, rold.y1, new_y2};
    assert (set.size() % 2 == 0);
    add_rectangle (set, r1);

    if (new_x1 < rold.x2)
    {
      rectangle r2 {new_x1, rold.x2, new_y2, rold.y2};
      add_rectangle (set, r2);
    } // otherwise, completely overrides second rectangle
    assert (set.size() % 2 == 0);
    //std::cout << "== replace overlap with " << v[0] << " with two rectangles " << r1 << " and " << r2 << std::endl;
  }
}

void handle_after_after (std::vector<rectangle> v
                         , bool until_end_0
                         , rectangle rold
                         , std::multiset<exp::algorithm::event<interval<0>>>& set)
{
  //std::cout << "== after_after: " << v.size() << std::endl;
  // for (auto&& r : v)
  //   std::cout << "   " << r << std::endl;
  if (until_end_0) // 
  {
    //
    //
    //  |---------|
    //  |rold     |
    //  |     |---+--|
    //  |     |xxx|  |
    //  |     |xxx|  |
    //  |-----+---|  |
    //        | v[i] |
    //        |------|
    //  covers rectangle in bottom-right
    
    auto new_x1 = v[0].x1;
    auto new_y2 = v[0].y1;

    rectangle r1 {rold.x1, new_x1, rold.y1, rold.y2};
    rectangle r2 {new_x1, rold.x2, rold.y1, new_y2};
    assert (set.size() % 2 == 0);
    add_rectangle (set, r1);
    add_rectangle (set, r2);
    assert (set.size() % 2 == 0);
    //std::cout << "== replace overlap with " << v[0] << " with two rectangles " << r1 << " and " << r2 << std::endl;
  }
  else
  {
    // |-------------|
    // |     rold    |
    // |             |
    // |   |------|  |
    // |   |xxxxxx|  |
    // |---+------+--|
    //     | v[i] |
    //     |------|
    //  covers rectangle in bottom-middle

    // three rectangles
    auto new_x2 = v[0].x1;
    auto new_x1 = v[0].x2;
    auto new_y1 = v[0].y1;

    rectangle r1 {rold.x1, rold.x2, rold.y1, new_y1};
    rectangle r2 {rold.x1, new_x2, new_y1, rold.y2};
    assert (set.size() % 2 == 0);
    add_rectangle (set, r1);
    add_rectangle (set, r2);
    if (new_x1 < rold.x2)
    {
      rectangle r3 {new_x1, rold.x2, new_y1, rold.y2};
      add_rectangle (set, r3);
    } // otherwise v[i] actually covers the bottom-right corner.
    assert (set.size() % 2 == 0);
    //std::cout << "== replace overlap with " << v[0] << " with three rectangles " << r1 << ", " << r2 << " and " << r3 << std::endl;
  }
}

void handle_after_before (std::vector<rectangle> v
                          , bool until_end_0
                          , rectangle rold
                          , std::multiset<exp::algorithm::event<interval<0>>>& set)
{
  // std::cout << "== after_before: " << v.size() << std::endl;
  // for (auto&& r : v)
  //   std::cout << "   " << r << std::endl;
  if (until_end_0) // 
  {
    //        |------|
    //        |      |
    //  |-----+---|  |
    //  |rold |xxx|  |
    //  |     |xxx|  |
    //  |     |xxx|  |
    //  |     |xxx|  |
    //  |-----+---|  |
    //        | v[i] |
    //        |------|
    // splits vertically, covering from the right

    //std::cout << "must redo rectangle making x be bigger, replacing for a single rectangle" << std::endl;

    auto new_x2 = v[0].x1;
    rectangle r1 {rold.x1, new_x2, rold.y1, rold.y2};
    add_rectangle (set, r1);
    //std::cout << "== replace overlap with " << v[0] << " with rectangle " << r1 << std::endl;
  }
  else
  {
    //        |------|
    //        |      |
    //  |-----+------+---|
    //  |rold |xxxxxx|   |
    //  |     |xxxxxx|   |
    //  |     |xxxxxx|   |
    //  |     |xxxxxx|   |
    //  |-----+------+---|
    //        | v[i] |
    //        |------|

    // splits vertically in two parts, covering the middle
    auto new_x2 = v[0].x1;
    auto new_x1 = v[0].x2;
    rectangle r1 {rold.x1, new_x2, rold.y1, rold.y2};
    add_rectangle (set, r1);

    if (new_x1 < rold.x2)
    {
      rectangle r2 {new_x1, rold.x2, rold.y1, rold.y2};
      add_rectangle (set, r2);
    } // otherwise v[i] covers bottom-right corner
    
    //std::cout << "== replace overlap with " << v[0] << " with two rectangle " << r1 << " and " << r2 << std::endl;
  }
}

void add_regions (std::multiset<exp::algorithm::event<interval<0>>>& set)
{
  std::pair<int, int> arcs[][4] =
    {
     {{10, 10}, {200,200}, {500, 1000}, {0, 0}}
     , {{380, 380}, {600,1600}, {500, 1000}, {0, 0}}
     , {{0, 100}, {600, 300}, {700, 600}, {1200, 1500}}
     , {{0, 0}, {1280, 0}, {0, 1000}, {1280, 1000}}
     , {{0, 0}, {1280 , 900}, {1, 900}, {1280, 1}}
    };

  std::pair<int, int> modifiers[] =
    {
     {0, 0}
     , {100, 100}
     , {20, 100}
     , {1000, 200}
     , {200, 1000}
     , {90, 300}
     , {300, 600}
    };

  auto first_mod = &modifiers[0]
    , last_mod = first_mod + sizeof(modifiers)/sizeof(modifiers[0]);
  for (; first_mod != last_mod; ++first_mod)
  {
    for (int x_mod = 0; x_mod < 500; x_mod += 150)
    {
      for (int y_mod = 0; y_mod != 500; y_mod += 10)
      {
        auto arc_first = &arcs[0]
          , arc_last = &arcs[0] + sizeof(arcs)/sizeof(arcs[0]);
        for (;arc_first != arc_last; ++arc_first)
        {
          if ((*arc_first)[3].first == 0
              && (*arc_first)[3].second == 0)
          {
            auto x1 = (*arc_first)[0].first + x_mod + first_mod->first;
            assert (x1 >= 0);
            auto x2 = (*arc_first)[0].second + y_mod + first_mod->second + x1;
            assert ((*arc_first)[0].second + y_mod + first_mod->second > 0);
            assert (x2 >= 0);
            auto y1 = (*arc_first)[2].first + x_mod + first_mod->first;
            assert (y1 >= 0);
            auto y2 = (*arc_first)[2].second + y_mod + first_mod->second + y1;
            assert (y2 >= 0);
            add_rectangle (set, {x1, x2, y1, y2});
          }
          else
          {
            auto x1 = (*arc_first)[0].first + x_mod + first_mod->first;
            assert (x1 >= 0);
            auto x2 = (*arc_first)[0].second + y_mod + first_mod->second + x1;
            assert ((*arc_first)[0].second + y_mod + first_mod->second >= 0);
            assert ((*arc_first)[0].second + y_mod + first_mod->second <= x2);
            assert (x2 >= 0);
            auto y1 = (*arc_first)[3].first + x_mod + first_mod->first;
            assert (y1 > 0);
            auto y2 = (*arc_first)[3].second + y_mod + first_mod->second + y1;
            assert (y2 > 0);
            add_rectangle (set, {x1, x2, y1, y2});
          }
        }
      }
    }
  }
}
                  

int main()
{
  using exp::algorithm::event_type;
  typedef interval<0> interval_0;
  typedef interval<1> interval_1;
  typedef exp::algorithm::event<interval_0> event_0;
  typedef exp::algorithm::event<interval_1> event_1;
  std::multiset<event_0> set;
  std::vector<event_0> actives_0;
  std::vector<event_1> overlapped_0;
  std::vector<event_1> actives_1;
  std::set<rectangle> overlapped_regions;

  std::set<std::pair<rectangle, rectangle>> edges;

  std::vector<rectangle> rectangles
    {
       {  0,  10,   0,  35}
     , {  0,   5,  20,  30}
     , { 20,  30,  10,  20}
     , {  0,  20,  30 , 50}
     , {  5,  20,  20,  35}};
  std::copy (rectangles.begin(), rectangles.end(), exp::algorithm::interval_inserter<event_0> (set));
  add_regions (set);

  auto now = std::chrono::high_resolution_clock::now();
  exp::algorithm::sweep_interrupt r = exp::algorithm::sweep_interrupt::break_;
  while (r == exp::algorithm::sweep_interrupt::break_)
  {
    actives_0.clear();
    //std::cout << "scan_events for x. Set size " << set.size() << std::endl;
  r = exp::algorithm::scan_events
    (actives_0, set
     , nullptr
     , [&] (auto&& parameters_0, event_0 const& e_0) -> exp::algorithm::sweep_interrupt
       {
         //std::cout << "X overlaps " << e_0 << std::endl;
         //for (auto&& p : parameters_0) std::cout << "    " << p << std::endl;
         using exp::algorithm::event_api::get_opposite_event;
         overlapped_0.clear();

         assert (e_0.type == event_type::end);
         auto end_0_insertion_point = std::lower_bound (parameters_0.begin(), parameters_0.end(), e_0);
         assert (end_0_insertion_point == parameters_0.end());

         for (auto current = parameters_0.begin()
                ;current != end_0_insertion_point; ++current)
         {
           using exp::algorithm::event_api::get_position;
           assert (current->type == event_type::begin);
           if (get_position (*current) != get_position (e_0))
             *exp::algorithm::interval_inserter<event_1> (overlapped_0) = interval_1{current->interval.rectangle};
         }

         actives_1.clear();
         auto r =
         exp::algorithm::scan_events (actives_1, overlapped_0
                                      , nullptr
                                      , [&] (auto parameters_1, event_1 const& e_1) -> exp::algorithm::sweep_interrupt
                                        {
                                          // std::cout << "overlaps for " << e_1 << std::endl;
                                          // std::cout << "parameters_0 " << parameters_0.size() << std::endl;
                                          // for (auto && p : parameters_0)
                                          // {
                                          //   std::cout << "    " << p << std::endl;
                                          // }
                                          //   std::cout << "parameters_1 " << parameters_1.size() << std::endl;
                                          //   for (auto&& p : parameters_1) std::cout << "    " << p << std::endl;
                                          if (parameters_1.size() == 1)
                                            return exp::algorithm::sweep_interrupt::continue_;
                                          else
                                          {
                                          }
                                          auto position_less = []
                                            (auto const& l, auto const& r) -> bool
                                            {
                                              using exp::algorithm::event_api::get_position;
                                              return get_position (l) == get_position (r)
                                                ? l.type < r.type
                                                : get_position (l) < get_position (r);
                                            };
                                                                 
                                          using exp::algorithm::event_api::get_position;
                                          std::vector<rectangle> before_open_range_0;
                                          auto found_open_0 = std::lower_bound (parameters_0.begin(), parameters_0.end()
                                                                                , event_0{exp::algorithm::event_type::begin
                                                                                            , interval_0{e_1.interval.rectangle}}
                                                                                , position_less);
                                          while (found_open_0->interval.rectangle != e_1.interval.rectangle)
                                            ++found_open_0;
                                          assert (found_open_0->interval.rectangle == e_1.interval.rectangle);
                                          auto open_last_0 = std::upper_bound (found_open_0, parameters_0.end(), get_opposite_event(e_1)
                                                                               , position_less);
                                          //assert (std::lower_bound (parameters_0.begin(), parameters_0.end(), e_0) == parameters_0.end());
                                          assert (found_open_0->interval.rectangle == get_opposite_event(e_1).interval.rectangle);
                                          assert (found_open_0 != parameters_0.end());
                                          std::vector<rectangle> after_open_range_0;
                                          std::vector<rectangle> before_open_range_1;
                                          auto found_open_1 = std::lower_bound (parameters_1.begin(), parameters_1.end(), get_opposite_event(e_1)
                                                                                , position_less);
                                          assert (*found_open_1 == get_opposite_event(e_1));
                                          auto open_last_1 = std::upper_bound (found_open_1, parameters_1.end(), get_opposite_event(e_1)
                                                                               , position_less);
                                          auto found_close_1 = std::lower_bound (found_open_1, parameters_1.end(), e_1
                                                                                 , position_less);
                                          {
                                            decltype(parameters_1.rend()) reverse_found_close_1 (found_close_1);
                                            for (; reverse_found_close_1 != parameters_1.rend()
                                                   && get_position (*reverse_found_close_1) == get_position(e_1)
                                                   ; ++reverse_found_close_1)
                                              ;
                                            found_close_1 = reverse_found_close_1.base();
                                          }
                                            
                                          std::vector<rectangle> after_open_range_1;
                                          auto get_rectangle = [] (auto&& e) { return e.interval.rectangle; };
                                          {
                                            auto current_0 = parameters_0.begin();
                                            while (current_0 != open_last_0)
                                            {
                                              //std::cout << "looping through " << *current_0 << std::endl;
                                              // skip my own rectangle
                                              if (current_0 != found_open_0)
                                              {
                                                assert (current_0->type == event_type::begin);
                                                //std::cout << "adding " << *current_0 << std::endl;
                                                before_open_range_0.push_back (get_rectangle(*current_0));
                                              }
                                              ++current_0;
                                            }
                                          }
                                          std::transform
                                            (open_last_0, parameters_0.end()
                                             , std::back_inserter(after_open_range_0)
                                             , get_rectangle);
                                          {
                                            auto current_1 = parameters_1.begin();
                                            while (current_1 != open_last_1)
                                            {
                                              if (current_1 != found_open_1)
                                              {
                                                assert (current_1->type == event_type::begin);
                                                before_open_range_1.push_back (get_rectangle(*current_1));
                                              }
                                              ++current_1;
                                            }
                                          }
                                          std::transform
                                            (open_last_1
                                             , found_close_1
                                             , std::back_inserter(after_open_range_1)
                                             , get_rectangle);

                                          if (before_open_range_1.empty()
                                              && after_open_range_1.empty())
                                            return exp::algorithm::sweep_interrupt::continue_;
                                          // else
                                            // std::cout << "before open range_1 " << before_open_range_1.size()
                                            //           << " after_open_range_1 " << after_open_range_1.empty()
                                            //           << "before open range_0 " << before_open_range_0.size()
                                            //           << " after_open_range_0 " << after_open_range_0.empty()
                                            //           << std::endl;

                                          std::vector<rectangle> before_before;
                                          std::vector<rectangle> before_after;
                                          std::vector<rectangle> after_after;
                                          std::vector<rectangle> after_before;

                                          ::fill (before_before, before_open_range_0, before_open_range_1);
                                          ::fill (before_after, before_open_range_0, after_open_range_1);
                                          ::fill (after_before, after_open_range_0, before_open_range_1);
                                          ::fill (after_after, after_open_range_0, after_open_range_1);

                                          if (before_before.empty()
                                              && before_after.empty()
                                              && after_before.empty()
                                              && after_after.empty())
                                          {
                                            //std::cout << "no override? for " << e_1.interval.rectangle << std::endl;
                                            return exp::algorithm::sweep_interrupt::continue_;
                                          }

                                          ::erase_rectangle (set, e_1.interval.rectangle);
                                          
                                          if (!before_before.empty())
                                            ::handle_before_before (before_before, e_0.interval.rectangle == e_1.interval.rectangle
                                                                    , e_1.interval.rectangle, set);
                                          if (!before_after.empty())
                                            ::handle_before_after (before_after, e_0.interval.rectangle == e_1.interval.rectangle
                                                                   , e_1.interval.rectangle, set);
                                          if (!after_before.empty())
                                            ::handle_after_before (after_before, e_0.interval.rectangle == e_1.interval.rectangle
                                                                   , e_1.interval.rectangle, set);
                                          if (!after_after.empty())
                                            ::handle_after_after (after_after, e_0.interval.rectangle == e_1.interval.rectangle
                                                                   , e_1.interval.rectangle, set);
                                          
                                          return exp::algorithm::sweep_interrupt::break_;
                                        });
         return r;
       }
     );
  }

  auto diff = std::chrono::high_resolution_clock::now() - now;
  std::cout << "Time calculation "
            << std::chrono::duration_cast<std::chrono::seconds>(diff).count()
            << "s" << std::endl;

  std::set <rectangle> rects;
  for (auto&& s : set)
  {
    rects.insert (s.interval.rectangle);
  }
  int bigger_x = 0, bigger_y = 0;
  long area = 0;
  std::cout << "new rectangles: " << rects.size() << std::endl;
  for (auto&& r : rects)
  {
    std::cout << "    " << r << std::endl;
    if (r.x2 > bigger_x)
      bigger_x = r.x2;
    if (r.y2 > bigger_y)
      bigger_y = r.y2;
    area += (r.x2-r.x1)*(r.y2-r.y1);
  }

  std::cout << "finsihed bigger_x: " << bigger_x << " bigger_y: " << bigger_y << " area " << area << std::endl;

  return -1;
}
