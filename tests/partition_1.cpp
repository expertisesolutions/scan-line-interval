///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm/rectangles_partition.hpp>

#include <set>
#include <vector>
#include <iostream>

int main()
{
  typedef std::pair<int, int> interval;
  typedef exp::algorithm::rectangle<interval, interval> rectangle;
  std::vector<rectangle> rects
        {
           { { 0,  10}, { 0,  35}}
         , { { 0,   5}, { 20,  30}}
         , { {20,  30}, { 10,  20}}
         , { { 0,  20}, { 30 , 50}}
         , { { 5,  20}, { 20,  35}}};
  
  exp::algorithm::rectangle_partition (rects);

  

  return 0;

}
