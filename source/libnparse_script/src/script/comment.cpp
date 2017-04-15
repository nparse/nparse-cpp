/*
 * @file $/source/libnparse_script/src/script/comment.cpp
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
#define BOOST_SPIRIT_UNICODE
#include <boost/spirit/include/qi.hpp>
#include <nparse/nparse.hpp>
#include "../static.hpp"

// NOTE: This is a nice example of how developers can integrate ANTA with other
//		 parsers, boost::spirit in particular.

namespace qi = boost::spirit::qi;
namespace ce = boost::spirit::unicode;

template <typename M_>
class Comment: public anta::Acceptor<M_>
{
	typedef typename anta::iterator<M_>::type iterator;
	typedef typename anta::range<M_>::type range;
	qi::rule<iterator> m_comment;

public:
	Comment ()
	{
		using ce::space;
		using ce::char_;
		using qi::eol;
		m_comment %=
		   *(	space
			|	("//" > *(char_ - eol) > eol)
			|	("/*" > *(char_ - "*/") > "*/")
			);
	}

public:
	// Overridden from Acceptor<M_>:

	void accept (const range& C, const range& E,
			typename anta::spectrum<M_>::type& S) const
	{
		iterator i = E. second;
		if (qi::parse(i, C. second, m_comment))
		{
			S. push(E. second, i);
		}
	}

};

namespace anta { namespace ndl { namespace detail {

template <typename M_>
struct eval_terminal<M_, ::Comment<M_> >: eval_acceptor<M_> {};

}}} // namespace anta::ndl::detail

namespace {

using namespace nparse;

class Construct: public IConstruct
{
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
// <DEBUG_NODE_NAMING>
		entry_		("Comment.Entry")
// </DEBUG_NODE_NAMING>
	{
		using boost::proto::lit;
		entry_ = lit(Comment<SG>());
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, script_comment, nparse.script.Comment)
