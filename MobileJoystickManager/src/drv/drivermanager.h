#ifndef DRIVERMANAGER_H
#define DRIVERMANAGER_H

#define _WINSOCKAPI_
#include <Windows.h>
#include <QList>

namespace drv
{
    class Driver;

    class DriverManager
    {
    public:
        DriverManager();
        ~DriverManager();

        void findDriver();
        void clear();
        int getDriverIndex(const Driver* driver);

        const QList<Driver*>& getInstances()
        {
            return instances;
        }

    private:
        HANDLE openControlDevice(LPWSTR devicePath);

        HANDLE fileHandle;
        QList<Driver*> instances;
    };
}

#endif // DRIVERMANAGER_H
