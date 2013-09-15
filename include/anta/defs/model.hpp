/*
 * @file $/include/anta/defs/model.hpp
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
#ifndef ANTA_DEFS_MODEL_HPP_
#define ANTA_DEFS_MODEL_HPP_

namespace anta {

/**
 *	Common integer type.
 */
typedef unsigned long uint_t;

/**
 *	A bit of metaprogramming :)
 */
namespace meta {

/// True tag.
struct true_ {};

/// False tag.
struct false_ {};

/// Narrow sequence tag.
struct narrow {};

/// Wide sequence tag.
struct wide {};

/**
 *	Metafunction that is used to separate basic and extended models.
 */
template <typename Model_, typename Phony_ = Model_>
struct is_basic
{
	// NOTE: All the models are supposed to be basic unless the opposite is
	//		 declared (e.g. by a specialization of this metafunction for some
	//		 particular template arguments).
	typedef true_ type;

};

} // namespace meta

/**
 *	Basic model. Ta-Daa!
 */
template <typename Sequence_ = meta::narrow>
struct model
{
	/**
	 *	Model extension tag.
	 */
	struct extension {};

	/**
	 *	Type of sequence.
	 */
	typedef Sequence_ sequence;

};

/**
 *	Metafunction that is used to determine model of a parametrized type.
 *	@{ */
template <typename Type_> struct model_of;

/// Specialization for parametrized classes.
template <typename M_, template <typename M2_> class Class_>
struct model_of<Class_<M_> >
{
	typedef M_ type;

};

/**	@} */

} // namespace anta

#endif /* ANTA_DEFS_MODEL_HPP_ */
