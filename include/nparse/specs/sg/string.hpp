/*
 * @file $/include/nparse/specs/sg/string.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef NPARSE_SPECS_SG_STRING_HPP_
#define NPARSE_SPECS_SG_STRING_HPP_

/**
 *	The anta::string<...> template specializations both for NLG and SG models
 *	make narrow and wide string values mutually convertible without using of any
 *	expicit calls or special syntax.
 */
namespace anta {

template <>
struct string<nparse::SG>
{
	typedef encode::wrapper<std::basic_string<character<nparse::SG>::type> >
		type;

};

} // namespace anta

/**
 *	The model-specific string types.
 */
namespace nparse {

typedef anta::string<SG>::type sg_string_t, string_t;

} // namespace nparse

#endif /* NPARSE_SPECS_SG_STRING_HPP_ */
