/*
 * @file $/include/nparse/exceptions.hpp
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
#ifndef NPARSE_EXCEPTIONS_HPP_
#define NPARSE_EXCEPTIONS_HPP_

namespace nparse { namespace ex {

#define NPARSE_CARRIER(name, type) \
	typedef boost::error_info<struct tag_##name, type> name;

/**
 *	A set of carriers of supplementary error information.
 *	@{ */
NPARSE_CARRIER(message, std::string)
NPARSE_CARRIER(location, anta::range<SG>::type)
NPARSE_CARRIER(file, std::string)
NPARSE_CARRIER(line, int)
NPARSE_CARRIER(offset, int)
NPARSE_CARRIER(function, std::string)
/**	@} */

/**
 *	Generic error. Represents the base class for all nParse exceptions.
 */
struct generic_error: boost::exception {};

/**
 *	Compile error. May occur when a grammar script is being imported.
 */
struct compile_error: generic_error {};

/**
 *	Syntax error. May occur when a grammar script is being parsed.
 */
struct syntax_error: compile_error {};

/**
 *	Runtime error. May occur when a grammar script is being executed. 
 */
struct runtime_error: generic_error {};

/**
 *	No L-Value. A special case of runtime error thrown on attempt to write to a
 *	read-only port.
 */
struct no_lvalue: public runtime_error {
	no_lvalue () {
		*this << message("no l-value: writing to a read-only port");
	}
};

/**
 *	No R-Value. A special case of runtime error thrown on attempt to read from a
 *	write-only port.
 */
struct no_rvalue: public runtime_error {
	no_rvalue () {
		*this << message("no r-value: reading from a write-only port");
	}
};

}} // namespace nparse::ex

#endif /* NPARSE_EXCEPTIONS_HPP_ */
