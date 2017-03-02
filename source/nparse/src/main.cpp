/*
 * @file $/source/nparse/src/main.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.7

The MIT License (MIT)
Copyright (c) 2007-2017 Alex S Kudinov <alex.s.kudinov@nparse.com>
 
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
#if !defined(DEBUG)
#	include <iostream>
#endif

#include "nparse_app.hpp"

// Program entry point.
int main (const int argc, char** argv)
{
	int res = 0;
#if !defined(DEBUG)
	try
	{
#endif
		// Instantiate the main application class.
		nParseApp application;

		// Configure the application using command line arguments.
		if (application. configure(argc, argv))
		{
			// Execute the application.
			res = application. execute();
		}
#if !defined(DEBUG)
	}
	catch (const std::exception& err)
	{
		std::cerr << "error:\n\t" << err. what() << std::endl;
		res = 1;
	}
#endif

	plugin::IManager::instance(). shutdown();
	return res;
}
