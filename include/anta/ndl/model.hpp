/*
 * @file $/include/anta/ndl/model.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.8

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
#ifndef ANTA_NDL_MODEL_HPP_
#define ANTA_NDL_MODEL_HPP_

namespace anta {

/******************************************************************************/

namespace ndl {

/**
 *	Model extension metafunction.
 *	@{ */

struct tag {};

template <typename Model_, typename Ext_ = typename Model_::extension>
struct extend
{
	struct type: public Model_
	{
		typedef tag extension;

	};

};

template <typename Model_>
struct extend<Model_, tag>
{
	typedef Model_ type;

};

/**	@} */

} // namespace ndl

/******************************************************************************/

namespace meta {

/**
 *	Specialization of the metafunction that identifies extended models.
 */
template <typename Model_>
struct is_basic<typename ndl::extend<Model_>::type, Model_>
{
	typedef false_ type;

};

} // namespace meta

/******************************************************************************/

} // namespace anta

#endif /* ANTA_NDL_MODEL_HPP_ */
