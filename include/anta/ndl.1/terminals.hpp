/*
 * @file $/include/anta/ndl.1/terminals.hpp
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
#ifndef ANTA_NDL_TERMINALS_HPP_
#define ANTA_NDL_TERMINALS_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	The default type for the terminal key.
 */
template <typename M_>
struct terminal_key
{
	typedef typename string<M_>::type type;

};

/**
 *	Terminal alphabet.
 */
template <typename M_, typename base_type = std::map<
	typename terminal_key<M_>::type, Acceptor<M_>*> >
class Terminals: public base_type
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
	Terminals ()
	{
	}

	/**
	 *	The destructor.
	 */
	~Terminals ()
	{
		utility::free2nds(*this);
	}

	/**
	 *	Put a terminal metasymbol which is given by an acceptor.
	 */
	template <typename Acceptor_>
	JointJump<M_> put (const Acceptor_& a_acceptor,
			const key_type& a_def = key_type())
	{
		if (a_def. empty())
		{
			return JointJump<M_>(a_acceptor, Label<M_>(true));
		}
		else
		{
			iterator found_at = this -> find(a_def);
			if (found_at == this -> end())
			{
				const std::pair<iterator, bool> p = this -> insert(value_type(
							a_def, new Acceptor_(a_acceptor)));
				assert(p. second);
				found_at = p. first;
			}
			return JointJump<M_>(*(found_at -> second), Label<M_>(true));
		}
	}

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_TERMINALS_HPP_ */
