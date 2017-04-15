/*
 * @file $/include/anta/defs/base.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef ANTA_DEFS_BASE_HPP_
#define ANTA_DEFS_BASE_HPP_

namespace anta {

/******************************************************************************/

/**
 *	Template base class for classes that allow injections.
 */
template
	<	typename Derived_
	,	typename Model_
		// NOTE: The third template parameter here is used to resolve abiguity
		//		 between a default base class if it is specialized for a basic
		//		 model and a base class specialized for an extended model.
	,	typename IsBasic_ = typename meta::is_basic<Model_>::type
	>
class Base
{
};

/**
 *	Auxiliary base class that uses so-called static polymorphism to convert a
 *	pointer to itself to the derived type.
 */
template <typename Derived_>
class BaseStatic
{
protected:
	/**
	 *	Derived type.
	 */
	typedef Derived_ derived_type;

	/**
	 *	Get a mutable reference of derived type to itself.
	 */
	derived_type* self ()
	{
		return static_cast<derived_type*>(this);
	}

	/**
	 *	Get a constant reference of derived type to itself.
	 */
	const derived_type* self () const
	{
		return static_cast<const derived_type*>(this);
	}

};

/**
 *	Auxiliary base class that uses traditional dynamic polymorphism to convert a
 *	pointer to itself to the derived type.
 */
template <typename Derived_>
class BaseDynamic
{
public:
	/**
	 *	The polymorphic destructor.
	 */
	virtual ~BaseDynamic () {}

protected:
	/**
	 *	Derived type.
	 */
	typedef Derived_ derived_type;

	/**
	 *	Get a mutable reference of derived type to itself.
	 */
	derived_type* self ()
	{
		return dynamic_cast<derived_type*>(this);
	}

	/**
	 *	Get a constant reference of derived type to itself.
	 */
	const derived_type* self () const
	{
		return dynamic_cast<const derived_type*>(this);
	}

};

/******************************************************************************/

} // namespace anta

#endif /* ANTA_DEFS_BASE_HPP_ */
