/*
 * @file $/include/nparse/functions.hpp
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
#ifndef NPARSE_FUNCTIONS_HPP_
#define NPARSE_FUNCTIONS_HPP_

namespace nparse {

/**
 *	A helper function for getting the name of the target node.
 */
inline const anta::ndl::node_name<SG>::type& get_node_name (
		const hnd_arg_t& arg)
{
	return arg. state. get_arc(). get_target(). get_name();
}

/**
 *	A helper function for getting the string value of the label of the arc.
 */
inline const std::string& get_label_str (const hnd_arg_t& arg)
{
	return arg. state. get_arc(). get_label(). get();
}

/**
 *	A helper function for getting the integer value of the label of the arc.
 */
inline int get_label_int (const hnd_arg_t& arg)
{
	return arg. state. get_arc(). get_label(). get_int();
}

/**
 *	A helper function for getting the accepted range.
 */
inline const anta::range<SG>::type& get_accepted_range (const hnd_arg_t& arg)
{
	return arg. state. get_range();
}

/**
 *	A helper function for getting the accepted range as a string.
 */
inline string_t get_accepted_str (const hnd_arg_t& arg)
{
	const anta::range<SG>::type& range = get_accepted_range(arg);
	return string_t(range. first, range. second);
}


/**
 *	Auxiliary ports for the useful placeholder generator functions.
 *	@{ */
inline anta::dsel::placeholder<SG>::type push (
		const anta::dsel::reference<SG>::type& a_ref)
{
	return anta::dsel::push<SG>(a_ref);
}

inline anta::dsel::placeholder<SG>::type pop (
		const anta::dsel::reference<SG>::type& a_ref)
{
	return anta::dsel::pop<SG>(a_ref);
}

inline anta::dsel::placeholder<SG>::type delta ()
{
	return anta::dsel::delta<SG>();
}
/**	@} */

/**
 *	An auxiliary functor that is used to sort operator category in priority
 *	ascending order.
 */
struct less_priority
{
	typedef std::pair<std::string, IPrioritized*> entry_t;

	bool operator() (const entry_t& u, const entry_t& v) const
	{
		return u. second -> priority() < v. second -> priority();
	}

};

} // namespace nparse

#endif /* NPARSE_FUNCTIONS_HPP_ */
