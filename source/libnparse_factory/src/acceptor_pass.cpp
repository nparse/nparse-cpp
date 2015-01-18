/*
 * @file $/source/libnparse_factory/src/acceptor_pass.cpp
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
#include <encode/encode.hpp>
#include <nparse/nparse.hpp>
#include "static.hpp"
#include "_priority.hpp"

namespace {

using namespace nparse;

class Acceptor: public IAcceptor
{
public:
	Acceptor ()
	{
	}

public:
	// Overridden from IAcceptor:
	operator const anta::Acceptor<NLG>& () const
	{
		return anta::unconditional<NLG>();
	}

};

class Generator: public IAcceptorGenerator
{
public:
	// Overridden from IPrioritized:
	int priority () const
	{
		return PRIORITY_PASS;
	}

	// Overridden from IAcceptorGenerator:
	bool generate (const string_t& a_definition, const IAcceptor*& a_instance,
			const std::string&/* a_path*/)
	{
		if (a_definition. empty())
		{
			a_instance = new Acceptor();
			return true;
		}
		return false;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Generator, acceptor_pass, nparse.acceptors.Pass, 1 )