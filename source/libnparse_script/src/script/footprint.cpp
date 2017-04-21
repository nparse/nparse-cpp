/*
 * @file $/source/libnparse_script/src/script/footprint.cpp
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
#include <nparse/nparse.hpp>
#include "../static.hpp"

namespace {

static const unsigned char flat[] = {
	0x09, 0x1f, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0xba, 0xc9, 0x63, 0xf2,
	0xe0, 0xa0, 0xe9, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0xd2,
	0x11, 0x69, 0x41, 0xce, 0x42, 0x45, 0xb9, 0x14, 0xbd, 0x1c, 0xaa, 0x11,
	0xc1, 0xf5, 0x65, 0x11, 0xa0, 0x99, 0xcc, 0x69, 0x68, 0x88, 0xcd, 0x4c,
	0x23, 0x20, 0x11, 0xc1, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5,
	0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5, 0xf5,
	0xf5, 0xf5, 0xf5, 0x65, 0x11, 0xf2, 0xe1, 0x2d, 0x63, 0xf2, 0xe0, 0xc3,
	0xa0, 0xe9, 0xd4, 0xcc, 0x88, 0xcc, 0x88, 0xcc, 0x88, 0xcc, 0xf4, 0x75,
	0xe0, 0xc3, 0xa0, 0xf2, 0xd9, 0xdd, 0x5c, 0xc9, 0x63, 0xf2, 0xe0, 0xc3,
	0xa0, 0xe9, 0x75, 0xe0, 0xc3, 0xc3, 0xa0, 0xe9, 0x60, 0x75, 0xe0, 0xc3,
	0xc3, 0xa0, 0x2f, 0xea, 0x62, 0x69, 0x68, 0x88, 0xe9, 0x74, 0x4c, 0xe8,
	0xcc, 0x88, 0xe9, 0xdc, 0x10, 0x6a, 0x5b, 0x6a, 0x5b, 0x7a, 0xe0, 0x6b,
	0xb8, 0x75, 0xcc, 0x88, 0xcc, 0x69, 0x68, 0x63, 0x53, 0x10, 0x6a, 0x5b,
	0x7a, 0xe0, 0x53, 0x10, 0x65, 0x65, 0x5b, 0x7a, 0xe0, 0xa0, 0xcd, 0x9a,
	0xf6, 0x62, 0x88, 0xcc, 0x88, 0xcc, 0x88, 0xcc, 0x88, 0xcc, 0xf4, 0x75,
	0xe0, 0x53, 0x10, 0x6a, 0x5b, 0x6a, 0x5b, 0x65, 0xd4, 0xcc, 0x88, 0xcc,
	0x88, 0xcc, 0x88, 0xcc, 0xf4, 0x60, 0x60, 0x74, 0x61, 0xc4, 0xa0, 0xe9,
	0x74, 0x4c, 0xe8, 0xcc, 0x11, 0xe0, 0xc3, 0x53, 0x92, 0xed, 0x62, 0x88,
	0xcc, 0x88, 0xcc, 0x88, 0xcc, 0x88, 0xcc, 0x88, 0xcc, 0xf4, 0x60, 0x60,
	0x74, 0x4c, 0x88, 0x61, 0x74, 0x6c, 0xe7, 0xe8, 0xcc, 0x88, 0xcc, 0x88,
	0xcc, 0xf4, 0x60, 0x60, 0xba, 0xc9, 0xd6, 0x0c, 0x67, 0x88, 0xcc, 0x28,
	0x4c, 0x88, 0x61, 0xc4, 0x36, 0x3f, 0x53, 0xc8, 0x8f, 0x16, 0x7e, 0x02,
	0x8f, 0x16, 0x7e, 0x02, 0x8f, 0x16, 0xaf, 0x60, 0x60, 0x60, 0x74, 0x61,
	0xc4, 0x53, 0x2d, 0x0b, 0x8f, 0x16, 0x7e, 0x02, 0x8f, 0x16, 0xaf, 0x60,
	0x60, 0x0b, 0x02, 0x05, 0xc3, 0xc3, 0x53, 0x10, 0x65, 0x74, 0x61, 0xc4,
	0xc3, 0xbd, 0x68, 0x84, 0xaa, 0x11, 0xc1, 0x20, 0x44, 0x62, 0xd0, 0xc4,
	0xed, 0xdc, 0x8b, 0xfb, 0x24, 0x53, 0x9d, 0x05, 0xb7, 0x5c, 0x45, 0x66,
	0x0b, 0x70, 0x07, 0xad, 0xee, 0xe8, 0x58, 0x82, 0x3b, 0xd3, 0xdc, 0x05,
	0x32, 0x7b, 0xd3, 0xcf, 0xba, 0xf8, 0x71, 0xd8, 0x20, 0x65, 0x11, 0xf2,
	0x7d, 0xe5, 0x9a, 0xbc, 0x68, 0xc7, 0xd0, 0x70, 0xd2, 0xdb, 0x5c, 0xc4,
	0x3f, 0x02, 0x02, 0x55, 0x72, 0x52, 0x0d, 0x86, 0xac, 0x5b, 0xe3, 0x22,
	0x4e, 0xd1, 0xda, 0x02, 0x60, 0x60, 0x60, 0x6b, 0x9d, 0xda, 0xd9, 0x5d,
	0x53, 0xea, 0xe2, 0x1a, 0x79, 0x0a, 0x45, 0x4f, 0x2e, 0x15, 0x37, 0xc6,
	0xd9, 0x4b, 0x0d, 0x8f, 0xd0, 0x78, 0x9a, 0x1a, 0x79, 0x0a, 0x45, 0x2f,
	0x53, 0xda, 0xab, 0x0f, 0x74, 0x5b, 0x07, 0x9e, 0x01, 0x40, 0x4b};

static const unsigned char salt[] = {
	37, 113, 41, 73, 89, 3, 13, 47, 17, 11, 97, 7, 71, 107, 59,
	61, 109, 43, 29, 23, 19, 5, 103, 79, 67, 83, 53, 31, 127, 101};

using namespace nparse;

class Function: public IFunction
{
public:
	// Overridden from IFunction:

	result_type evalVal (IEnvironment& a_env,
			const arguments_type& a_arguments) const
	{
		if (a_arguments. size() != 2)
		{
			throw ex::runtime_error()
				<< ex::function("footprint")
				<< ex::message("invalid argument count");
		}

		unsigned char x[3] = { 0, 0, 0 };

		for (arguments_type::const_iterator arg = a_arguments. begin();
				arg != a_arguments. end(); ++ arg)
		{
			const result_type vval = arg -> evalVal(a_env);
			if (vval. id() != anta::aux::type_id::integer)
			{
				throw ex::runtime_error()
					<< ex::function("footprint")
					<< ex::message("all arguments must be of integer type");
			}
			const anta::aux::integer<NLG>::type cval = vval. as_integer();
			if (cval < 0 || 255 < cval)
			{
				throw ex::runtime_error()
					<< ex::function("footprint")
					<< ex::message("all arguments must be in range [0, 255]");
			}
			x[arg - a_arguments. begin() + 1]
				= static_cast<unsigned char>(cval);
		}

		std::string res;
		res. reserve(sizeof(flat));
		for (std::size_t i = 0; i < sizeof(flat); ++ i)
		{
			x[0] = x[1];
			x[1] = x[2];
			x[2] = flat[i] * salt[(1 + x[0] * x[1]) % 30] - x[0] + x[1];
			res. push_back(static_cast<char>(x[2] & 0x7F));
		}

		return result_type(res);
	}

};

} // namespace

PLUGIN(Function, script_footprint, nparse.script.functions.footprint)
