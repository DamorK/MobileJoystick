#ifndef MESSAGES_H
#define MESSAGES_H

#define _WINSOCKAPI_
#include <Windows.h>

namespace msg
{
    enum MessageType
    {
        SET_REPORT = 1,
        GET_REPORT = 2
    };

    struct DriverMessage // Remote device --> [Application -->] Driver
    {
        BYTE msgType;
        BYTE x;
        BYTE y;
        BYTE buttons;
    };

    struct DeviceMessage // Application --> Remote device
    {
        DeviceMessage(BYTE type)
            : msgType(type)
        {}

        BYTE msgType;
    };
}

#endif // MESSAGES_H
