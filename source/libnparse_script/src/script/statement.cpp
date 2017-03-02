/*
 * @file $/source/libnparse_script/src/script/statement.cpp
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
#include <plugin/category.hpp>
#include <nparse/nparse.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include "../static.hpp"

namespace {

using namespace nparse;

class Construct: public IConstruct
{
	static const std::string sc_construct_ctg;
	plugin::category<IConstruct> m_constructs;
	plugin::instance<IConstruct> m_comment, m_expression;
	anta::ndl::Rule<SG> entry_, constructs_;

public:
	Construct ():
		m_constructs (sc_construct_ctg),
		m_comment ("nparse.script.Comment"),
		m_expression ("nparse.script.Expression"),
// <DEBUG_NODE_NAMING>
		entry_		("Statement.Entry"),
		constructs_	("Statement.Constructs")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;

		for (plugin::category<IConstruct>::const_iterator
				i = m_constructs. begin(); i != m_constructs. end(); ++ i)
		{
			constructs_. cluster()(
				i -> second -> entry(). cluster(),
				anta::Label<SG>(
					i -> first. substr(sc_construct_ctg. length() + 1))
			);
		}

		entry_ =
			m_comment -> entry()
		>	(	m_expression -> entry() > space > ';'
			|	constructs_
			);
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

const std::string Construct::sc_construct_ctg("nparse.script.constructs");

} // namespace

PLUGIN(Construct, script_statement, nparse.script.Statement)
