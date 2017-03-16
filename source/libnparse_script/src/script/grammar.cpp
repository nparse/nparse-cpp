/*
 * @file $/source/libnparse_script/src/script/grammar.cpp
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
#include <cctype>
#include <plugin/category.hpp>
#include <nparse/nparse.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/end.hpp>
#include "../static.hpp"

namespace {

using namespace nparse;

class Construct: public IConstruct
{
	static const std::string sc_declaration_ctg;
	plugin::category<IConstruct> m_decls;
	plugin::instance<IConstruct> m_comment;

	anta::ndl::Rule<SG> entry_, decls_;

public:
	Construct ():
		m_decls (sc_declaration_ctg),
		m_comment ("nparse.script.Comment")
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;

		// Link a local node to the entry node of each of the rule constructs.
		for (plugin::category<IConstruct>::const_iterator i = m_decls. begin();
				i != m_decls. end(); ++ i)
		{
			decls_. cluster()(
				// Get the entry node of the rule.
				i -> second -> entry(). cluster(),
				// Label the new arc with the name of the linked rule.
				anta::Label<SG>(
					i -> first. substr(sc_declaration_ctg. length() + 1))
			);
		}

		// Define the top rule of the grammar.
		entry_ = m_comment -> entry() % (decls_ > space > ';') > space > end;
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

const std::string Construct::sc_declaration_ctg("nparse.script.decls");

} // namespace

PLUGIN(Construct, script_grammar, nparse.script.Grammar)
