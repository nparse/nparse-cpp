/*
 * @file $/source/nparse-port/src/nparse-php.cpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.6

The MIT License (MIT)
Copyright (c) 2007-2013 Alex S Kudinov <alex@nparse.com>
 
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
#include <assert.h>
#include <map>
#include <encode/encode.hpp>
#include <nparse/_version.hpp>
#include <nparse-port/parser.hpp>

#if defined(HAVE_CONFIG_H)
#include "../php/config.h"
#endif

extern "C"
{
#include "php.h"
}

extern zend_module_entry nparse_module_entry;

#if (PHP_VERSION_ID < 50399)
typedef function_entry zend_function_entry;
#endif

/******************************************************************************/

struct nparse_object
{
	zend_object std;
	nparse::Parser* impl;
	std::wstring* text;

};

ZEND_BEGIN_ARG_INFO(arginfo_nParse__void, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, __construct)
{
	nparse_object* object = static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC));
	object -> impl = new nparse::Parser();
	object -> text = new std::wstring();
	RETURN_NULL();
}

PHP_METHOD(nParse, version)
{
	RETURN_STRING( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> version(), 1 );
}

PHP_METHOD(nParse, status)
{
	RETURN_LONG( static_cast<int>( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> status() ) );
}

PHP_METHOD(nParse, status_str)
{
	const char* status = static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC))
		-> impl -> status_str();

	if (!*status)
	{
		zend_error( E_WARNING, "the parser is in invalid state" );
		RETURN_NULL();
	}
	else
	{
		RETURN_STRING( status, 1 );
	}
}

ZEND_BEGIN_ARG_INFO(arginfo_nParse_compile, 0)
	ZEND_ARG_INFO( 0, grammar )
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, compile)
{
	const char* a_grammar = NULL;
	int a_grammar_len = 0;
	if (FAILURE == zend_parse_parameters(1 TSRMLS_CC,
				"s", &a_grammar, &a_grammar_len))
	{
		RETURN_NULL();
	}

	assert( a_grammar[a_grammar_len] == 0 );

	static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC))
	-> impl -> load(NULL, a_grammar);
	RETURN_NULL();
}

ZEND_BEGIN_ARG_INFO(arginfo_nParse_load, 0)
	ZEND_ARG_INFO( 0, filename )
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, load)
{
	const char* a_filename = NULL;
	int a_filename_len = 0;
	if (FAILURE == zend_parse_parameters(1 TSRMLS_CC,
				"s", &a_filename, &a_filename_len))
	{
		RETURN_NULL();
	}

	assert( a_filename[a_filename_len] == 0 );

	static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC))
	-> impl -> load(a_filename, NULL);
	RETURN_NULL();
}

ZEND_BEGIN_ARG_INFO(arginfo_nParse_parse, 0)
	ZEND_ARG_INFO( 0, parse )
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, parse)
{
	const char* a_input = NULL;
	int a_input_len = 0;
	if (FAILURE == zend_parse_parameters(1 TSRMLS_CC,
				"s", &a_input, &a_input_len))
	{
		RETURN_NULL();
	}

	assert( a_input[a_input_len] == 0 ); // not really necessary

	nparse_object* object = static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC));
	nparse::Parser& parser = *(object -> impl);
	std::wstring& text = *(object -> text);

	// Convert the input string to UCS4 and store in the container
	// associated with the parser object.
	encode::wstring(a_input, a_input + a_input_len). swap(text);

	RETURN_BOOL( parser. parse(text. data(), text. size()) );
}

PHP_METHOD(nParse, next)
{
	RETURN_BOOL( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> next() );
}

PHP_METHOD(nParse, step)
{
	RETURN_BOOL( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> step() );
}

PHP_METHOD(nParse, rewind)
{
	static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC)
	) -> impl -> rewind();
	RETURN_NULL();
}

PHP_METHOD(nParse, reset)
{
	nparse_object* object = static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC));
	object -> impl -> reset();
	object -> text -> clear();
	RETURN_NULL();
}

typedef std::map<std::size_t, zval*> recursive_references_t;

void wrap (zval*& a_res, const nparse::Variable& a_var, char* a_buf,
		const unsigned int a_buf_len, recursive_references_t& a_recref)
{
	switch (a_var. type())
	{
	case nparse::Variable::Boolean:
		ZVAL_BOOL(a_res, a_var. as_boolean());
		break;

	case nparse::Variable::Integer:
		ZVAL_LONG(a_res, a_var. as_integer());
		break;

	case nparse::Variable::Real:
		ZVAL_DOUBLE(a_res, a_var. as_real());
		break;

	case nparse::Variable::String:
		ZVAL_STRING(a_res, a_var. as_string(a_buf, a_buf_len), 1);
		break;

	case nparse::Variable::Array:
		{
			const std::size_t id = a_var. id();
			assert(id != 0);
			recursive_references_t::iterator found_at = a_recref. find(id);
			if (found_at != a_recref. end())
			{
				a_res = found_at -> second;
				zval_add_ref(&a_res);
			}
			else
			{
				array_init(a_res);
				a_recref. insert(recursive_references_t::value_type(id, a_res));
				for (nparse::VariableIterator i(a_var. begin()); ! i. end();
						++ i)
				{
					zval* entry = NULL;
					MAKE_STD_ZVAL(entry);
					wrap(entry, *i, a_buf, a_buf_len, a_recref);
					add_assoc_zval(a_res, i -> key(), entry);
				}
			}
		}
		break;

	case nparse::Variable::Null:
	default:
		ZVAL_NULL(a_res);
		break;
	}
}

ZEND_BEGIN_ARG_INFO(arginfo_nParse_get, 0)
	ZEND_ARG_INFO( 0, variable )
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, get)
{
	const char* a_name = NULL;
	int a_name_len = 0;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
				"|s!", &a_name, &a_name_len))
	{
		RETURN_NULL();
	}

	assert( a_name == NULL || a_name[a_name_len] == 0 );

	nparse::Variable result = static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get(a_name);

	char buf[NPARSE_STRBUF_SIZE];
	recursive_references_t recref;
	wrap(return_value, result, buf, sizeof(buf), recref);
}

PHP_METHOD(nParse, label)
{
	RETURN_LONG( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> label() );
}

PHP_METHOD(nParse, node)
{
	char buf[NPARSE_STRBUF_SIZE];
	RETURN_STRING( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> node(buf, sizeof(buf)), 1 );
}

PHP_METHOD(nParse, text)
{
	char buf[NPARSE_STRBUF_SIZE];
	RETURN_STRING( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> text(buf, sizeof(buf)), 1 );
}

PHP_METHOD(nParse, shift)
{
	RETURN_LONG( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> shift() );
}

ZEND_BEGIN_ARG_INFO(arginfo_nParse_set, 0)
	ZEND_ARG_INFO( 0, variable )
	ZEND_ARG_INFO( 0, value )
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, set)
{
	const char* a_name = NULL;
	int a_name_len = 0;
	zval* a_value = NULL;
	if (FAILURE == zend_parse_parameters(2 TSRMLS_CC,
				"sz", &a_name, &a_name_len, &a_value))
	{
		RETURN_NULL();
	}

	nparse_object* object = static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC));
	nparse::Parser& parser = *(object -> impl);

	assert( a_name[a_name_len] == 0 );

	switch (a_value -> type)
	{
	case IS_BOOL:
		parser. set( a_name, Z_BVAL_P(a_value) );
		break;

	case IS_LONG:
		parser. set( a_name, (int) Z_LVAL_P(a_value) );
		break;

	case IS_DOUBLE:
		parser. set( a_name, Z_DVAL_P(a_value) );
		break;

	case IS_STRING:
		{
			const char* str = Z_STRVAL_P(a_value);
			assert( str[Z_STRLEN_P(a_value)] == 0 );
			parser. set( a_name, str );
		}
		break;

	default:
		zend_error(E_WARNING, "can not set trace variable of this type");
		break;
	}

	RETURN_NULL();
}

PHP_METHOD(nParse, clear)
{
	static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC)
	) -> impl -> clear();
	RETURN_NULL();
}

PHP_METHOD(nParse, get_message_count)
{
	RETURN_LONG( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get_message_count() );
}

ZEND_BEGIN_ARG_INFO(arginfo_nParse_get_message, 0)
	ZEND_ARG_INFO( 0, index )
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, get_message)
{
	long a_index = 0;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
			"l", &a_index))
	{
		RETURN_NULL();
	}

	RETURN_STRING( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get_message(a_index), 1 );
}

ZEND_BEGIN_ARG_INFO(arginfo_nParse_get_location, 0)
	ZEND_ARG_INFO( 0, index )
	{   "line", 4, NULL, 0, 0, 1, 1 }, // ZEND_ARG_TYPE_INFO
	{ "offset", 6, NULL, 0, 0, 1, 1 },
ZEND_END_ARG_INFO()

PHP_METHOD(nParse, get_location)
{
	long a_index = 0;
	zval* a_line = NULL;
	zval* a_offset = NULL;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
			"l|zz", &a_index, &a_line, &a_offset))
	{
		RETURN_NULL();
	}

	int line = 0;
	int offset = 0;
	const char* location = static_cast<nparse_object*>(
			zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get_location(a_index, &line, &offset);

	if (a_line)
	{
		ZVAL_LONG(a_line, line);
	}

	if (a_offset)
	{
		ZVAL_LONG(a_offset, offset);
	}

	RETURN_STRING(location, 1);
}

PHP_METHOD(nParse, get_iteration_count)
{
	RETURN_LONG( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get_iteration_count() );
}

PHP_METHOD(nParse, get_trace_count)
{
	RETURN_LONG( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get_trace_count() );
}

PHP_METHOD(nParse, get_pool_usage)
{
	RETURN_LONG( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get_pool_usage() );
}

PHP_METHOD(nParse, get_pool_capacity)
{
	RETURN_LONG( static_cast<nparse_object*>(
				zend_object_store_get_object(getThis() TSRMLS_CC)
		) -> impl -> get_pool_capacity() );
}

zend_function_entry global_nparse_methods[] = {
	PHP_ME(nParse, __construct,			arginfo_nParse__void,			ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(nParse, version,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, status,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, status_str,			arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, compile,				arginfo_nParse_compile,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, load,				arginfo_nParse_load,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, parse,				arginfo_nParse_parse,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, next,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, step,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, rewind,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, reset,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get,					arginfo_nParse_get,				ZEND_ACC_PUBLIC)
	PHP_ME(nParse, label,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, node,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, text,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, shift,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, set,					arginfo_nParse_set,				ZEND_ACC_PUBLIC)
	PHP_ME(nParse, clear,				arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get_message_count,	arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get_message,			arginfo_nParse_get_message,		ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get_location,		arginfo_nParse_get_location,	ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get_iteration_count,	arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get_trace_count,		arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get_pool_usage,		arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	PHP_ME(nParse, get_pool_capacity,	arginfo_nParse__void,			ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

/******************************************************************************/

zend_class_entry*		global_nparse_class_entry = NULL;
zend_object_handlers	global_nparse_object_handlers;

void nparse_destroy (void* a_instance TSRMLS_DC)
{
	nparse_object* object = static_cast<nparse_object*>(a_instance);
	delete object -> text;
	delete object -> impl;
	zend_hash_destroy(object -> std. properties);
	FREE_HASHTABLE(object -> std. properties);
	efree(object);
}

zend_object_value nparse_create (zend_class_entry* a_class_entry TSRMLS_DC)
{
	zend_object_value result;
	zval* temp = NULL;

	nparse_object* object =
		static_cast<nparse_object*>(emalloc(sizeof(nparse_object)));
	memset(object, 0, sizeof(nparse_object));
	object -> std. ce = a_class_entry;

	ALLOC_HASHTABLE(object -> std. properties);
	zend_hash_init(
		object -> std. properties,
		0,
		NULL,
		ZVAL_PTR_DTOR,
		0
	);
#if (PHP_VERSION_ID < 50399)
	zend_hash_copy(
		object -> std. properties,
		& a_class_entry -> default_properties,
		reinterpret_cast<copy_ctor_func_t>(zval_add_ref),
		static_cast<void*>(&temp),
		sizeof(temp)
	);
#else
	object_properties_init(
		&(object -> std),
		a_class_entry
	);
#endif

	result. handle = zend_objects_store_put(
		object,
		NULL,
		nparse_destroy,
		NULL TSRMLS_DC
	);
	result. handlers =& global_nparse_object_handlers;

	return result;
}

PHP_MINIT_FUNCTION(nparse)
{
	zend_class_entry class_entry;
	INIT_CLASS_ENTRY(class_entry, "nParse", global_nparse_methods);
	global_nparse_class_entry = zend_register_internal_class(
			&class_entry TSRMLS_CC);
	global_nparse_class_entry -> create_object = nparse_create;
	memcpy(
		& global_nparse_object_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers)
	);
	global_nparse_object_handlers. clone_obj = NULL;
	return SUCCESS;
}

zend_module_entry nparse_module_entry = {
#if (ZEND_MODULE_API_NO >= 20010901)
	STANDARD_MODULE_HEADER,
#endif
	"nParse", // extension name
	NULL,
	PHP_MINIT(nparse),
	NULL,
	NULL,
	NULL,
	NULL,
#if (ZEND_MODULE_API_NO >= 20010901)
	NPARSE_VERSION_STR, // extension version
#endif
	STANDARD_MODULE_PROPERTIES
};

#if defined(COMPILE_DL_NPARSE)
ZEND_GET_MODULE(nparse)
#endif
