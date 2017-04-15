/*
 * @file $/include/nparse/specs/sg/rcopy.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef NPARSE_SPECS_SG_RCOPY_HPP_
#define NPARSE_SPECS_SG_RCOPY_HPP_

/**
 *	The utility::rcopy<...> template specialization makes semantic actions given
 *	as the domain specific lambda expressions value-copyable.
 */
namespace utility {
namespace proto = boost::proto;

template <typename Expr_>
class rcopy<anta::dsel::lambda<nparse::SG>::type<Expr_> >
{
public:
	typedef typename proto::result_of::deep_copy<
				anta::dsel::lambda<nparse::SG>::type<Expr_>
			>::type type, ref;

	static type f (const anta::dsel::lambda<nparse::SG>::type<Expr_>& a_lambda)
	{
		return proto::deep_copy(a_lambda);
	}

};

} // namespace utility

#endif /* NPARSE_SPECS_SG_RCOPY_HPP_ */
