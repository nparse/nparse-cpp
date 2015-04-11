/*
The MIT License (MIT)
Copyright (c) 2010-2013 Alex S Kudinov <alex@nparse.com>
 
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

#define PLUGIN_EXCEPTION(exception, message)	\
	class exception: public plugin_error {		\
	public: exception (): plugin_error ("plugin: " message) {} };

PLUGIN_EXCEPTION(
	interface_type_conflict,
	"the instantiated object can not be referred with a pointer of this type")

PLUGIN_EXCEPTION(
	interface_version_conflict,
	"a factory producing instances of this version has been installed already")

PLUGIN_EXCEPTION(
	unknown_interface,
	"no factory producing instances of this interface has been found")

PLUGIN_EXCEPTION(
	unknown_interface_version,
	"no factory producing instances of this version of the interface has been"
	" found")

} // namespace plugin

#endif /* PLUGIN_EXCEPTIONS_HPP_ */
