/*
 * @file $/source/nparse-port/src/variable_data.hpp
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
#ifndef SRC_VARIABLE_HPP_
#define SRC_VARIABLE_HPP_

namespace nparse {

struct VariableData
{
	// NOTE: Can't use context_entry here because its first element is constant.
	typedef std::pair<
			anta::ndl::context_key<NLG>::type,
			anta::ndl::context_value<NLG>::type
		> entry_type;

	entry_type entry;

	typedef std::vector<anta::ndl::context_key<NLG>::type> keys_type;
	boost::shared_ptr<keys_type> keys;

};

struct VariableIteratorData
{
	const anta::ndl::Context<NLG>* context;
	VariableData::keys_type::const_iterator begin, end;
	Variable current;

};

} // namespace nparse

#endif /* SRC_VARIABLE_HPP_ */
