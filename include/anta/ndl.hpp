/*
 * @file $/include/anta/ndl.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.8

The MIT License (MIT)
Copyright (c) 2007-2017 Alex Kudinov <alex.s.kudinov@gmail.com>

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
#ifndef ANTA_NDL_HPP_
#define ANTA_NDL_HPP_

// [ standard library, boost ]
#include <list>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <sstream>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>
#include <boost/proto/proto.hpp>

// [ contributed ]
#include <util/callback.hpp>

// [ local dependencies ]
#include "core.hpp"

// [ local components ]
#include "ndl/model.hpp"
#include "ndl/context.hpp"
#include "ndl/actions.hpp"
#include "ndl/selectors.hpp"
#include "ndl/node.hpp"
#include "ndl/generator.hpp"
#include "ndl/cluster.hpp"
#include "ndl/prototypes.hpp"
#include "ndl/rule.hpp"

// [ local components: old style grammars ]
#if defined(ANTA_NDL_OLD_STYLE)
#include "ndl.1/terminals.hpp"
#include "ndl.1/nonterminals.hpp"
#include "ndl.1/grammar.hpp"
#include "ndl.1/network.hpp"
#endif

#endif /* ANTA_NDL_HPP_ */
