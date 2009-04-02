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

#include <jz_networking/NetMan.h>
#include <jz_networking/Socket.h>

#if JZ_PLATFORM_WINDOWS
#   include <WinSock2.h>
#endif

namespace jz
{
    networking::NetMan* networking::NetMan::mspSingleton = null;

    namespace networking
    {

        NetMan::NetMan()
        {
#           if JZ_PLATFORM_WINDOWS
                WSAData data;
                WSAStartup(MAKEWORD(2, 2), &data);
#           endif
        }

        NetMan::~NetMan()
        {
#           if JZ_PLATFORM_WINDOWS
                WSACleanup();
#           endif
        }

        Socket* NetMan::CreateSocket()
        {
            return (new Socket());
        }

        Socket* NetMan::CreateSocket(const IPAddress& aAddress)
        {
            return (new Socket(aAddress));
        }

    }
}
