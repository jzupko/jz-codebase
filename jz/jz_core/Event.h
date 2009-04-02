//
// Copyright (c) 2009 Joseph A. Zupko
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once
#ifndef _JZ_EVENT_H_
#define _JZ_EVENT_H_

#include <jz_core/Auto.h>
#include <jz_core/Delegate.h>
#include <jz_core/Utility.h>

namespace jz
{

    // the return type of a delegate assigned to an event
    // must always be a bool. If a delegate returns true,
    // the event is considered to be handled and the Event()
    // call returns immediately without calling any other
    // delegates in its list.
    template <typename R, typename A1 = DummyType, typename A2 = DummyType, typename A3 = DummyType, typename A4 = DummyType, typename Dummy = DummyType> class EventImpl;
    template <typename F> class Event;

    // R()
#   define JZ_EVENT_COMMA
#   define JZ_EVENT_T      
#   define JZ_EVENT_T_ARGS 
#   define JZ_EVENT_ARGS   
#   define JZ_EVENT_PARAMS 
#   include <jz_core/_Event.h>

    // R(A1)
#   define JZ_EVENT_COMMA  ,
#   define JZ_EVENT_T      typename A1
#   define JZ_EVENT_T_ARGS A1
#   define JZ_EVENT_ARGS   A1 a1
#   define JZ_EVENT_PARAMS a1
#   include <jz_core/_Event.h>   

    // R(A1, A2)
#   define JZ_EVENT_COMMA  ,
#   define JZ_EVENT_T      typename A1, typename A2
#   define JZ_EVENT_T_ARGS A1, A2
#   define JZ_EVENT_ARGS   A1 a1, A2 a2
#   define JZ_EVENT_PARAMS a1, a2
#   include <jz_core/_Event.h>   

    // R(A1, A2, A3)
#   define JZ_EVENT_COMMA  ,
#   define JZ_EVENT_T      typename A1, typename A2, typename A3
#   define JZ_EVENT_T_ARGS A1, A2, A3
#   define JZ_EVENT_ARGS   A1 a1, A2 a2, A3 a3
#   define JZ_EVENT_PARAMS a1, a2, a3
#   include <jz_core/_Event.h>    

    // R(A1, A2, A3, A4)
#   define JZ_EVENT_COMMA  ,
#   define JZ_EVENT_T      typename A1, typename A2, typename A3, typename A4
#   define JZ_EVENT_T_ARGS A1, A2, A3, A4
#   define JZ_EVENT_ARGS   A1 a1, A2 a2, A3 a3, A4 a4
#   define JZ_EVENT_PARAMS a1, a2, a3, a4
#   include <jz_core/_Event.h>

}

#endif