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

#if !defined(JZ_EVENT_PARAMS) || !defined(JZ_EVENT_ARGS) || !defined(JZ_EVENT_T_ARGS)  || !defined(JZ_EVENT_T)  || !defined(JZ_EVENT_COMMA)
#   error "Please define all necessary macros."
#endif

template <typename R JZ_EVENT_COMMA JZ_EVENT_T>
class Event<R (JZ_EVENT_T_ARGS)>;

template <typename R JZ_EVENT_COMMA JZ_EVENT_T>
class EventImpl<R JZ_EVENT_COMMA JZ_EVENT_T_ARGS>
{
public:
    typedef Delegate<R (JZ_EVENT_T_ARGS)> EntryDelegate;

    class Entry sealed
    {
        public:
            Entry()
                : mppHead(null), mppTail(null), mpPrevious(null), mpNext(null)
#               ifndef NDEBUG
                , mbInDelegate(false)
#               endif
            {}

            ~Entry()
            {
                Reset();
            }

            void Reset()
            {
                JZ_ASSERT(!mbInDelegate);

                if (mpPrevious) { mpPrevious->mpNext = mpNext; }
                if (mpNext) { mpNext->mpPrevious = mpPrevious; }
                
                if (mppHead && *mppHead == this) { *mppHead = mpNext; }
                if (mppTail && *mppTail == this) { *mppTail = mpPrevious; }

                mppHead = null;
                mppTail = null;
                mpPrevious = null;
                mpNext = null;
                mDelegate.Reset();
            }
            
       private:
            Entry(const Entry&);
            Entry& operator=(const Entry&);

            void _Set(const EntryDelegate& d, Entry** appHead, Entry** appTail, Entry* apPrevious)
            {
                JZ_ASSERT(!mbInDelegate);
                
                mDelegate = d;
                mppHead = appHead;
                mppTail = appTail;
                mpPrevious = apPrevious;
                mpNext = null;

                if (mpPrevious) { mpPrevious->mpNext = this; }
            }

            friend ConstInternalListIterator<Entry>;
            friend InternalListIterator<Entry>;
            friend Event<R (JZ_EVENT_T_ARGS)>;
            friend EventImpl;
            
            const Entry* GetNext() const { return mpNext; }
            Entry* GetNext() { return mpNext; }
            
            const Entry* GetPrevious() const { return mpPrevious; }
            Entry* GetPrevious() { return mpPrevious; }
            
            Entry** mppHead;
            Entry** mppTail;
            Entry* mpPrevious;
            Entry* mpNext;
            EntryDelegate mDelegate;

#           ifndef NDEBUG
                bool mbInDelegate;
#           endif
    };

    protected:
        typedef ConstInternalListIterator<Entry> const_iterator;
        typedef InternalListIterator<Entry> iterator;
        
        const_iterator begin() const { return mpHead; }
        iterator begin() { return mpHead; }
        
        const_iterator end() const { return null; }
        iterator end() { return null; }
            
    public:
        static void InPlaceConstruct(EventImpl& e)
        {
            e.mpHead = null;
            e.mpTail = null;
        }

        EventImpl()
            : mpHead(null), mpTail(null)
        {}
        
        ~EventImpl()
        {
            for (iterator I = begin(); I != end(); I++)
            {
                I->mppHead = null;
                I->mppTail = null;
            }
        }
        
        operator bool() const
        {
            return (mpHead != null);
        }
        
        void Add(const EntryDelegate& d, Entry& e)
        {
            e.Reset();
            e._Set(d, &mpHead, &mpTail, mpTail);

            if (!mpHead) { mpHead = &e; }
            mpTail = &e;
        }

        template <R (*F)(JZ_EVENT_T_ARGS)>
        void Add(Entry& e)
        {
            EntryDelegate d(EntryDelegate::Bind<F>());
            
            Add(d, e);
        }

        template <typename T, R (T::*M)(JZ_EVENT_T_ARGS)>
        void Add(T* apObject, Entry& e)
        {
            EntryDelegate d(EntryDelegate::Bind<T, M>(apObject));
            
            Add(d, e);
        }

    private:
        EventImpl(const EventImpl& e);
        EventImpl& operator=(const EventImpl& e);

        Entry* mpHead;
        Entry* mpTail;
    };

template <typename R JZ_EVENT_COMMA JZ_EVENT_T>
class Event<R (JZ_EVENT_T_ARGS)> : public EventImpl<R JZ_EVENT_COMMA JZ_EVENT_T_ARGS>
{
public:
    void operator()(JZ_EVENT_ARGS) const
    {
        for (const_iterator I = begin(); I != end(); I++)
        {
#           ifndef NDEBUG
            const_cast<bool&>(I->mbInDelegate) = true;
#           endif

            (I->mDelegate)(JZ_EVENT_PARAMS);

#           ifndef NDEBUG
            const_cast<bool&>(I->mbInDelegate) = false;
#           endif
        }
    }
};

template <JZ_EVENT_T>
class Event<bool (JZ_EVENT_T_ARGS)> : public EventImpl<bool JZ_EVENT_COMMA JZ_EVENT_T_ARGS>
{
public:
    void operator()(JZ_EVENT_ARGS) const
    {
        for (const_iterator I = begin(); I != end(); I++)
        {
#           ifndef NDEBUG
            const_cast<bool&>(I->mbInDelegate) = true;
#           endif

            if ((I->mDelegate)(JZ_EVENT_PARAMS))
            {
#               ifndef NDEBUG
                const_cast<bool&>(I->mbInDelegate) = false;
#               endif

                return;
            }
            
#           ifndef NDEBUG
            const_cast<bool&>(I->mbInDelegate) = false;
#           endif
        }
    }
};

#undef JZ_EVENT_PARAMS
#undef JZ_EVENT_ARGS
#undef JZ_EVENT_T_ARGS
#undef JZ_EVENT_T
#undef JZ_EVENT_COMMA
