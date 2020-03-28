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

int main()
{
  typedef std::pair<int, int> interval;
  typedef exp::algorithm::event<interval> event;
  std::multiset<event> set;
  std::vector<event> actives;

  std::vector<interval> intervals {{0, 5}, {0, 10}, {20, 30}, {0, 25}};
  std::copy (intervals.begin(), intervals.end(), exp::algorithm::interval_inserter<event> (set));

  exp::algorithm::scan_events
    (actives, set
     , [] (auto&& parameters, event const& e)
       {
         std::cout << "add begin parameters [" << std::endl;
         for (auto&& p : parameters)
         {
           std::cout << p << "," << std::endl;
         }
         std::cout << "] and " << e << std::endl;
       }
     , [] (auto&& parameters, event const& e)
       {
         std::cout << "removed begin parameters [" << std::endl;
         for (auto&& p : parameters)
         {
           std::cout << p << std::endl;
         }
         std::cout << "] " << e << std::endl;
       }
     );

  return -1;
}
