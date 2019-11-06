#include "remotedevice.h"
#include "drv/driver.h"

namespace bth
{
    RemoteDevice::IOStatus RemoteDevice::readMessage()
    {
        if (remainingBytes <= 0)
            remainingBytes = sizeof(msg::DriverMessage);

        int offset = sizeof(msg::DriverMessage) - remainingBytes;
        char* buffer = (char*) &messageBuffer + offset;

        int bytes = recv(socket, buffer, remainingBytes, 0);
        if (bytes == 0) // <-- socket disconnected
        {
            remainingBytes = 0;
            return IO_ERROR;
        }

        remainingBytes -= bytes;
        if (remainingBytes == 0)
        {
            switch (messageBuffer.msgType)
            {
            case msg::SET_REPORT:
                driver->handleMessage(messageBuffer);
                break;
            }
            return IO_COMPLETED;
        }

        return IO_PENDING;
    }

    RemoteDevice::IOStatus RemoteDevice::sendMessage(const msg::DeviceMessage& message)
    {
        const msg::DeviceMessage* data = &message;
        int bytesToSend = sizeof(msg::DeviceMessage);

        while (bytesToSend)
        {
            int bytesSent = send(socket, (const char*) data, bytesToSend, 0);
            if (bytesSent == SOCKET_ERROR)
            {
                return IO_ERROR;
            }
            bytesToSend -= bytesSent;
            data += bytesSent;
        }

        return IO_COMPLETED;
    }
}
