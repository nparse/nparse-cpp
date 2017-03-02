/*
 * @file $/source/libnparse_factory/src/acceptor_end.cpp
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
#include <nparse/nparse.hpp>
#include <anta/sas/end.hpp>
#include "static.hpp"
#include "_priority.hpp"

namespace {

using namespace nparse;

class Acceptor: public IAcceptor
{
	anta::sas::End<NLG> m_impl;

public:
	Acceptor ()
	{
	}

public:
	// Overridden from IAcceptor:
	const anta::Acceptor<NLG>& get () const
	{
		return m_impl;
	}

};

class AcceptorFactory: public IAcceptorFactory
{
public:
	// Overridden from IPrioritized:
	int priority () const
	{
		return PRIORITY_END;
	}

	// Overridden from IAcceptorFactory:
	bool create (const string_t& a_definition, const IAcceptor*& a_instance,
			const std::string&/* a_path*/)
	{
		static const string_t sc_def("^$");
		if (a_definition == sc_def)
		{
			a_instance = new Acceptor();
			return true;
		}
		return false;
	}

};

} // namespace

PLUGIN(AcceptorFactory, acceptor_end, nparse.acceptors.End)
