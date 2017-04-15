/*
 * @file $/include/anta/ndl.1/nonterminals.hpp
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
#ifndef ANTA_NDL_NONTERMINALS_HPP_
#define ANTA_NDL_NONTERMINALS_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	Nonterminal alphabet.
 */
template <typename M_, typename base_type = std::map<
	typename node_name<M_>::type, Cluster<M_>*> >
class Nonterminals: public base_type
{
public:
	typedef typename base_type::key_type key_type;
	typedef typename base_type::mapped_type mapped_type;
	typedef typename base_type::value_type value_type;
	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;

	/**
	 *	The only constructor.
	 */
	Nonterminals ()
	{
	}

	/**
	 *	The destructor.
	 */
	~Nonterminals ()
	{
		utility::free2nds(*this);
	}

	/**
	 *	Define a grammar rule.
	 *	(Returns a mutable reference to a named cluster).
	 */
	Cluster<M_>& define (const key_type& a_name)
	{
		iterator found_at = this -> find(a_name);
		if (found_at == this -> end())
		{
			const std::pair<iterator, bool> p = this -> insert(value_type(
						a_name, new Cluster<M_>(a_name)));
			assert(p. second);
			found_at = p. first;
		}
		return *(found_at -> second);
	}

	/**
	 *	Put a nonterminal metasymbol which is referenced using the name of a
	 *	cluster of the network.
	 */
	JointCall<M_> put (const key_type& a_name)
	{
		return JointCall<M_>(define(a_name));
	}

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_NONTERMINALS_HPP_ */
