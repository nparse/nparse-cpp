/*
 * @file $/include/anta/dsel/grammar.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex@nparse.com>
 
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
#ifndef ANTA_DSEL_GRAMMAR_HPP_
#define ANTA_DSEL_GRAMMAR_HPP_

namespace anta { namespace dsel {

/******************************************************************************/

namespace proto = boost::proto;

/**
 *
 */
template <typename M_>
struct GrammarCases
{
	template <typename Tag_, typename Void_ = void>
	struct case_: proto::not_<proto::_> {};

};

/**
 *
 */
template <typename M_>
struct Grammar: proto::switch_<GrammarCases<M_> > {};

/******************************************************************************/

}} // namespace anta::dsel

#endif /* ANTA_DSEL_GRAMMAR_HPP_ */