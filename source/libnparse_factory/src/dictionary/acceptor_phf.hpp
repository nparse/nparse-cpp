/*
 * @file $/source/libnparse_factory/src/dictionary/acceptor_phf.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.4

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
#ifndef SRC_DICTIONARY_ACCEPTOR_PHF_HPP_
#define SRC_DICTIONARY_ACCEPTOR_PHF_HPP_

// NOTE: PHF stands for Perfect Hash Function

#include <algorithm>
#include <utf8.h>
#include <cmph.h>
#include <nparse/nparse.hpp>
#include "dictionary.hpp"
#include "combinator.hpp"

namespace dictionary {

template <typename M_>
class AcceptorPHF: public anta::Acceptor<M_>
{
	typedef AcceptorPHF<M_> self_type;
	typedef dictionary::Dictionary dict_type;
	typedef typename anta::string<M_>::type string_type;
	typedef signed int size_type;

private:
	/**
	 *	Required by CMPH interface.
	 *	@{ */

	static int read (void* a_this, char** a_key, cmph_uint32* a_key_size)
	{
		self_type* inst = static_cast<self_type*>(a_this);
		if (inst -> m_cursor == inst -> m_dict. end())
			return 0;

		inst -> m_max_size = std::max<size_type>(
				inst -> m_max_size,
				inst -> m_cursor -> first. size() );

		*a_key = const_cast<char*>(
				inst -> m_cursor -> first. data() );

		*a_key_size = static_cast<cmph_uint32>(
				inst -> m_cursor -> first. size() );

		++ (inst -> m_cursor);

		return static_cast<int>(*a_key_size);
	}

	static void dispose (void*, char*, cmph_uint32)
	{
	}

	static void rewind (void* a_this)
	{
		self_type* inst = static_cast<self_type*>(a_this);
		inst -> m_cursor = inst -> m_dict. begin();
	}

	/**	@} */

private:
	/**
	 *	Hash ordering functor.
	 */
	class hash_less
	{
		const self_type& m_cmph;

	public:
		hash_less (const self_type& a_cmph):
			m_cmph (a_cmph)
		{
		}

		unsigned int hash (const std::string& a_key) const
		{
			return m_cmph. hash(a_key. data(), a_key. size());
		}

		bool operator() (const dict_type::value_type& u,
				const dict_type::value_type& v) const
		{
			return hash(u. first) < hash(v. first);
		}

	};

	static unsigned int no_index ()
	{
		return static_cast<unsigned int>(-1);
	}

	unsigned int hash (const char* a_data, const unsigned a_size) const
	{
		if (! m_hash. empty())
		{
			return static_cast<unsigned int>(cmph_search_packed(
				const_cast<char*>(m_hash. data()), a_data, a_size ));
		}
		return no_index();
	}

	unsigned int index (const char* a_data, const unsigned a_size) const
	{
		const unsigned int at = hash(a_data, a_size);
		if (at < m_dict. size())
		{
			dict_type::const_reference entry = m_dict[at];
			if	(	entry. first. size() == a_size
				&&	strncmp(entry. first. data(), a_data, a_size) == 0
				)
			{
				return at;
			}
		}
		return no_index();
	}

	void compile ()
	{
		cmph_io_adapter_t adapter;
		cmph_config_t* config = NULL;
		cmph_t* cmph = NULL;

		// prepare dictionary iterator
		m_cursor = m_dict. begin();
		adapter. data = static_cast<void*>(this);
		adapter. nkeys = static_cast<cmph_uint32>(m_dict. size());
		adapter. read =& this->read;
		adapter. dispose =& this->dispose;
		adapter. rewind =& this->rewind;

		// configure & build the pefrect hash function
		config = cmph_config_new(&adapter);
		cmph_config_set_algo(config, CMPH_CHD);
		cmph = cmph_new(config);
		cmph_config_destroy(config);

		// compress hash function and store it as a string
		m_hash. resize( cmph_packed_size(cmph) );
		cmph_pack(cmph, &* m_hash. begin());
		cmph_destroy(cmph);

		// sort dictionary entries in hash order
		std::sort(m_dict. begin(), m_dict. end(), hash_less(*this));
	}

public:
	// Overridden from Acceptor<M_>:

	void accept (const typename anta::range<M_>::type& C,
			const typename anta::range<M_>::type& E,
			typename anta::spectrum<M_>::type& S) const
	{
		char buf[128];
		char* out = &* buf;

		typedef typename anta::iterator<M_>::type iterator_type;
		const iterator_type j_max = std::min<iterator_type>(
				E. second + m_max_size, C. second);
		iterator_type j = E. second;

		while (j != j_max)
		{
			out = utf8::unchecked::append(*j, out);
			++ j;

			const unsigned int at = index(&* buf, out - &* buf);
			if (at == no_index())
				continue;

			anta::State<M_>* state = NULL;
			for (Combinator i(m_dict[at]); ! i. end(); ++ i)
			{
				if (i. pitch())
				{
					state = S. push(E. second, j);
				}
				state -> ref(i. key(), S, true) = i. value();
			}

			if (state == NULL)
			{
				S. push(E. second, j);
			}
		}
	}

public:
	AcceptorPHF (const std::string& a_filename):
		m_max_size (0)
	{
		const int err_line = m_dict. load(a_filename);
		if (err_line != 0)
		{
			using namespace nparse;
			throw ex::syntax_error()
				<< ex::file(a_filename)
				<< ex::line(err_line)
				<< ex::message("invalid dictionary entry");
		}

		compile();
	}

private:
	dict_type m_dict;
	std::string m_hash;

	dict_type::const_iterator m_cursor;
	size_type m_max_size;

};

} // namespace dictionary

#endif /* SRC_DICTIONARY_ACCEPTOR_PHF_HPP_ */
