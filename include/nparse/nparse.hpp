/*
 * @file $/include/nparse/nparse.hpp
 *
This file is a part of the "nParse" project -
        a general purpose parsing framework, version 0.1.2

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
#ifndef NPARSE_NPARSE_HPP_
#define NPARSE_NPARSE_HPP_

//		Flag:						// Enables:
#define UNICODE						// - wide internal strings (utf-16/32)
#define ANTA_NDL_STACKING			// - push/pop operations on contexts
#define ANTA_NDL_MUTABLE_ANCESTOR	// - context ancestor manipulation

#include "_version.hpp"

// [ contributed ]
#include <plugin/plugin.hpp>
#include <encode/encode.hpp>
#include <anta/ndl.hpp>
#include <anta/dsel.hpp>
#include <anta/_aux/variable.hpp>

// [ specializations for the nParse script grammar model ]
#include "specs/sg/model.hpp"
#include "specs/sg/string.hpp"
#include "specs/sg/context_value.hpp"
#include "specs/sg/rcopy.hpp"
#include "specs/sg/label.hpp"
#include "specs/sg/observer.hpp"

// [ specializations for the natural language grammar model ]
#include "specs/nlg/model.hpp"
#if defined(NPARSE_SWAP_FILE)
#include "specs/nlg/pool.hpp" //optional
#endif
#include "specs/nlg/string.hpp"
#include "specs/nlg/context_key.hpp" // optional
#include "specs/nlg/context_value.hpp"
#include "specs/nlg/joint_base.hpp"
#include "specs/nlg/rcopy.hpp"

// [ plug debug print observer ]
#include <anta/_aux/debug_print.hpp>
#if defined(DEBUG_PRINT)
ANTA_AUX_DEBUG_PRINT_OBSERVER(nparse::NLG)
#endif

// [ nParse library specific ]
#include "exceptions.hpp"
#include "interfaces.hpp"
#include "functions.hpp"
#include "markers.hpp"

// These files have to be included at the end of the list in order to resolve
// some recursive type definitions.
#include "specs/nlg/action.hpp"
#include "environment.hpp"

// @todo: move this section somewhere
namespace nparse {

inline IStaging::joint_pointer::joint_pointer(
		const anta::Acceptor<NLG>& a_acceptor, const anta::Label<NLG>& a_label):
	base_type ()
{
	reset(new anta::ndl::JointJump<NLG>(a_acceptor, a_label));
}

inline IStaging::joint_pointer::joint_pointer (
		const anta::ndl::Cluster<NLG>& a_cluster):
	base_type ()
{
	reset(new anta::ndl::JointCall<NLG>(a_cluster));
}

} // namespace nparse

#endif /* NPARSE_NPARSE_HPP_ */
