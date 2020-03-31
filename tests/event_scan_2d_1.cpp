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
bool operator==(rectangle l, rectangle r)
{
  return l.x1 == r.x1 && l.x2 == r.x2 && l.y1 == r.y1 && l.y2 == r.y2;
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
     , [&] (auto&& parameters, event_0 const& e)
       {
         using exp::algorithm::event_api::get_opposite_event;
         overlapped_0.clear();
         *exp::algorithm::interval_inserter<event_1> (overlapped_0) = interval_1{e.interval.rectangle};

         assert (e.type == event_type::end);
         auto begin_0_insertion_point = std::lower_bound (actives_0.begin(), actives_0.end(), get_opposite_event(e));
         auto end_0_insertion_point = std::lower_bound (actives_0.begin(), actives_0.end(), e);

         for (auto current = actives_0.begin()
                ;current != begin_0_insertion_point; ++current)
         {
           using exp::algorithm::event_api::get_position;
           assert (current->type == event_type::begin);
           if (get_position (*current) != get_position (e))
             *exp::algorithm::interval_inserter<event_1> (overlapped_0) = interval_1{current->interval.rectangle};
         }
         for (auto current = begin_0_insertion_point
                ;current != end_0_insertion_point; ++current)
         {
           assert (current->type == event_type::begin);
           *exp::algorithm::interval_inserter<event_1> (overlapped_0) = interval_1{current->interval.rectangle};
         }

         actives_1.clear();
         exp::algorithm::scan_events (actives_1, overlapped_0
                                      , nullptr
                                      , [&] (auto&& parameters, event_1 const& e_1)
                                        {
                                          for (auto&& p : parameters)
                                          {
                                            using exp::algorithm::event_api::get_position;
                                            if (get_position (p) != get_position (e_1)
                                                && edges.find (std::make_pair(p.interval.rectangle, e_1.interval.rectangle)) == edges.end())
                                            {
                                              edges.insert (std::make_pair(e_1.interval.rectangle, p.interval.rectangle));
                                              std::cout << "overlap " << e_1.interval.rectangle << " with " << p.interval.rectangle << std::endl;
                                            }
                                          }
                                        });
       }
     );

  std::cout << "edges: " << edges.size() << std::endl;

  std::cout << "finsihed" << std::endl;
  return 0;
}
