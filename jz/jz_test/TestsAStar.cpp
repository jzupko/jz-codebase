#include <jz_pathfinding/AStar.h>
#include <jz_test/Tests.h>

namespace tut
{

    DUMMY(TestsAStar);

    using namespace jz;
    using namespace jz::pathfinding;

    static float EstDistance(u16 x, u16 y, void_p apUserData)
    {
        JZ_ASSERT(y == 4u);

        switch (x)
        {
        case 1u: return 4.0f; break;
        case 2u: return 2.0f; break;
        case 3u: return 4.0f; break;
        case 4u: return 0.0f; break;
        case 5u: return 2.0f; break;
        case 6u: return 4.5f; break;
        default:
            JZ_ASSERT(false);
            return (Constants<float>::kMax);
            break;
        }
    }

    static float AdjDistance(u16 x, u16 y, void_p apUserData)
    {
        if (x > y) { Swap(x, y); }

        if (x == 0u && y == 1u) { return 1.5f; }
        else if (x == 0u && y == 6u) { return 2.0f; }
        else if (x == 1u && y == 2u) { return 2.0f; }
        else if (x == 2u && y == 3u) { return 3.0f; }
        else if (x == 3u && y == 4u) { return 4.0f; }
        else if (x == 4u && y == 5u) { return 2.0f; }
        else if (x == 5u && y == 6u) { return 3.0f; }

        JZ_ASSERT(false);
        return (Constants<float>::kMax);
    }

    template<> template<>
    void Object::test<1>()
    {
        vector<u16> adj(AStar<u16, 3>::kMaxNodes);
        for (size_t i = 0u; i < AStar<u16, 3>::kMaxNodes; i++) { adj[i] = AStar<u16, 3>::kNullNode; }

        adj[(0*3)+0] = 1u; adj[(0*3)+1] = 6u;
        adj[(1*3)+0] = 0u; adj[(1*3)+1] = 2u;
        adj[(2*3)+0] = 1u; adj[(2*3)+1] = 3u;
        adj[(3*3)+0] = 2u; adj[(3*3)+1] = 4u;
        adj[(4*3)+0] = 3u; adj[(4*3)+1] = 5u;
        adj[(5*3)+0] = 4u; adj[(5*3)+1] = 6u;
        adj[(6*3)+0] = 5u; adj[(6*3)+1] = 0u;

        vector<u16> path;
        ensure(AStar<u16, 3>::FindPath(0, 4, adj, EstDistance, AdjDistance, path)); 
        ensure(path.size() == 3u);
        if (path.size() == 3u)
        {
            ensure(path[0] == 6u);
            ensure(path[1] == 5u);
            ensure(path[2] == 4u);
        }
    }
    
}
