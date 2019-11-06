#include "initwinsock.h"
#include <QMutex>

namespace bth
{
    enum WSADataStatus
    {
        WSADATA_NOT_INITIALIZED,
        WSADATA_INITIALIZED,
        WSADATA_ERROR
    };

    WSADataStatus wsaDataStatus = WSADATA_NOT_INITIALIZED;
    WSADATA wsaData;
    QMutex wsMutex;

    bool initWinSock()
    {
        QMutexLocker lock(&wsMutex);

        if (wsaDataStatus == WSADATA_ERROR)
            return false;

        if (wsaDataStatus == WSADATA_INITIALIZED)
            return true;

        if (WSAStartup(0x202, &wsaData) == 0)
        {
            wsaDataStatus = WSADATA_INITIALIZED;
            atexit(cleanWinSock);
            return true;
        }

        wsaDataStatus = WSADATA_ERROR;
        return false;
    }

    void cleanWinSock()
    {
        WSACleanup();
    }
}
