#ifndef REMOTE_DEVICE_H
#define REMOTE_DEVICE_H

#include "msg/messages.h"
#include <WinSock2.h>
#include <QString>

namespace drv
{
    class Driver;
}

namespace bth
{
    class RemoteDevice
    {
    public:
        enum IOStatus
        {
            IO_COMPLETED,
            IO_PENDING,
            IO_ERROR
        };

        RemoteDevice(SOCKET s, const QString& devName, drv::Driver* drv)
            : socket(s)
            , name(devName)
            , driver(drv)
            , remainingBytes(0)
        {}

        ~RemoteDevice()
        {
            closesocket(socket);
        }

        inline SOCKET getSocket() const             { return socket; }
        inline const QString& getName() const       { return name; }
        inline const drv::Driver* getDriver() const { return driver; }

        IOStatus readMessage();
        IOStatus sendMessage(const msg::DeviceMessage& message);

        SOCKET socket;
        QString name;
        drv::Driver* driver;
        msg::DriverMessage messageBuffer;
        int remainingBytes;
    };
}

#endif // REMOTE_DEVICE_H
