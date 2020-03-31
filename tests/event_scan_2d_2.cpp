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
     , {  0,  25,  30 , 50}};
  std::copy (rectangles.begin(), rectangles.end(), exp::algorithm::interval_inserter<event_0> (set));

  exp::algorithm::scan_events
    (actives_0, set
     , nullptr
     , [&] (auto&& parameters_0, event_0 const& e_0)
       {
         //std::cout << "parameters.size() " << parameters.size() << " e_0 " << e_0 << std::endl;
         using exp::algorithm::event_api::get_opposite_event;
         overlapped_0.clear();

         assert (e_0.type == event_type::end);
         // auto begin_0_insertion_point = std::lower_bound (actives_0.begin(), actives_0.end(), get_opposite_event(e_0));
         // assert (begin_0_insertion_point != actives_0.end());
         // assert (*begin_0_insertion_point == get_opposite_event(e_0));
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
         exp::algorithm::scan_events (actives_1, overlapped_0
                                      , nullptr
                                      , [&] (auto parameters_1, event_1 const& e_1)
                                        {
                                          using exp::algorithm::event_api::get_position;
                                          std::cout << "overlaps for " << e_1 << std::endl;
                                          std::vector<rectangle> open_range_0;
                                          auto found_0 = std::lower_bound (parameters_0.begin(), parameters_0.end(), get_opposite_event(e_0));
                                          assert (std::lower_bound (parameters_0.begin(), parameters_0.end(), e_0) == parameters_0.end());
                                          assert (*found_0 == get_opposite_event(e_0));
                                          auto current_0 = parameters_0.begin();
                                          assert (found_0 != parameters_0.end());
                                          std::vector<rectangle> close_range_0;
                                          std::vector<rectangle> open_range_1;
                                          auto found_1 = std::lower_bound (parameters_1.begin(), parameters_1.end(), get_opposite_event(e_1));
                                          assert (*found_1 == get_opposite_event(e_1));
                                          // if (std::lower_bound (parameters_1.begin(), parameters_1.end(), e_1) != parameters_1.end())
                                          //   std::cout << "e_1 pos: " << *std::lower_bound (parameters_1.begin(), parameters_1.end(), e_1)
                                          //             << std::endl;
                                          // assert (std::lower_bound (parameters_1.begin(), parameters_1.end(), e_1) == parameters_1.end());
                                          auto current_1 = parameters_1.begin();
                                          std::vector<rectangle> close_range_1;
                                          auto get_rectangle = [] (auto&& e) { return e.interval.rectangle; };
                                          {
                                            
                                            bool found = false;
                                            while (current_0 != parameters_0.end()
                                                   && (!found || get_position(*current_0) == get_position(*found_0)))
                                            {
                                              if (current_0 == found_0)
                                                found = true;
                                              else
                                              {
                                                assert (current_1->type == event_type::begin);
                                                open_range_0.push_back (get_rectangle(*current_0));
                                              }
                                              ++current_0;
                                            }
                                          }
                                          assert (current_0 != found_0);
                                          std::transform
                                            (current_0, parameters_0.end()
                                             , std::back_inserter(close_range_0)
                                             , get_rectangle);
                                          {
                                            
                                            bool found = false;
                                            while (current_1 != parameters_1.end()
                                                   && (!found || get_position(*current_1) == get_position(*found_1)))
                                            {
                                              if (current_1 == found_1)
                                                found = true;
                                              else
                                              {
                                                assert (current_1->type == event_type::begin);
                                                open_range_1.push_back (get_rectangle(*current_1));
                                              }
                                              ++current_1;
                                            }
                                          }
                                          //assert (current_1 != found_1);
                                          std::transform
                                            (current_1
                                             , parameters_1.end()
                                             , std::back_inserter(close_range_1)
                                             , get_rectangle);

                                          std::cout << "open_range_0: " << open_range_0.size() << std::endl;
                                          for (auto&& r : open_range_0)
                                            std::cout << "   " << r << std::endl;
                                          std::cout << "close_range_0: " << close_range_0.size() << std::endl;
                                          for (auto&& r : close_range_0)
                                            std::cout << "   " << r << std::endl;
                                          std::cout << "open_range_1: " << open_range_1.size() << std::endl;
                                          for (auto&& r : open_range_1)
                                            std::cout << "   " << r << std::endl;
                                          std::cout << "close_range_1: " << close_range_1.size() << std::endl;
                                          for (auto&& r : close_range_1)
                                            std::cout << "   " << r << std::endl;

                                          std::vector<rectangle> open_open;
                                          std::vector<rectangle> open_close;
                                          std::vector<rectangle> close_close;
                                          std::vector<rectangle> close_open;
                                          auto fill = [] (std::vector<rectangle>& out, std::vector<rectangle> left, std::vector<rectangle> right)
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
                                                          std::cout << "searching " << a << " found " << *it << std::endl;
                                                          if (it != right.end() && a == *it)
                                                            out.push_back (a);
                                                        }
                                                      };
                                          fill (open_open, open_range_0, open_range_1);
                                          fill (open_close, open_range_0, close_range_1);
                                          fill (close_open, close_range_0, open_range_1);
                                          fill (close_close, close_range_0, close_range_1);

                                          std::cout << "open_open: " << open_open.size() << std::endl;
                                          for (auto&& r : open_open)
                                            std::cout << "   " << r << std::endl;
                                          std::cout << "open_close: " << open_close.size() << std::endl;
                                          for (auto&& r : open_close)
                                            std::cout << "   " << r << std::endl;
                                          std::cout << "close_open: " << close_open.size() << std::endl;
                                          for (auto&& r : close_open)
                                            std::cout << "   " << r << std::endl;
                                          std::cout << "close_close: " << close_close.size() << std::endl;
                                          for (auto&& r : close_close)
                                            std::cout << "   " << r << std::endl;
                                        });
       }
     );

  std::cout << "edges: " << edges.size() << std::endl;

  std::cout << "finsihed" << std::endl;
  return -1;
}
