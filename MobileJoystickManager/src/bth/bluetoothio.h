#ifndef BLUETOOTHIO_H
#define BLUETOOTHIO_H

#include <QList>
#include <QThread>
#include <QMutex>
#include <WinSock2.h>
#include <ws2bth.h>

namespace drv
{
    class Driver;
}

namespace bth
{
    class RemoteDevice;

    class BluetoothIO : public QThread
    {
        Q_OBJECT

    public:
        BluetoothIO();
        ~BluetoothIO();

        bool addConnection(PSOCKADDR_BTH addr, const QString& devName, drv::Driver* drv);
        void disconnect(drv::Driver* drv);
        virtual void run();

    signals:
        void connectionAdded(const drv::Driver* drv, const QString& devName);
        void connectionInterrupted(const drv::Driver* drv);

    private:
        bool fillSocketSet(fd_set* set);
        void processIO(fd_set* set);
        void clear();

    private:
        QList<RemoteDevice*> devices;
        QMutex devicesMutex;
    };
}

#endif // BLUETOOTHIO_H
