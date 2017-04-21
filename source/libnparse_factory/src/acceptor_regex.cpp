/*
 * @file $/source/libnparse_factory/src/acceptor_regex.cpp
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
#include <encode/encode.hpp>
#include <nparse/nparse.hpp>
#include <anta/sas/regex.hpp>
#include "_priority.hpp"
#include "static.hpp"

namespace {

using namespace nparse;

namespace rx = boost;
typedef rx::basic_regex<anta::character<NLG>::type> regex_type;
typedef rx::match_results<anta::iterator<NLG>::type> match_results_type;
typedef rx::sub_match<anta::iterator<NLG>::type> sub_match_type;

class Acceptor: public IAcceptor, public anta::Acceptor<NLG>
{
public:
	Acceptor (const string_t& a_pattern):
		m_regex (a_pattern, boost::regex_constants::save_subexpression_location)
	{
		const regex_type::size_type
#if BOOST_VERSION >= 105600
		i0 = 0, di = 1;
#else
		i0 = 1, di = 0;
#endif
		for (regex_type::size_type i = i0; i < m_regex. mark_count(); ++ i)
		{
			// NOTE: According to boost::regex documentation, marked groups have
			//		 the following syntax
			//		 (?<name>...)  OR  (?'name'...)
			//		 The iterator range returned by regex::subexpression(i) does
			//		 not include the closing parethesis, therefore the shortest
			//		 possible marked group definition '(?<x>' must contain at
			//		 least five characters.
			std::pair<
				regex_type::const_iterator,
				regex_type::const_iterator> p = m_regex.subexpression(i);
			if (std::distance(p. first, p. second) < 5)
			{
				continue;
			}
			std::advance(p. first, 3);
			switch (*(p. first - 1))
			{
			case L'<':
				p. second = std::find(p. first, p. second, L'>');
				break;
			case L'\'':
				p. second = std::find(p. first, p. second, *(p. first - 1));
				break;
			default:
				continue;
			}
			const IEnvironment::key_type key(string_t(p. first, p. second));
			m_named_groups. push_back(named_groups_t::value_type(key, i + di));
		}
	}

public:
	// Overridden from IAcceptor:
	const anta::Acceptor<NLG>& get () const
	{
		return *this;
	}

	// Overridden from Acceptor<NLG>:
	void accept (const anta::range<NLG>::type& C,
			const anta::range<NLG>::type& E, anta::spectrum<NLG>::type& S) const
	{
		match_results_type m;
		if (! rx::regex_search(E. second, C. second, m, m_regex,
					rx::match_continuous))
		{
			return;
		}
		else if (m. empty())
		{
			S. push(m[0]. first, m[0]. second);
		}
		else
		{
			anta::State<NLG>* state = S. spawn(m[0]. first, m[0]. second);
			if (state != NULL)
			{
				for (named_groups_t::const_iterator g = m_named_groups. begin();
						g != m_named_groups. end(); ++ g)
				{
					const sub_match_type& sub = m[g -> second];
					state -> ref(g -> first, S, true) = IEnvironment::value_type(
							sub. first, sub. second);
				}
				S. push(state);
			}
		}
	}

private:
	regex_type m_regex;

	typedef std::vector<std::pair<IEnvironment::key_type, regex_type::size_type
		> > named_groups_t;
	named_groups_t m_named_groups;

};

class AcceptorFactory: public IAcceptorFactory
{
public:
	// Overridden from IPrioritized:
	int priority () const
	{
		return PRIORITY_REGEX;
	}

	// Overridden from IAcceptorFactory:
	bool create (const string_t& a_definition, const IAcceptor*& a_instance,
			const std::string&/* a_path*/)
	{
		if (a_definition. size() > 1 && *a_definition. begin() == '^')
		{
			a_instance = new Acceptor(a_definition);
			return true;
		}
		else if (a_definition. size() > 3 && *a_definition. begin() == '/' &&
				*a_definition. rbegin() == '/')
		{
			a_instance = new Acceptor(
					a_definition. substr(1, a_definition. size() - 2));
			return true;
		}
		else
		{
			return false;
		}
	}

};

} // namespace

PLUGIN(AcceptorFactory, acceptor_regex, nparse.acceptors.RegEx)
