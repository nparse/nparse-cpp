/*
 * @file $/source/libnparse_script/src/script/_action_string.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef SRC_ACTION_STRING_HPP_
#define SRC_ACTION_STRING_HPP_

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <nparse/util/offset_state.hpp>
#include "../tokenizer.hpp"

namespace {

using namespace nparse;

// Represents an active string (supports substitutions).
class ActionString: public IAction
{
	// An auxiliary functor that determines current accepted range.
	template <typename Context_, typename Void_ = void>
	struct get_range {};

	// A specialization for the IEnvironment class.
	template <typename Void_>
	struct get_range<IEnvironment, Void_>
	{
		static nlg_string_t f (const IEnvironment& a_environment,
				const int a_offset = 0)
		{
			const anta::State<NLG>* s = offset_state<NLG>(
				& a_environment. get_traveller(). get_state(), a_offset);
			if (!s) throw flow_control(false);
			const anta::range<NLG>::type& r = s -> get_range();
			return nlg_string_t(r. first,  r. second);
		}

	};

	// A specialization for the Traveller class.
	template <typename Void_>
	struct get_range<anta::Traveller<NLG>, Void_>
	{
		static nlg_string_t f (const anta::Traveller<NLG>& a_traveller,
				const int a_offset = 0)
		{
			const anta::State<NLG>* s = offset_state<NLG>(
				& a_traveller. get_state(), a_offset);
			if (!s) throw flow_control(false);
			const anta::range<NLG>::type& r = s -> get_range();
			return nlg_string_t(r. first,  r. second);
		}

	};

	// An auxiliary template object that does variable substitutions.
	template <typename Context_>
	class formatter
	{
		const Context_& m_ctx;
		typedef typename Context_::key_type key_type;

	public:
		formatter (const Context_& a_ctx):
			m_ctx (a_ctx)
		{
		}

		template <typename Match_>
		nlg_string_t operator() (const Match_& a_match) const
		{
			static const nlg_string_t dollar("$");
			const nlg_string_t str = a_match[1]. str();
			if	(	str. empty()
				||	(str. size() == 2 && str[0] == L'{' && str[1] == L'}')
				)
			{
				// Substitute the last accepted range.
				return get_range<Context_>::f(m_ctx);
			}
			else
			if (str == dollar)
			{
				// Substitute the dollar sign.
				return dollar;
			}

			// The index can be optionally enclosed in curly brackets.
			const nlg_string_t index = (str[0] == L'{')
				?	nlg_string_t(str. begin()+1, str. begin()+(str. size()-1))
				:	nlg_string_t(str);

			// If the index is represented by an integer value, then it should
			// be treated as a placeholder offset, otherwise the index should be
			// treated and as a context key.

			for (nlg_string_t::const_iterator c = index. begin();
					c != index. end(); ++ c)
			{
				if (! std::isdigit(*c))
					return m_ctx. val(key_type(index)). as_string();
			}

			// Substitute an accepted range at the specified offset.
			return get_range<Context_>::f(
					m_ctx, boost::lexical_cast<int>(encode::unwrap(index)));
		}

	};

public:
	/**
	 *	Generic evaluation function for active string.
	 */
	template <typename Context_>
	string_t eval (const Context_& a_ctx) const
	{
		static const string_t def("\\$(\\$|\\w+|\\{\\w*\\})?");
		static const boost::basic_regex<string_t::value_type> pattern(def);
		return boost::regex_replace(m_string, pattern,
				formatter<Context_>(a_ctx));
	}

public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		return result_type(eval(a_env));
	}

public:
	/**
	 *	The only constructor.
	 */
	ActionString (const anta::range<SG>::type& a_range)
	{
		m_string = Tokenizer::decode(a_range. first, a_range. second);
	}

private:
	string_t m_string; /**< Stored string pattern. */

};

} // namespace

#endif /* SRC_ACTION_STRING_HPP_ */
