/*
 * @file $/source/libnparse_factory/src/acceptor_class.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@nparse.com>
 
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
#include <algorithm>
#define BOOST_SPIRIT_UNICODE
#include <boost/spirit/include/qi.hpp>

#include <nparse/nparse.hpp>
#include <utility/range_add.hpp>
#include "static.hpp"
#include "_priority.hpp"

namespace {

using namespace nparse;
namespace qi = boost::spirit::qi;
namespace ce = boost::spirit::unicode;

// (forward declaration)
class Acceptor;

/**
 *	An auxiliary grammar that parses complex character class definitions.
 */
template <typename I_ = string_t::const_iterator>
class ParserAdapter: public qi::grammar<I_>
{
	typedef qi::unused_type unused;

	struct functor
	{
		Acceptor* target;

	};

	struct functor_char: public functor
	{
		template <typename Attr_>
		void operator() (const Attr_& attr, unused, unused) const
		{
			functor::target -> add_char(attr);
		}

	};

	struct functor_class: public functor
	{
		template <typename Attr_>
		void operator() (const Attr_& attr, unused, unused) const
		{
			functor::target -> add_class(string_t(attr. begin(), attr. end()));
		}

	};

	struct functor_range: public functor
	{
		template <typename Attr_>
		void operator() (const Attr_& attr, unused, unused) const
		{
			using boost::fusion::at_c;
			functor::target -> add_range(at_c<0>(attr), at_c<1>(attr));
		}

	};

	struct functor_length: public functor
	{
		template <typename Attr_>
		void operator() (const Attr_& attr, unused, unused) const
		{
			functor::target -> add_length(attr, attr);
		}

		void operator() (unused, unused, unused) const
		{
			functor::target -> add_length(1, 1);
		}

	};

	struct functor_interval: public functor
	{
		template <typename Attr_>
		void operator() (const Attr_& attr, unused, unused) const
		{
			using boost::fusion::at_c;
			functor::target -> add_length(at_c<0>(attr), at_c<1>(attr));
		}

	};

	functor_char add_char;
	functor_class add_class;
	functor_range add_range;
	functor_length add_length;
	functor_interval add_interval;

	qi::rule<I_> m_entry;

public:
	ParserAdapter (Acceptor* a_acceptor):
		ParserAdapter::base_type (m_entry)
	{
		add_char. target = a_acceptor;
		add_class. target = a_acceptor;
		add_range. target = a_acceptor;
		add_length. target = a_acceptor;
		add_interval. target = a_acceptor;

		using ce::char_;
		using qi::uint_;
		using qi::eoi;

		m_entry %=
			'['
		>  +(	('\\' > char_[ add_char ])
			|	("[:" > (+char_("a-zA-Z0-9"))[ add_class ] > ":]")
			|	(((char_ - '-') >> '-') > char_)[ add_range ]
			|	(char_ - ']')[ add_char ]
			)
		>	']'
		>	(	eoi[ add_length ]
			|	'{'
			>	(	((uint_ >> '-') > uint_)[ add_interval ]
				|	uint_[ add_length ]
				)
				% ','
			>	'}'
			);
	}

};

/**
 *	Acceptor implementation.
 */
class Acceptor: public anta::Acceptor<NLG>, public IAcceptor
{
	string_t m_set;

	typedef std::vector<std::size_t> bounds_t;
	bounds_t m_bounds;

	typedef std::vector<int (*)(int)> std_classes_t;
	std_classes_t m_std_classes;

	bool test (const anta::character<NLG>::type& a_char) const
	{
		// @todo: use custom hashmap instead of binary search

		const int c = static_cast<int>(a_char);
		for (std_classes_t::const_iterator i = m_std_classes. begin();
				i != m_std_classes. end(); ++ i)
		{
			if ((*i)(c)) return true;
		}

		return std::binary_search(m_set. begin(), m_set. end(), a_char);
	}

public:
	// Overridden from anta::Acceptor<NLG>:
	void accept (const anta::range<NLG>::type& C,
			const anta::range<NLG>::type& E, anta::spectrum<NLG>::type& S) const
	{
		assert(! m_bounds. empty());

		anta::iterator<NLG>::type i = E. second;
		anta::iterator<NLG>::type i_max = i + std::min<std::size_t>(
				C. second - E. second, m_bounds. back() - 1
		);
		bounds_t::const_iterator b = m_bounds. begin();
		while (true)
		{
			if (static_cast<std::size_t>(i - E. second) == *b)
			{
				++ b;
				assert(b != m_bounds. end());
			}
			if ((b - m_bounds. begin()) & 1)
			{
				S. push(E. second, i);
			}
			if (i == i_max || !test(*i))
			{
				break;
			}
			++ i;
		}
	}

	// Overridden from IAcceptor:
	const anta::Acceptor<NLG>& get () const
	{
		return *this;
	}

public:
	void add_char (const string_t::value_type a_char)
	{
		string_t::iterator at = std::lower_bound(m_set. begin(), m_set. end(),
				a_char);
		if (at == m_set. end() || *at != a_char)
			m_set. insert(at, a_char);
	}

	void add_class (const string_t& a_class)
	{
		static const struct {
			string_t name;
			int (*func)(int);
		} sc_std[] = {
#define DECL(x) { #x, std::is##x },
			DECL(alnum)
			DECL(alpha)
			DECL(cntrl)
			DECL(digit)
			DECL(graph)
			DECL(lower)
			DECL(print)
			DECL(punct)
			DECL(space)
			DECL(upper)
			DECL(xdigit)
#undef DECL
			{ "", NULL }
		};

		for (int i = 0; sc_std[i]. func != NULL; ++ i)
		{
			if (sc_std[i]. name == a_class)
			{
				m_std_classes. push_back(sc_std[i]. func);
				return ;
			}
		}
	}

	void add_range (const string_t::value_type a_from,
			const string_t::value_type a_to)
	{
		for (string_t::value_type c = a_from; c <= a_to; ++ c)
			add_char(c);
	}

	void add_length (const std::size_t a_from, const std::size_t a_to)
	{
		utility::range_add(m_bounds, a_from, a_to + 1);
	}

};

/**
 *	AcceptorFactory implementation.
 */
class AcceptorFactory: public IAcceptorFactory
{
public:
	// Overridden from IPrioritized:
	int priority () const
	{
		return PRIORITY_CLASS;
	}

	// Overridden from IAcceptorFactory:
	bool create (const string_t& a_definition, const IAcceptor*& a_instance,
			const std::string&/* a_path*/)
	{
		string_t::const_iterator first = a_definition. begin();
		string_t::const_iterator last = a_definition. end();

		Acceptor* instance = new Acceptor();
		ParserAdapter<> pa(instance);

		try
		{
			if (qi::parse(first, last, pa) && first == last)
			{
				a_instance = instance;
				return true;
			}
		}
		catch (const qi::expectation_failure<string_t::const_iterator>& err)
		{
			// @todo: warn about a suspicious definition
		}

		delete instance;
		return false;
	}

};

} // namespace

PLUGIN(AcceptorFactory, acceptor_class, nparse.acceptors.Class)
