#include <jz_core/Auto.h>
#include <jz_core/Memory.h>
#include <jz_test/Tests.h>

namespace tut
{

    DUMMY(TestsAuto);

    using namespace jz;

    struct TestObject 
    {
    public:
        TestObject() : mReferenceCount(0u) { msCount++; }
        ~TestObject() { msCount--; }
        
        static size_t GetCount() { return msCount; }
        
        const AutoPtr<TestObject>& GetObject() const { return mpObject; }
        void SetObject(const AutoPtr<TestObject>& apObject) { mpObject = apObject; }
        
    private:
        size_t mReferenceCount;

        friend size_t jz::__GetRefCount<TestObject>(TestObject*);
        friend void jz::__IncrementRefCount<TestObject>(TestObject*);
        friend void jz::__DecrementRefCount<TestObject>(TestObject*);

        AutoPtr<TestObject> mpObject;
        static size_t msCount;
    };
    
    size_t TestObject::msCount = 0;

    template<> template<>
    void Object::test<1>()
    {
        AutoPtr<TestObject> pObject;
        
        ensure(!pObject.IsValid());
        ensure_equals(TestObject::GetCount(), 0);
        
        pObject = new TestObject();

        ensure(pObject.IsValid());        
        ensure_equals(TestObject::GetCount(), 1);
        ensure(!pObject->GetObject().IsValid());
        
        AutoPtr<TestObject> p(pObject);
        ensure(p.IsValid());
        ensure(p == pObject);
        
        for (natural i = 0; i < 100; i++)
        {
            p->SetObject(new TestObject());
            p = p->GetObject();
            ensure(p.IsValid());
            ensure_equals(p.GetReferenceCount(), 2);
            ensure(p.Get() != 0);
            ensure(!p.IsUnique());
        }
        
        ensure_equals(TestObject::GetCount(), 101);
        
        pObject.Reset();
        ensure_equals(TestObject::GetCount(), 1);
        p.Reset();
        ensure_equals(TestObject::GetCount(), 0);
    }
    
}
