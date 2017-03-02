/*
The MIT License (MIT)
Copyright (c) 2010-2017 Alex S Kudinov <alex.s.kudinov@gmail.com>

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
#ifndef PLUGIN_EXCEPTIONS_HPP_
#define PLUGIN_EXCEPTIONS_HPP_

#include <stdexcept>

namespace plugin {

class plugin_error: public std::logic_error
{
public:
	plugin_error (const char* a_msg):
		std::logic_error (a_msg)
	{
	}

};

#define PLUGIN_EXCEPTION( EXCEPTION, MESSAGE )	\
	class EXCEPTION: public plugin_error {		\
	public: EXCEPTION (): plugin_error ("plugin: " MESSAGE) {} };

PLUGIN_EXCEPTION(
	interface_not_found,
	"interface not found")

PLUGIN_EXCEPTION(
	interface_name_conflict,
	"interface name conflict")

PLUGIN_EXCEPTION(
	interface_type_conflict,
	"interface type conflict")

PLUGIN_EXCEPTION(
	using_incomplete_instance,
	"using incomplete instance")

PLUGIN_EXCEPTION(
	using_invalid_instance,
	"using invalid instance")

#undef PLUGIN_EXCEPTION

} // namespace plugin

#endif /* PLUGIN_EXCEPTIONS_HPP_ */
