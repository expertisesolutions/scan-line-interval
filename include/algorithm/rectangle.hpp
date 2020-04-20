///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_RECTANGLE_HPP
#define ALGORITHM_RECTANGLE_HPP

#include <algorithm/interval.hpp>

namespace exp { namespace algorithm {

template <typename I0, typename I1>
struct rectangle
{
  typedef I0 i0_type;
  typedef I1 i1_type;
  
  I0 i0;
  I1 i1;
};

template <typename I0, typename I1>
bool operator<(rectangle<I0, I1> const& r, rectangle<I0, I1> const& l)
{
  using interval_api::get_interval_begin;
  using interval_api::get_interval_end;
  return
      get_interval_begin (r.i0) == get_interval_begin (l.i0)
    ? get_interval_end (r.i0) == get_interval_end (l.i0)
    ? get_interval_begin (r.i1) == get_interval_begin (l.i1)
    ? get_interval_end (r.i1) < get_interval_end (l.i1)
    : get_interval_begin (r.i1) < get_interval_begin (l.i1)
    : get_interval_end (r.i0) < get_interval_end (l.i0)
    : get_interval_begin (r.i0) < get_interval_begin (l.i0)
    ;
}

template <typename I0, typename I1>
std::ostream& operator<<(std::ostream& os, rectangle<I0, I1> r)
{
  return os << "[ dim-0: " << r.i0 << " dim-1 " << r.i1 << " ]";
}

template <typename I0, typename I1>
bool operator==(rectangle<I0, I1> const& r, rectangle<I0, I1> const& l)
{
  return r.i0 == l.i0 && r.i1 == l.i1;
}

template <typename I0, typename I1>
bool operator!=(rectangle<I0, I1> const& r, rectangle<I0, I1> const& l)
{
  return r.i0 != l.i0 || r.i1 != l.i1;
}
    
} }

#endif
