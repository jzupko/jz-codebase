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

#include <jz_networking/IPAddress.h>
#include <jz_networking/Socket.h>
#include <jz_system/Thread.h>

#if JZ_PLATFORM_WINDOWS
#   include <WinSock2.h>
#endif

namespace jz
{
    namespace networking
    {

        Socket::Socket()
            : mAddress(IPAddress::Create()), mEvent(null), mReferenceCount(0u), mSocket(INVALID_SOCKET), mpThread(null), mbDone(false)
        {}

        Socket::Socket(const IPAddress& aAddress)
            : mAddress(IPAddress::Create()), mEvent(null), mReferenceCount(0u), mSocket(INVALID_SOCKET), mpThread(null), mbDone(false)
        {
            Open(aAddress);
        }

        Socket::~Socket()
        {
            Close();
        }

        bool Socket::Open(const IPAddress& aAddress)
        {
            Close();

#           if JZ_PLATFORM_WINDOWS
                mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (mSocket == INVALID_SOCKET) { return false; }

                SOCKADDR_IN addr;
                memset(&addr, 0, sizeof(SOCKADDR_IN));
                addr.sin_addr.s_addr = htonl(aAddress.Address);
                addr.sin_family = AF_INET;
                addr.sin_port = htons(aAddress.Port);

                if (bind(mSocket, (SOCKADDR*)(&addr), sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
                {
                    return false;
                }

                mAddress = aAddress;

                mEvent = WSACreateEvent();
                WSAEventSelect(mSocket, mEvent, FD_READ | FD_WRITE);

                mbDone = false;
                mpThread = new system::Thread(tr1::bind(&Socket::_TheadFunc, this));
                return true;
#           endif
        }

        void Socket::Close()
        {
            if (IsOpen())
            {
#               if JZ_PLATFORM_WINDOWS
                    mbDone = true;
                    while (!mpThread->bDone());
                    SafeDelete(mpThread);

                    WSACloseEvent(mEvent);

                    mAddress = IPAddress::Create();

                    closesocket(mSocket);
                    mSocket = INVALID_SOCKET;
#               endif
            }
        }

        bool Socket::IsOpen() const
        {
            return (mSocket != INVALID_SOCKET);
        }

        void Socket::_TheadFunc()
        {
            while (!mbDone)
            {
#               if JZ_PLATFORM_WINDOWS
                    DWORD d = WSAWaitForMultipleEvents(1, (&mEvent), false, 100u, false);

                    WSANETWORKEVENTS e;
                    if (WSAEnumNetworkEvents(mSocket, mEvent, &e) == 0)
                    {
                        if (e.lNetworkEvents & FD_READ) { _Receive(); }
                        if (e.lNetworkEvents & FD_WRITE) { _Send(); }
                    }
#               endif
            }
        }

        static const size_t kMaxPacket = 512u;

        void Socket::_Receive()
        {
#           if JZ_PLATFORM_WINDOWS
                
                char buf[kMaxPacket];

                SOCKADDR_IN addr;
                memset(&addr, 0, sizeof(SOCKADDR_IN));
                addr.sin_addr.s_addr = INADDR_ANY;
                addr.sin_family = AF_INET;
                addr.sin_port = 0;

                DWORD bytesReceived = 0u;
                INT len = sizeof(SOCKADDR_IN);
                DWORD flags = 0u;

                WSABUF data;
                data.buf = buf;
                data.len = kMaxPacket;
                if (WSARecvFrom(mSocket, &data, 1, &bytesReceived, &flags, (SOCKADDR*)(&addr), &len, null, null) == 0)
                {
                    system::Lock lock(mMutex);
                    const size_t kStart = mReceiveBuffer.size();
                    mReceiveBuffer.resize(mReceiveBuffer.size() + bytesReceived);
                    const size_t kEnd = mReceiveBuffer.size();

                    memcpy((&(mReceiveBuffer[kStart])), buf, (kEnd - kStart));
                }
#           endif
        }

        void Socket::_Send()
        {
            system::Lock lock(mMutex);

            SOCKADDR_IN addr;
            memset(&addr, 0, sizeof(SOCKADDR_IN));
            addr.sin_addr.s_addr = htonl(mAddress.Address);
            addr.sin_family = AF_INET;
            addr.sin_port = htons(mAddress.Port);

            const size_t kSize = mSendBuffer.size();
            DWORD bytesSent = 0u;
            for (size_t i = 0u; i < kSize; i += bytesSent)
            {
                WSABUF data;
                data.buf = (&(mSendBuffer[i]));
                data.len = Min((kSize - i), kMaxPacket);

                bytesSent = 0u;
                WSASendTo(mSocket, &data, 1, &bytesSent, 0, (SOCKADDR*)(&addr), sizeof(SOCKADDR_IN), null, null);
            }
        }

    }
}
