/*
 * @file $/source/libnparse_factory/src/factory.cpp
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
#include <plugin/category.hpp>
#include <nparse/nparse.hpp>
#include "static.hpp"

namespace {

using namespace nparse;

/*
 *	Interface implementation.
 */
class AcceptorFactory: public IAcceptorFactory
{
public:
	AcceptorFactory ();

	// Overridden from IPrioritized:

	int priority () const
	{
		return 0;
	}

	// Overridden from IAcceptorFactory:

	bool create (const string_t& a_definition, const IAcceptor*& a_instance,
			const std::string& a_path);

private:
	typedef plugin::category<IAcceptorFactory> delegates_t;
	delegates_t m_delegates;

};

AcceptorFactory::AcceptorFactory ():
	m_delegates ("nparse.acceptors")
{
	// Sort acceptor generator category in priority ascending order.
	std::sort(m_delegates. begin(), m_delegates. end(), less_priority());
}

bool AcceptorFactory::create (const string_t& a_definition,
		const IAcceptor*& a_instance, const std::string& a_path)
{
	a_instance = NULL;
	for (delegates_t::iterator i = m_delegates. begin();
			i != m_delegates. end(); ++ i)
	{
		if (i -> second -> create(a_definition, a_instance, a_path))
			return true;
	}
	return false;
}

} // namespace

PLUGIN(AcceptorFactory, acceptor_factory, nparse.AcceptorFactory)
