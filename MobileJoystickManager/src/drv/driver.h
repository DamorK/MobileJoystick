#ifndef DRIVER_H
#define DRIVER_H

#include "msg/messages.h"

namespace drv
{
    class Driver
    {
    public:
        Driver(HANDLE h, BYTE devIndex)
            : handle(h)
            , deviceIndex(devIndex)
        {}

        void handleMessage(const msg::DriverMessage& message);

    private:
        HANDLE handle;
        BYTE deviceIndex;
    };
}

#endif // DRIVER_H
