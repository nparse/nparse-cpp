/*
 * @file $/include/nparse-port/parser.hpp
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
#ifndef NPARSE_CPP_PARSER_HPP_
#define NPARSE_CPP_PARSER_HPP_

#include "variable.hpp"

namespace nparse {

// (forward declarations)
struct ParserData;

/**
 *	The main parser object.
 */
class Parser
{
public:
	enum status_t
	{
		stReady = 0,
		stSteady,
		stRunning,
		stCompleted,
		stLogicError,
		stSyntaxError,
		stSyntaxAmbiguity,
		stRuntimeError
	};

public:
	/**
	 *	The only constructor.
	 */
	Parser ();

	/**
	 *	The destructor.
	 */
	~Parser ();

public:
	/**
	 *	Get version information.
	 */
	const char* version () const;

	/**
	 *	Get current parser status.
	 *	@{ */
	status_t status () const;
	const char* status_str () const;
	/**	@} */

	/**
	 *	Load and compile grammar given as a local file, a string, or both.
	 */
	void load (const char* a_filename, const char* a_grammar = NULL);

	/**
	 *	These functions are used to operate parser and navigate traces.
	 *	@{ */
	bool parse (const wchar_t* a_input, const int a_len = 0);
	bool next ();
	bool step ();
	void rewind ();
	void reset ();
	/**	@} */

	/**
	 *	These functions are used to examine individual trace elements (also
	 *	known as analysis states).
	 *	@{ */
	Variable get (const char* a_variable) const;
	int label () const;
	char* node (char* a_buf, const int a_len) const;
	char* text (char* a_buf, const int a_len) const;
	int shift () const;
	/**	@} */

	/**
	 *	These functions are used to set initial values to trace variables.
	 *	@{ */
	void set (const char* a_variable, const bool a_value);
	void set (const char* a_variable, const int a_value);
	void set (const char* a_variable, const double a_value);
	void set (const char* a_variable, const char* a_value);
	void clear ();
	/**	@} */

	/**
	 *	These functions are used to retrieve detailed information on the
	 *	internal parser state.
	 *	@{ */
	int get_message_count () const;
	const char* get_message (const int a_index) const;
	const char* get_location (const int a_index, int* a_line = NULL,
			int* a_offset = NULL) const;
	/**	@} */

	/**
	 *	These functions are used to retrieve auxiliary (satistical) information.
	 *	@{ */
	unsigned long get_iteration_count () const;
	unsigned long get_trace_count () const;
	unsigned long get_pool_usage () const;
	unsigned long get_pool_capacity () const;
	/**	@} */

private:
	ParserData* m_;

};

} // namespace nparse

#endif /* NPARSE_CPP_PARSER_HPP_ */
