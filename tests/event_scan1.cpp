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

struct interval
{
  int x1, x2;
};

bool operator==(interval l, interval r)
{
  return l.x1 == r.x1 && l.x2 == r.x2;
}

std::ostream& operator<<(std::ostream& os, interval i)
{

  return os << "[x1: " << i.x1 << " x2: " << i.x2 << "]" << std::endl;
}

struct event_event
{
  exp::algorithm::event_type type;
  int position;
  interval i;

  event_event (exp::algorithm::event_type type, interval i)
    : type (type), i (i)
  {
    if (type == exp::algorithm::event_type::start)
      position = i.x1;
    else
      position = i.x2;
  }
};

bool operator==(event_event const& e, exp::algorithm::event_type p)
{
  return p == e.type;
}
bool operator==(exp::algorithm::event_type p, event_event const& e)
{
  return e == p;
}
bool operator<(event_event const& l, event_event const& r)
{
  return l.position < r.position;
}
std::ostream& operator<<(std::ostream& os, event_event e)
{
  return os << "[type: " << e.type << " pos: " << e.position << " interval " << e.i << "]";
}
bool match_event_edges (event_event l, event_event r)
{
  return  l.i == r.i && l.type != r.type;
}

int main()
{
  std::multiset<event_event> set;
  std::vector<event_event> actives;

  std::vector<interval> intervals {{0, 10}, {20, 30}, {0, 25}};
  std::copy (intervals.begin(), intervals.end(), exp::algorithm::interval_inserter<event_event> (set));

  exp::algorithm::scan_events
    (actives, set
     , [] (auto&& parameters, event_event const& e)
       {
         std::cout << "parameters [" << std::endl;
         for (auto&& p : parameters)
         {
           std::cout << p << "," << std::endl;
         }
         std::cout << "] and " << e << std::endl;
       }
     , [] (auto&& parameters, event_event const& e)
       {
         std::cout << "parameters [" << std::endl;
         for (auto&& p : parameters)
         {
           std::cout << p << std::endl;
         }
         std::cout << "] " << e << std::endl;
       }
     );

  return -1;
}
