///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_SPLIT_RECTANGLES_HPP
#define ALGORITHM_SPLIT_RECTANGLES_HPP

#include <cassert>
#include <iostream>

namespace exp { namespace algorithm {

enum class rectangle_overlap_disposition
{
  before, middle, after, across 
};

typedef std::integral_constant<rectangle_overlap_disposition, rectangle_overlap_disposition::before> overlap_disposition_before_t;
typedef std::integral_constant<rectangle_overlap_disposition, rectangle_overlap_disposition::middle> overlap_disposition_middle_t;
typedef std::integral_constant<rectangle_overlap_disposition, rectangle_overlap_disposition::after> overlap_disposition_after_t;
typedef std::integral_constant<rectangle_overlap_disposition, rectangle_overlap_disposition::across> overlap_disposition_across_t;

namespace detail {

template <typename R>
auto rget_x1 (R&& r) { using algorithm::interval_api::get_interval_begin; return get_interval_begin(r.i0); }
template <typename R>
auto rget_x2 (R&& r) { using algorithm::interval_api::get_interval_end; return get_interval_end(r.i0); }
template <typename R>
auto rget_y1 (R&& r) { using algorithm::interval_api::get_interval_begin; return get_interval_begin(r.i1); }
template <typename R>
auto rget_y2 (R&& r) { using algorithm::interval_api::get_interval_end; return get_interval_end(r.i1); }

}
    
template <typename Rectangle>
std::vector<Rectangle> split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_before_t, overlap_disposition_after_t)
{
  std::cout << "split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_before_t, overlap_disposition_after_t)" << std::endl;
    // ix1       ix2
    //    ex1        ex2
    //    |----------|         ey1
    //    | dividend |
    // |--+------|---+         iy1
    // |  |xxxxxx|   |
    // |  |------+---|         ey2
    // |         |
    // | divisor |
    // |         |
    // |---------|             iy2

  auto ex1 = detail::rget_x1 (dividend)
    , ix2 = detail::rget_x2 (divisor)
    , ex2 = detail::rget_x2 (dividend)
    , ey1 = detail::rget_y1 (dividend)
    , iy1 = detail::rget_y1 (divisor)
    , ey2 = detail::rget_y2 (dividend);
  std::vector<Rectangle> r;
  assert (ey1 < iy1);
  assert (ix2 < ex2 && iy1 < ey2);
  r.push_back(Rectangle {{ex1, ex2}, {ey1, iy1}});
  r.push_back(Rectangle {{ix2, ex2}, {iy1, ey2}});
  return r;
}

template <typename Rectangle>
std::vector<Rectangle> split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_before_t, overlap_disposition_across_t)
{
  std::cout << "split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_before_t, overlap_disposition_across_t)" << std::endl;
    // ix1       ix2
    //    ex1              ex2
    //
    // |---------|             iy1
    // |         |
    // |  |------+---------|   ey1
    // |  |xxxxxx|dividend |
    // |  |xxxxxx|         |
    // |  |xxxxxx|         |
    // |  |------+---------|   ey2
    // |         |
    // | divisor |
    // |         |
    // |---------|             iy2

  auto ix2 = detail::rget_x2 (divisor)
    , ex2 = detail::rget_x2 (dividend)
    , ey1 = detail::rget_y1 (dividend)
    , ey2 = detail::rget_y2 (dividend);
  std::vector<Rectangle> r {Rectangle{{ix2, ex2}, {ey1, ey2}}};
  return r;
}
    
template <typename Rectangle>
std::vector<Rectangle> split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_middle_t, overlap_disposition_after_t)
{
  std::cout << "split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_middle_t, overlap_disposition_after_t)" << std::endl;
  //     ix1      ix2
  // ex1              ex2
  //
  // |---------+------|   ey1
  // |     dividend   |
  // |--|---------|---|   iy1
  // |  |xxxxxxxxx|   |
  // |--+---------+---|   ey2
  //    |         |
  //    | divisor |
  //    |         |
  //    |---------|       iy2

  auto ex1 = detail::rget_x1 (dividend)
    , ix1 = detail::rget_x1 (divisor)
    , ix2 = detail::rget_x2 (divisor)
    , ex2 = detail::rget_x2 (dividend)
    , ey1 = detail::rget_y1 (dividend)
    , iy1 = detail::rget_y1 (divisor)
    , ey2 = detail::rget_y2 (dividend);

  assert (ex1 < ix1 && ix2 < ex2 && ey1 < iy1 && iy1 < ey2);
  std::vector<Rectangle> r {{{ex1, ex2}, {ey1, iy1}}, {{ex1, ix1}, {iy1, ey2}}, {{ix2, ex2}, {iy1, ey2}}};
  return r;
}

template <typename Rectangle>
std::vector<Rectangle> split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_middle_t, overlap_disposition_across_t)
{
  std::cout << "split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_middle_t, overlap_disposition_across_t)" << std::endl;
  //    ix1       ix2
  // ex1                       ex2
  //
  //    |---------|                iy1
  //    |         |
  // |--+---------+------------|   ey1
  // |  |xxxxxxxxx| dividend   |
  // |  |xxxxxxxxx|            |
  // |  |xxxxxxxxx|            |
  // |--+---------+------------|   ey2
  //    |         |
  //    | divisor |
  //    |         |
  //    |---------|       iy2

  auto ex1 = detail::rget_x1 (dividend)
    , ix1 = detail::rget_x1 (divisor)
    , ix2 = detail::rget_x2 (divisor)
    , ex2 = detail::rget_x2 (dividend)
    , iy1 = detail::rget_y1 (divisor)
    , ey1 = detail::rget_y1 (dividend)
    , ey2 = detail::rget_y2 (dividend)
    , iy2 = detail::rget_y2 (divisor);
  static_cast<void>(iy1); static_cast<void>(iy2);
  assert (ex1 < ix1 && ix2 < ex2 && iy1 < ey1 && ey2 < iy2);
  return {{{ex1, ix1}, {ey1, ey2}}, {{ix2, ex2}, {ey1, ey2}}};
}
    
template <typename Rectangle>
std::vector<Rectangle> split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_across_t, overlap_disposition_after_t)
{
  std::cout << "split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_across_t, overlap_disposition_after_t)" << std::endl;
  //    ex1       ex2
  // ix1              ix2
  //
  //    |---------|       ey1
  //    |dividend |    
  // |--+---------+---|   iy1
  // |  |xxxxxxxxx|   |
  // |  |---------|   |   ey2
  // |                |
  // |    divisor     |
  // |                |
  // |----------------|   iy2

  auto ex1 = detail::rget_x1 (dividend)
    , ex2 = detail::rget_x2 (dividend)
    , ey1 = detail::rget_y1 (dividend)
    , iy1 = detail::rget_y1 (divisor);

  assert (ey1 < iy1);
  return {{{ex1, ex2}, {ey1, iy1}}};
}
    
template <typename Rectangle>
std::vector<Rectangle> split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_after_t, overlap_disposition_after_t)
{
  std::cout << "split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_after_t, overlap_disposition_after_t)" << std::endl;
  //           ix1     ix2
  //    ex1        ex2
  //    |----------|         ey1
  //    | divid|nd |
  //    |      |---+---|     iy1
  //    |      |xxx|   |
  //    |------+---|   |     ey2
  //           |       |
  //           |divisor|
  //           |       |
  //           |-------|     iy2
  auto ex1 = detail::rget_x1 (dividend)
    , ix1 = detail::rget_x1 (divisor)
    , ex2 = detail::rget_x2 (dividend)
    , ey1 = detail::rget_y1 (dividend)
    , iy1 = detail::rget_y1 (divisor)
    , ey2 = detail::rget_y2 (dividend);
  assert (ix1 < ex2 && ey1 < iy1);
  return {{{ex1, ix1}, {ey1, ey2}}, {{ix1, ex2}, {ey1, iy1}}};
}

template <typename Rectangle>
std::vector<Rectangle> split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_after_t, overlap_disposition_across_t)
{
  std::cout << "split_rectangle (Rectangle dividend, Rectangle divisor, overlap_disposition_after_t, overlap_disposition_across_t)" << std::endl;
  //           ix1     ix2
  // ex1           ex2
  //
  //           |-------|     iy1
  //           |       |
  // |---------+---|   |     ey1
  // | dividend|xxx|   |
  // |         |xxx|   |
  // |         |xxx|   |
  // |---------+---|   |     ey2
  //           |       |
  //           |divisor|
  //           |       |
  //           |-------|     iy2

  auto ex1 = detail::rget_x1 (dividend)
    , ix1 = detail::rget_x1 (divisor)
    , ey1 = detail::rget_y1 (dividend)
    , ey2 = detail::rget_y2 (dividend);
  assert (ex1 < ix1);
  return {{{ex1, ix1}, {ey1, ey2}}};
}
    
} }

#endif
