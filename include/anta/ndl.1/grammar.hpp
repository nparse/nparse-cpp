/*
 * @file $/include/anta/ndl.1/grammar.hpp
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
#ifndef ANTA_NDL_GRAMMAR_HPP_
#define ANTA_NDL_GRAMMAR_HPP_

namespace anta { namespace ndl {

/******************************************************************************/

/**
 *	The grammar.
 */
template <typename M_>
class Grammar: public Base<Grammar<M_>, M_>
{
public:
	/**
	 *	The only constructor.
	 */
	Grammar ()
	{
	}

public:
	/**
	 *	Get a constant reference to the object that represents the terminal
	 *	alphabet.
	 */
	const Terminals<M_>& get_terminals () const
	{
		return m_terminals;
	}

	/**
	 *	Get a constant reference to the object that represents the nonterminal
	 *	alphabet.
	 */
	const Nonterminals<M_>& get_nonterminals () const
	{
		return m_nonterminals;
	}

	/**
	 *	Define a grammar rule.
	 */
	Cluster<M_>& rule (const typename Nonterminals<M_>::key_type& a_name)
	{
		return m_nonterminals. define(a_name);
	}

	/**
	 *	Put a nonterminal metasymbol.
	 */
	JointCall<M_> nonterm (const typename Nonterminals<M_>::key_type& a_name)
	{
		return m_nonterminals. put(a_name);
	}

	/**
	 *	Put a terminal metasymbol.
	 */
	template <typename Acceptor_>
	JointJump<M_> term (const Acceptor_& a_acceptor,
			const typename Terminals<M_>::key_type& a_def =
				typename Terminals<M_>::key_type())
	{
		return m_terminals. put(a_acceptor, a_def);
	}

	/**
	 *	Put an unconditional terminal metasymbol (pass).
	 */
	JointJump<M_> pass ()
	{
		static const UnconditionalRepeat<M_> sc_unc = UnconditionalRepeat<M_>();
		return m_terminals. put(sc_unc);
	}

	/**
	 *	Put a nonterminal call to an external node.
	 */
	static JointCall<M_> call (const ::anta::Node<M_>& a_node)
	{
		return JointCall<M_>(a_node);
	}

private:
	Terminals<M_> m_terminals;
	Nonterminals<M_> m_nonterminals;

};

/******************************************************************************/

}} // namespace anta::ndl

#endif /* ANTA_NDL_GRAMMAR_HPP_ */
