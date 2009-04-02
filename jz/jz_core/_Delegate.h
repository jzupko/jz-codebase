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

#if !defined(JZ_DELEGATE_PARAMS) || !defined(JZ_DELEGATE_ARGS) || !defined(JZ_DELEGATE_T_ARGS)  || !defined(JZ_DELEGATE_T)  || !defined(JZ_DELEGATE_COMMA)
#   error "Please define all necessary macros before including this file."
#endif

template <typename R JZ_DELEGATE_COMMA JZ_DELEGATE_T>
class Delegate<R (JZ_DELEGATE_T_ARGS)>;

template <typename R JZ_DELEGATE_COMMA JZ_DELEGATE_T>
class DelegateImpl<R JZ_DELEGATE_COMMA JZ_DELEGATE_T_ARGS>
{
    public:
        template <R (*F)(JZ_DELEGATE_T_ARGS)>
        static DelegateImpl Bind()
        {
            return DelegateImpl(&CallFunction<F>, 0);
        }
        
        template <typename T, R (T::*M)(JZ_DELEGATE_T_ARGS)>
        static DelegateImpl Bind(T* apObject)
        {
            return DelegateImpl(&CallObject<T, M>, apObject);
        }
        
    protected:
        friend Delegate<R (JZ_DELEGATE_T_ARGS)>;
        
        typedef R (*Caller)(void* apObject JZ_DELEGATE_COMMA JZ_DELEGATE_ARGS);

        Caller mpCaller;
        void*  mpObject;

        DelegateImpl()
            : mpCaller(0), mpObject(0)
        {}

        DelegateImpl(Caller apCaller, void* apObject)
            : mpCaller(apCaller), mpObject(apObject)
        {}
        
        template <R (*F)(JZ_DELEGATE_T_ARGS)>
        static R CallFunction(void* apObject JZ_DELEGATE_COMMA JZ_DELEGATE_ARGS)
        {
            return (F)(JZ_DELEGATE_PARAMS);
        }
        
        template <typename T, R (T::*M)(JZ_DELEGATE_T_ARGS)>
        static R CallObject(void* apObject JZ_DELEGATE_COMMA JZ_DELEGATE_ARGS)
        {
            return (static_cast<T*>(apObject)->*M)(JZ_DELEGATE_PARAMS);
        }
};

template <typename R JZ_DELEGATE_COMMA JZ_DELEGATE_T>
class Delegate<R (JZ_DELEGATE_T_ARGS)> : public DelegateImpl<R JZ_DELEGATE_COMMA JZ_DELEGATE_T_ARGS>
{
    public:
        Delegate()
        {}
        
        Delegate(const Delegate& d)
            : DelegateImpl(d.mpCaller, d.mpObject)
        {}
        
        Delegate(const DelegateImpl& d)
            : DelegateImpl(d.mpCaller, d.mpObject)
        {}

        Delegate& operator=(const Delegate& d)
        {
            mpCaller = d.mpCaller;
            mpObject = d.mpObject;
            
            return *this;
        }
        
        Delegate& operator=(const DelegateImpl& d)
        {
            mpCaller = d.mpCaller;
            mpObject = d.mpObject;
            
            return *this;
        }
        
        operator bool() const
        {
            return mpCaller != 0;
        }

        R operator()(JZ_DELEGATE_ARGS) const
        {
            return (*mpCaller)(mpObject JZ_DELEGATE_COMMA JZ_DELEGATE_PARAMS);
        }

        void Reset()
        {
            mpCaller = 0;
            mpObject = 0;
        }        
};

#undef JZ_DELEGATE_PARAMS
#undef JZ_DELEGATE_ARGS
#undef JZ_DELEGATE_T_ARGS
#undef JZ_DELEGATE_T
#undef JZ_DELEGATE_COMMA
