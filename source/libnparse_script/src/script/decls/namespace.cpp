/*
 * @file $/source/libnparse_script/src/script/decls/namespace.cpp
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
#include <anta/sas/string.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/regex.hpp>
#include "../../static.hpp"

namespace {

using namespace nparse;

class Construct: public IConstruct
{
	bool create_namespace (const hnd_arg_t& arg)
	{
		arg. staging. setNamespace(get_accepted_str(arg));
		return true;
	}

	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
// <DEBUG_NODE_NAMING>
		entry_		("Namespace.Entry")
// </DEBUG_NODE_NAMING>
	{
		using namespace anta::ndl::terminals;
		using boost::proto::lit;

		anta::Label<SG>
			doCreateNamespace = hnd_t(this, &Construct::create_namespace);

		entry_ = "namespace" > +space
			> regex("\\A(?=\\w)[-_\\.\\w]+(?<=\\w)") * doCreateNamespace;
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN_STATIC_EXPORT_SINGLETON(
		Construct, decl_namespace, nparse.script.decls.Namespace, 1 )
