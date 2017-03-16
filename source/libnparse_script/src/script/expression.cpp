/*
 * @file $/source/libnparse_script/src/script/expression.cpp
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
#include <assert.h>
#include <sstream>
#include <algorithm>
#include <boost/ptr_container/ptr_vector.hpp>
#include <plugin/category.hpp>
#include <nparse/nparse.hpp>
#include "../static.hpp"

namespace {

using namespace nparse;

class Construct: public IConstruct
{
	static const std::string sc_operator_ctg;
	typedef plugin::category<IOperator> operators_t;
	operators_t m_operators;
	boost::ptr_vector<anta::ndl::Rule<SG> > levels_;

public:
	Construct ():
		m_operators (sc_operator_ctg)
	{
	}

	void initialize ()
	{
		// Sort operator category in priority ascending order.
		std::sort(m_operators. begin(), m_operators. end(), less_priority());

		// Instantiate the set of rules representing expression levels.
		levels_. resize(m_operators. size());

		// Initialize expression levels by deploying operators.
		std::stringstream tmp;
		for (operators_t::const_iterator i = m_operators. begin();
				i != m_operators. end(); ++ i)
		{
			const int index = static_cast<int>(i - m_operators. begin());
// <DEBUG_NODE_NAMING>
			tmp << "Expression.Level(" << index << ", "
				<< i -> first. substr(sc_operator_ctg. length() + 1) << ')';
			levels_[index]. cluster(). set_name(tmp. str());
			tmp. str("");
			tmp. clear();
// </DEBUG_NODE_NAMING>
			i -> second -> deploy(
				levels_[index],
				index ? levels_[index - 1] : levels_[index],
				levels_. back()
			);
		}
	}

	const anta::ndl::Rule<SG>& entry (const int a_level) const
	{
		// The top level (which is the comma operator) is considered to be the
		// entry rule.
		return *(levels_. begin() + (levels_. size() + a_level - 1));
	}

};

const std::string Construct::sc_operator_ctg("nparse.script.operators");

} // namespace

PLUGIN(Construct, script_expression, nparse.script.Expression)
