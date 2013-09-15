/*
 * @file $/include/nparse/interfaces.hpp
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
#ifndef NPARSE_INTERFACES_HPP_
#define NPARSE_INTERFACES_HPP_

#include <boost/scoped_ptr.hpp>

namespace nparse {

/**
 *	An intermediate interface that helps to organize instances within a
 *	particular plugin category.
 */
class IPrioritized: public plugin::IInterface
{
public:
	/**
	 *	Get instance priority.
	 */
	virtual int priority () const = 0;

};

/**
 *	An interface for objects that are responsible for handling single instances
 *	of acceptors specific to the NLG model.
 */
class IAcceptor
{
public:
	/**
	 *	The destructor (needed for correct memory disposal).
	 */
	virtual ~IAcceptor () {}

	/**
	 *	Get a const reference to the actual implementation of the acceptor.
	 */
	virtual operator const anta::Acceptor<NLG>& () const = 0;

};

/**
 *	An interface for objects that are responsible for instantiation of acceptors
 *	specific to the NLG model using string definitions.
 */
class IAcceptorGenerator: public IPrioritized
{
public:
	/**
	 *	Generate an acceptor instance from the given string definition.
	 */
	virtual bool generate (const string_t& a_definition,
			const IAcceptor*& a_instance, const std::string& a_path) = 0;

};

/**
 *	An interface for the aceptor factory.
 */
class IAcceptorFactory: public IAcceptorGenerator
{
public:
	/**
	 *	Import generator family.
	 */
	virtual int import (const std::string& a_family) = 0;

	/**
	 *	Reorder the inner generator list according to the given suffix list.
	 */
	virtual void reorder (const std::vector<std::string>& a_suffix_list,
			const bool a_make_first) = 0;

};

/**
 *	An interface for objects that represent constructs (pluggable grammatical
 *	modules) of the script grammar.
 */
class IConstruct: public plugin::IInterface
{
public:
	/**
	 *	Get the entry node of a subnetwork.
	 */
	virtual const anta::ndl::Rule<SG>& entry (const int a_level = 0) const = 0;

};

/**
 *	A pseudo inteface for an object that represents the execution environment
 *	for semantic actions associated with the elements of an NLG network.
 */
class IEnvironment
{
public:
	/**
	 *	Necessary type definitions for meeting the requirements of the Context
	 *	concept.
	 *	@{ */
	typedef anta::ndl::context_key<NLG>::type key_type;
	typedef anta::ndl::context_value<NLG>::type value_type;
	/**	@} */

	/**
	 *	Get the traveller.
	 */
	inline anta::Traveller<NLG>& get_traveller () const;

	/**
	 *	Get a mutable reference to a trace variable from the local context.
	 */
	inline value_type& ref (const key_type& a_key, const bool a_reset = false);

	/**
	 *	Get a constant reference to a trace variable from the global context.
	 */
	inline const value_type& val (const key_type& a_key) const;

	/**
	 *	Create an additional stored context.
	 */
	inline boost::shared_ptr<anta::ndl::Context<NLG> > create (
			const anta::ndl::Context<NLG>* a_ancestor = NULL) const;

public:
	/**
	 *	The only constructor.
	 */
	inline IEnvironment (anta::Traveller<NLG>& a_traveller,
			const bool a_local_only);

	/**
	 *	The destructor.
	 */
	inline ~IEnvironment ();

private:
	anta::Traveller<NLG>& m_traveller;
	bool m_local_only;
	boost::scoped_ptr<anta::ndl::Context<NLG> > m_local_context;

};

/**
 *	An interface for objects that represent semantic actions specific to the NLG
 *	model.
 */
class IAction
{
public:
	virtual ~IAction () {}

public:
	/**
	 *	Common result type.
	 */
	typedef IEnvironment::value_type result_type;

	/**
	 *	Execute a semantic action in the given environment and return a copy of
	 *	the result.
	 */
	virtual result_type evalVal (IEnvironment& a_env) const
	{
		throw ex::no_rvalue();
	}

	/**
	 *	Execute a semantic action in the given environment and return a mutable
	 *	reference to the stored result.
	 */
	virtual result_type& evalRef (IEnvironment& a_env) const
	{
		throw ex::no_lvalue();
	}

	/**
	 *	Get location in the source script.
	 */
	virtual bool getLocation (anta::range<nparse::SG>::type& a_range) const
	{
		return false;
	}

};

/**
 *	A semantic action pointer that complements runtime exceptions with the
 *	necessary information on a source location in a grammar script.
 */
class action_pointer: public boost::shared_ptr<IAction>
{
public:
	action_pointer ():
		boost::shared_ptr<IAction>()
	{
	}

	action_pointer (IAction* a_ptr):
		boost::shared_ptr<IAction>(a_ptr)
	{
	}

	inline IAction::result_type evalVal (IEnvironment& a_env) const
	{
		try
		{
			return (*this) -> evalVal(a_env);
		}
		catch (ex::runtime_error& err)
		{
			rethrow(err);
		}
		catch (const std::exception& err)
		{
			rethrow(err);
		}
	}

	inline IAction::result_type& evalRef (IEnvironment& a_env) const
	{
		try
		{
			return (*this) -> evalRef(a_env);
		}
		catch (ex::runtime_error& err)
		{
			rethrow(err);
		}
		catch (const std::exception& err)
		{
			rethrow(err);
		}
	}

private:
	void rethrow (ex::runtime_error& err) const
	{
		if (! boost::get_error_info<ex::location>(err))
		{
			anta::range<SG>::type where;
			if ((*this) -> getLocation(where))
				err << ex::location(where);
		}
		throw err;
	}

	void rethrow (const std::exception& err) const
	{
		ex::runtime_error rte;
		rte << ex::message(err. what());
		rethrow(rte);
	}

};

/**
 *	An interface for a storage object that is used for exchanging of arbitrary
 *	intermediate data between the building blocks (constructs) of the script
 *	grammar.
 */
class IStaging: public plugin::IInterface
{
public:
	/**
	 *	Allocate or pick up a named cluster.
	 */
	virtual anta::ndl::Cluster<NLG>& cluster (const string_t& a_name) = 0;

	/**
	 *	Allocate or pick up an acceptor.
	 */
	virtual const anta::Acceptor<NLG>& acceptor (const string_t& a_def) = 0;

	/**
	 *	Store a custom acceptor pointer for the delayed destruction.
	 */
	virtual const anta::Acceptor<NLG>& acceptor (IAcceptor* a_ptr) = 0;

	/**
	 *	Push a semantic action to the action stack.
	 */
	virtual void push (const action_pointer& a_ptr) = 0;

	/**
	 *	Pop a semantic action from the action stack.
	 */
	virtual action_pointer pop () = 0;

	/**
	 *	Extract all semantic actions form the action stack and store them in the
	 *	given vector in direct (FIFO) order.
	 */
	virtual void swap (std::vector<action_pointer>& a_av) = 0;

	/**
	 *	Add a source file to the import queue.
	 */
	virtual void import (const std::string& a_path, const bool a_reset = false)
		= 0;

	/**
	 *	Load next source file from the import queue.
	 */
	virtual bool load (anta::range<SG>::type& a_range) = 0;

	/**
	 *	Identify a location in the source files by an iterator.
	 */
	virtual bool identify (const anta::iterator<SG>::type& a_iterator,
			std::string& a_file, int& a_line, int& a_offset) const = 0;

	/**
	 *	Set current namespace.
	 */
	virtual void setNamespace (const string_t& a_namespace = string_t()) = 0;

	/**
	 *	Get current namespace.
	 */
	virtual const string_t& getNamespace () const = 0;

	/**
	 *	LL joint pointer.
	 */
	class joint_pointer: public boost::shared_ptr<anta::ndl::JointBase<NLG> >
	{
		typedef boost::shared_ptr<anta::ndl::JointBase<NLG> > base_type;

	public:
		/**
		 *	The default constructor.
		 */
		joint_pointer ():
			base_type ()
		{
		}

		/**
		 *	Create a jump joint.
		 */
		inline joint_pointer (const anta::Acceptor<NLG>& a_acceptor,
				const anta::Label<NLG>& a_label);

		/**
		 *	Create a call joint.
		 */
		inline joint_pointer (const anta::ndl::Cluster<NLG>& a_cluster);

		/**
		 *	Clone arbitrary joint structure.
		 *	@{ */
		template <typename Joint_>
		joint_pointer (const Joint_& a_joint):
			base_type ()
		{
			(*this)(a_joint);
		}

		template <typename Joint_>
		void operator() (const Joint_& a_joint)
		{
			reset(new Joint_(a_joint));
		}
		/**	@} */

	};

	/**
	 *	Push a joint to the LL stack.
	 */
	virtual void pushJoint (const joint_pointer& a_ptr) = 0;

	/**
	 *	Pop a joint from the LL stack.
	 */
	virtual joint_pointer popJoint () = 0;

	/**
	 *	Bind a pluggable object factory.
	 */
	virtual void bind (plugin::IFactory* a_factory) = 0;

	/**
	 *	Extend an exception with all avaiable relevant information.
	 */
	virtual void extend (ex::generic_error& a_error) const = 0;

};

/**
 *	An interface for objects representing operators in the embedded scripting
 *	language.
 */
class IOperator: public IPrioritized
{
public:
	/**
	 *	An auxiliary type representing operator deployment level.
	 */
	typedef anta::ndl::Rule<SG>& level_t;

	/**
	 *	Deploy the operator.
	 */
	virtual void deploy (level_t a_current, level_t a_previous, level_t a_top)
		const = 0;

};

/**
 *	An interface for objects that represent functions in the embedded scripting
 *	language.
 */
class IFunction: public plugin::IInterface
{
public:
	/**
	 *	Common result type.
	 */
	typedef IEnvironment::value_type result_type;

	/**
	 *	Function argument list type.
	 */
	typedef std::vector<action_pointer> arguments_type;

	/**
	 *	Execute a semantic action in the given environment.
	 */
	virtual result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		throw ex::no_rvalue();
	}

	/**
	 *	Execute a semantic action in the given environment and return a mutable
	 *	reference to the stored result.
	 */
	virtual result_type& evalRef (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		throw ex::no_lvalue();
	}

};

} // namespace nparse

#endif /* NPARSE_INTERFACES_HPP_ */
