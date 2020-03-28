///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_INTERVAL_HPP
#define ALGORITHM_INTERVAL_HPP

#include <algorithm>
#include <ostream>

namespace exp { namespace algorithm {

namespace interval_api {

template <typename T>
std::ostream& operator<<(std::ostream& os, std::pair<T, T> i)
{
  return os << "[x1: " << i.first << " x2: " << i.second << "]" << std::endl;
}
  
template <typename T, typename Enable = void>
struct interval_position_type;

template <typename T>
struct interval_position_type<std::pair<T, T>>
{
  typedef T type;
};
  
template <typename T>
T get_interval_begin (std::pair<T, T> i) { return i.first; }

template <typename T>
T get_interval_end (std::pair<T, T> i) { return i.second; }

}

using namespace interval_api;
  
} }

#endif
