#include <jz_core/Tree.h>
#include <jz_test/Tests.h>

namespace tut
{

    DUMMY(TestsTree);

    using namespace jz;

    struct TestTree : public TreeNode<TestTree>
    {
    public:
        TestTree() { msCount++; }
        ~TestTree() { msCount--; }
        
        static size_t GetCount() { return msCount; }
        
    private:
        friend size_t jz::__GetRefCount<TestTree>(TestTree*);
        friend void jz::__IncrementRefCount<TestTree>(TestTree*);
        friend void jz::__DecrementRefCount<TestTree>(TestTree*);

        static size_t msCount;
    };
    
    size_t TestTree::msCount = 0;

    template<> template<>
    void Object::test<1>()
    {
        AutoPtr<TestTree> pTree = new TestTree();
        AutoPtr<TestTree> pTree2(new TestTree());
        
        ensure_equals(TestTree::GetCount(), 2);
        
        pTree = new TestTree();

        ensure(pTree.IsValid());        
        ensure_equals(TestTree::GetCount(), 2);
        
        AutoPtr<TestTree> p(pTree);
        ensure(p.IsValid());
        ensure(p == pTree);
        
        for (natural i = 0; i < 100; i++)
        {
            AutoPtr<TestTree> pChild(new TestTree());
            pChild->SetParent(p.Get());
            p = &(p->front());
            ensure(p.IsValid());
            ensure(p.Get() != null);

            ensure(p == pChild);
            ensure(p.Get() == pChild.Get());
            
            ensure_equals(p.GetReferenceCount(), 3);
            ensure(!p.IsUnique());
        }
        
        ensure_equals(TestTree::GetCount(), 102);
        
        pTree->SetParent(null);
        
        pTree.Reset();
        ensure_equals(TestTree::GetCount(), 2);
        p.Reset();
        ensure_equals(TestTree::GetCount(), 1);
        pTree2.Reset();
        ensure_equals(TestTree::GetCount(), 0);
    }
    
}
