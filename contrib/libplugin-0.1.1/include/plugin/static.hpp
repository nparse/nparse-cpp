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
#ifndef PLUGIN_STATIC_HPP_
#define PLUGIN_STATIC_HPP_

/**
 *	Declare a statically initializable plug-in.
 */
#define PLUGIN_STATIC_EXPORT( CLASS, SLOT, NAME, VERSION ) \
	namespace { plugin::install<CLASS> SLOT##_v##VERSION ( #NAME, VERSION ); } \
	const void* plugin::PLUGIN_NAMESPACE::static_export::SLOT =& \
		SLOT##_v##VERSION;

#define PLUGIN_STATIC_EXPORT_SINGLETON( CLASS, SLOT, NAME, VERSION ) \
	namespace { plugin::install_singleton<CLASS> SLOT##_v##VERSION ( #NAME, \
			VERSION ); } \
	const void* plugin::PLUGIN_NAMESPACE::static_export::SLOT =& \
		SLOT##_v##VERSION;

/**
 *	Declare a statically initializable plug-in module.
 */
#define PLUGIN_STATIC_MODULE( SLOT_LIST ) \
	namespace plugin { \
		extern void eval (...); \
	namespace PLUGIN_NAMESPACE { \
		struct static_export { \
			typedef const void* addr; \
			static addr SLOT_LIST; \
			static_export () { eval( SLOT_LIST ); } }; \
	} }

/**
 *	Deploy the initialization point for a plug-in module.
 */
#define PLUGIN_STATIC_DEPLOY \
	namespace plugin { namespace PLUGIN_NAMESPACE { \
		void init () { delete new static_export; } \
	} }

/**
 *	Declare the export of a plug-in module initialization point.
 */
#define PLUGIN_STATIC_IMPORT( NS ) \
	namespace plugin { namespace NS { \
		extern void init (); \
	} }

/**
 *	Call the initialization point of a plug-in module.
 */
#define PLUGIN_STATIC_INIT( NS ) \
	plugin::NS::init()

#endif /* PLUGIN_STATIC_HPP_ */
