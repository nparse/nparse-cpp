/*
 * @file $/source/libnparse_script/src/script/constructs/foreach.cpp
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
#include <algorithm>
#include <nparse/nparse.hpp>
#include <nparse/util/linked_action.hpp>
#include <anta/dsel/rt/assign.hpp>
#include <anta/dsel/rt/comma.hpp>
#include <anta/sas/symbol.hpp>
#include <anta/sas/test.hpp>
#include <anta/sas/regex.hpp>
#include "../../static.hpp"
#include "control.hpp"

namespace {

using namespace nparse;

template <typename M_>
struct keys
{
	typedef std::vector<typename anta::ndl::context_key<M_>::type> type;

};

template <typename M_>
class key_lister: public std::iterator<std::output_iterator_tag, void, void,
	void, void>
{
	typename keys<M_>::type& m_keys;

public:
	key_lister (typename keys<M_>::type& a_keys):
		m_keys (a_keys)
	{
	}

	key_lister& operator= (const typename anta::ndl::context_entry<M_>::type&
			a_pair)
	{
		if (! a_pair. first. empty())
			m_keys. push_back(a_pair. first);
		return *this;
	}

	key_lister& operator* () { return *this; }
	key_lister& operator++ () { return *this; }
	key_lister& operator++ (int) { return *this; }

#if defined(BOOST_MSVC)
	const key_lister& operator= (const key_lister& a_inst)
	{
		if (this != &a_inst)
		{
			this -> ~key_lister();
			new(this) key_lister(a_inst. m_keys);
		}
		return *this;
	}
#endif
};

class Action: public LinkedAction
{
public:
	// Overridden from IAction:

	result_type evalVal (IEnvironment& a_env) const
	{
		// Get source array.
		result_type list = m_list. evalVal(a_env);

		if (list. is_null())
		{
			return result_type();
		}

		if (! list. is_array())
		{
			anta::range<SG>::type where;
			getLocation(where);
			throw ex::runtime_error()
				<< ex::function("foreach")
				<< ex::location(where)
				<< ex::message(
					"argument is supposed to be either array or null");
		}

		// Get array keys.
		keys<NLG>::type klist;
		list. as_array() -> list(key_lister<NLG>(klist), false);

		// Get a mutable reference to the key storage (variable).
		result_type& iter = m_iter. evalRef(a_env);

		// Go through the key list.
		for (keys<NLG>::type::const_iterator i = klist. begin();
				i != klist. end(); ++ i)
		{
			try
			{
				iter = static_cast<const std::string&>(*i);
				m_body. evalVal(a_env);
			}
			catch (const loop_control_t& lc)
			{
				if (! lc. target(). empty() && lc. target() != m_loop)
					throw;

				if (lc. exit())
					break;
			}
		}

		// Return nothing.
		return result_type();
	}

public:
	Action (const anta::range<SG>::type& a_range, const std::string& a_loop,
			IStaging& a_staging):
		LinkedAction (a_range), m_loop (a_loop)
	{
		m_body = a_staging. pop();
		m_list = a_staging. pop();
		m_iter = a_staging. pop();
	}

private:
	std::string m_loop;
	action_pointer m_iter, m_list, m_body;

};

class Construct: public IConstruct
{
	bool create_action (const hnd_arg_t& arg)
	{
		// Determine the name of the loop.
		const anta::string<SG>::type loop =
			arg. state. val("loop"). as_string();

		// Instantiate and push a new action.
		arg. staging. push(new Action(get_marked_range(arg), loop,
					arg. staging));

		return true;
	}

	plugin::instance<IConstruct> m_expression, m_statement;
	anta::ndl::Rule<SG> entry_;

public:
	Construct ():
		m_expression ("nparse.script.Expression"),
		m_statement ("nparse.script.Statement"),
// <DEBUG_NODE_NAMING>
		entry_		("ForEach.Entry")
// </DEBUG_NODE_NAMING>
	{
	}

	void initialize ()
	{
		using namespace anta::ndl::terminals;
		using namespace anta::dsel;

		anta::Label<SG>
			doCreateAction = hnd_t(this, &Construct::create_action);

		reference<SG>::type loop = ref<SG>("loop");

		entry_ =
			(	(+alnum) [ loop = delta(), true ] > regex("\\A\\s*:\\s*")
			|	pass [ loop = "", true ] )
		>	regex("\\Afor\\s*each\\s*\\(\\s*") * M0 > m_expression -> entry()
		>	regex("\\A\\s*in(?!\\w)\\s*") > m_expression -> entry()
		>	regex("\\A\\s*\\)\\s*") [ push(loop) ]
		>	m_statement -> entry()
		>	pass [ pop(loop) ] * M1 * doCreateAction;
	}

	const anta::ndl::Rule<SG>& entry (const int) const
	{
		return entry_;
	}

};

} // namespace

PLUGIN(Construct, construct_foreach, nparse.script.constructs.ForEach)
