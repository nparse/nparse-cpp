/*
 * @file $/include/nparse-port/variable.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef NPARSE_CPP_VARIABLE_HPP_
#define NPARSE_CPP_VARIABLE_HPP_

#include <iterator>

// recommended buffer size for string data
#define NPARSE_STRBUF_SIZE 4096

namespace nparse {

// (forward declarations)
class Variable;
class VariableIterator;
struct VariableData;
struct VariableIteratorData;

/**
 *	An object that represents trace variables.
 */
class Variable
{
public:
	enum type_t
	{
		Null = 0,
		Boolean,
		Integer,
		Real,
		String,
		Array
	};

public:
	Variable (VariableData* a_ = NULL);
	~Variable ();

	Variable (const Variable& a_inst);
	const Variable& operator= (const Variable& a_inst);
	void swap (Variable& a_inst);

	type_t type () const;
	const char* tag () const;
	const char* key () const;
	std::size_t id () const;

	bool	get_boolean (const bool a_def = false) const;
	int		get_integer (const int a_def = 0) const;
	double	get_real (const double a_def = 0.0) const;
	char*	get_string (char* a_buf, const int a_len,
				const char* a_def = "") const;

	bool	as_boolean () const;
	int		as_integer () const;
	double	as_real () const;
	char*	as_string (char* a_buf, const int a_len) const;

	typedef VariableIterator const_iterator;

	VariableIterator begin () const;
	VariableIterator end () const;
	int size () const;

	VariableIterator get (const int a_key) const;
	VariableIterator get (const char* a_key) const;

private:
	VariableData* m_;

};

/**
 *	An iterator object that is used to run through associative arrays stored in
 *	trace variables.
 */
class VariableIterator: public std::iterator<std::forward_iterator_tag,
	Variable>
{
public:
	VariableIterator (VariableIteratorData* a_data = NULL);
	~VariableIterator ();

	VariableIterator (const VariableIterator& a_iter);
	const VariableIterator& operator= (const VariableIterator& a_iter);

	bool operator== (const VariableIterator& a_iter) const;

	bool operator!= (const VariableIterator& a_iter) const
	{
		return !(*this == a_iter);
	}

	const Variable& operator* () const;

	const Variable* operator-> () const
	{
		return &**this;
	}

	const VariableIterator& operator++ ();

	VariableIterator operator++ (int)
	{
		VariableIterator temp(*this);
		++(*this);
		return temp;
	}

	void seek (const int a_key);
	void seek (const char* a_key);

	bool end () const
	{
		return m_ == NULL;
	}

private:
	VariableIteratorData* m_;

};

} // namespace nparse

#endif /* NPARSE_CPP_VARIABLE_HPP_ */
