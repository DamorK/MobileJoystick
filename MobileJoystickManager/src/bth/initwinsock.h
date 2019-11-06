#ifndef INITWINSOCK_H
#define INITWINSOCK_H

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

namespace bth
{
    bool initWinSock();
    void cleanWinSock();
}

#endif // INITWINSOCK_H
