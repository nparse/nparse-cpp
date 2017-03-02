/*
 * @file $/source/libnparse_factory/src/dictionary/dictionary.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex.s.kudinov@gmail.com>
 
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
#ifndef SRC_DICTIONARY_DICTIONARY_HPP_
#define SRC_DICTIONARY_DICTIONARY_HPP_

#include <utility>
#include <vector>
#include <string>

namespace dictionary {

typedef std::pair<std::string, std::vector<std::string> > category_t;

typedef std::pair<std::string, std::vector<category_t> > entry_t;

class Dictionary: public std::vector<entry_t>
{
public:
	bool parse_entry (std::string::const_iterator& a_begin,
			const std::string::const_iterator& a_end);

	bool parse_entry (const std::string& a_definition);

	int load (const std::string& a_filename);

};

} // namespace dictionary

#endif /* SRC_DICTIONARY_DICTIONARY_HPP_ */
