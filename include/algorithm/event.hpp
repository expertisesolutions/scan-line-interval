///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Felipe Magno de Almeida.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ALGORITHM_EVENT_HPP
#define ALGORITHM_EVENT_HPP

namespace exp { namespace algorithm {

enum class event_type
{
  start, end
};

template <typename I>
struct event
{
  event_type type;
  I interval;
};


    
} }

#endif
