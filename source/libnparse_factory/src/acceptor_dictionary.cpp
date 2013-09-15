/*
 * @file $/source/libnparse_factory/src/acceptor_dictionary.cpp
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
#include <nparse/nparse.hpp>
#include "dictionary/acceptor_red.hpp"
#if defined(NPARSE_CMPH)
#include "dictionary/acceptor_phf.hpp"
#endif
#include "static.hpp"
#include "_priority.hpp"

namespace {

using namespace nparse;

template <typename Implementation_>
class Acceptor: public IAcceptor
{
	Implementation_ m_impl;

public:
	Acceptor (const std::string& a_filename):
		m_impl (a_filename)
	{
	}

public:
	// Overridden from IAcceptor:
	operator const anta::Acceptor<NLG>& () const
	{
		return m_impl;
	}

};

class Generator: public IAcceptorGenerator
{
public:
	// Overridden from IPrioritized:
	int priority () const
	{
		return PRIORITY_DICTIONARY;
	}

	// Overridden from IAcceptorGenerator:
	bool generate (const string_t& a_definition, const IAcceptor*& a_instance,
			const std::string& a_path)
	{
		namespace xp = ::boost::xpressive;
		using namespace xp;

		int type = 0;
		std::string path;
		sregex pattern =
			(	as_xpr("red")[ xp::ref(type) = 100 ]
			|	as_xpr("phf")[ xp::ref(type) = 200 ]
			) >> ':' >> (s1= +_)[xp::ref(path) = s1];

		type = regex_match(encode::string(a_definition), pattern) ? type : 0;
		path. insert(0, 1, '/');
		path. insert(0, a_path);

		bool success = true;
		switch (type)
		{
		case 100:
			a_instance = new Acceptor<dictionary::AcceptorRED<NLG> >(path);
			break;

		case 200:
#if defined(NPARSE_CMPH)
			a_instance = new Acceptor<dictionary::AcceptorPHF<NLG> >(path);
#else
			success = false;
#endif
			break;

		default:
			success = false;
			break;
		}

		return success;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Generator, acceptor_dictionary, nparse.acceptors.Dictionary, 1 )
