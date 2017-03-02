/*
 * @file $/include/anta/dsel.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef ANTA_DSEL_HPP_
#define ANTA_DSEL_HPP_

// [ standard library, boost ]
#include <math.h>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/lexical_cast.hpp>

// [ contributed ]
#include <encode/encode.hpp>

// [ local dependencies ]
#include "ndl.hpp"
#include "_aux/variable.hpp"

// [ local components: utility ]
#include "dsel/util/left_cast.hpp"
#include "dsel/util/right_cast.hpp"
#include "dsel/util/priority_cast.hpp"
#include "dsel/util/make_bool.hpp"
#include "dsel/util/make_key.hpp"
#include "dsel/util/comparable.hpp"

// [ local components: genric ]
#include "dsel/value.hpp"
#include "dsel/reference.hpp"
#include "dsel/placeholder.hpp"
#include "dsel/context.hpp"
#include "dsel/grammar.hpp"
#include "dsel/lambda.hpp"

// [ local components: run-time ]
#include "dsel/rt/terminal.hpp"
#if defined (ANTA_DSEL_RT)
#include "dsel/rt/comma.hpp"
#include "dsel/rt/assign.hpp"
#include "dsel/rt/subscript.hpp"
#include "dsel/rt/equal_to.hpp"
#include "dsel/rt/not_equal_to.hpp"
#include "dsel/rt/less.hpp"
#include "dsel/rt/greater.hpp"
#include "dsel/rt/less_equal.hpp"
#include "dsel/rt/greater_equal.hpp"
#include "dsel/rt/logical_and.hpp"
#include "dsel/rt/logical_or.hpp"
#include "dsel/rt/logical_not.hpp"
#include "dsel/rt/plus.hpp"
#include "dsel/rt/minus.hpp"
#include "dsel/rt/multiplies.hpp"
#include "dsel/rt/divides.hpp"
#include "dsel/rt/modulus.hpp"
#include "dsel/rt/negate.hpp"
#include "dsel/rt/pre_inc.hpp"
#include "dsel/rt/pre_dec.hpp"
#include "dsel/rt/post_inc.hpp"
#include "dsel/rt/post_dec.hpp"
#endif

#endif /* ANTA_DSEL_HPP_ */
