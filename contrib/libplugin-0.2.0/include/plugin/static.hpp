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
#ifndef PLUGIN_STATIC_HPP_
#define PLUGIN_STATIC_HPP_

/**
 *	Declare a plugin.
 */
#define PLUGIN( CLASS, SLOT, NAME ) \
	namespace { plugin::install<CLASS> install##SLOT(#NAME); } \
	const void* plugin::PLUGIN_NAMESPACE::static_export::SLOT =& \
		install##SLOT;

/**
 *	Declare a plugin module, i.e. a collection of plugins.
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
 *	Deploy initialization point for a plugin module.
 */
#define PLUGIN_STATIC_DEPLOY \
	namespace plugin { namespace PLUGIN_NAMESPACE { \
		void init () { delete new static_export; } \
	} }

/**
 *	Import plugin module initialization point.
 */
#define PLUGIN_STATIC_IMPORT( NS ) \
	namespace plugin { namespace NS { \
		extern void init (); \
	} }

/**
 *	Invoke plugin module initialization point.
 */
#define PLUGIN_STATIC_INIT( NS ) \
	plugin::NS::init()

#endif /* PLUGIN_STATIC_HPP_ */
