#include "bluetoothio.h"
#include "remotedevice.h"
#include "utl/logger.h"
#include "msg/messages.h"

namespace bth
{
BluetoothIO::BluetoothIO()
{
}

BluetoothIO::~BluetoothIO()
{
    clear();
}

bool BluetoothIO::addConnection(PSOCKADDR_BTH addr, const QString& devName, drv::Driver* drv)
{
    SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

    if (s == INVALID_SOCKET)
    {
        WLOGGER << L"Unable to create socket: " << ::GetLastError() << std::endl;
        return false;
    }

    if (::connect(s, (sockaddr*) addr, sizeof(SOCKADDR_BTH)))
    {
        WLOGGER << L"Unable to connect with remote device: " << ::GetLastError() << std::endl;
        closesocket(s);
        return false;
    }

    RemoteDevice* device = new RemoteDevice(s, devName, drv);
    if (device->sendMessage(msg::DeviceMessage(msg::GET_REPORT)) == RemoteDevice::IO_ERROR)
    {
        WLOGGER << L"Sending initial message failed" << std::endl;
        delete device;
        return false;
    }

    devicesMutex.lock();
    devices.push_back(device);
    if (devices.size() == 1) // first element -> start second thread for IO
    {
        this->start();
    }
    devicesMutex.unlock();

    emit connectionAdded(drv, devName);
    return true;
}

void BluetoothIO::disconnect(drv::Driver *drv)
{
    QMutexLocker lock(&devicesMutex);
    QMutableListIterator<RemoteDevice*> it(devices);
    while (it.hasNext())
    {
        RemoteDevice* dev = it.next();
        if (dev->getDriver() == drv)
        {
            delete dev;
            it.remove();
        }
    }
}

void BluetoothIO::run()
{
    fd_set sockets;
    while (fillSocketSet(&sockets))
    {
        if (select(0, &sockets, NULL, NULL, NULL) == SOCKET_ERROR)
        {
            WLOGGER << "select() operation failed: " << ::GetLastError() << std::endl;
            break;
        }

        processIO(&sockets);
    }
}

bool BluetoothIO::fillSocketSet(fd_set *set)
{
    QMutexLocker lock(&devicesMutex);
    if (devices.size() > 0)
    {
        FD_ZERO(set);
        foreach(RemoteDevice* dev, devices)
        {
            FD_SET(dev->getSocket(), set);
        }
        return true;
    }
    return false;
}

void BluetoothIO::processIO(fd_set *set)
{
    QMutexLocker lock(&devicesMutex);
    QMutableListIterator<RemoteDevice*> it(devices);
    while (it.hasNext())
    {
        RemoteDevice* dev = it.next();
        if (FD_ISSET(dev->getSocket(), set))
        {
            RemoteDevice::IOStatus status = dev->readMessage();

            if (status == RemoteDevice::IO_COMPLETED)
            {
                status = dev->sendMessage(msg::DeviceMessage(msg::GET_REPORT));
            }

            if (status == RemoteDevice::IO_ERROR)
            {
                emit connectionInterrupted(dev->getDriver());
                delete dev;
                it.remove();
            }
        }
    }
}

void BluetoothIO::clear()
{
    QMutexLocker lock(&devicesMutex);
    foreach (RemoteDevice* dev, devices)
    {
        delete dev;
    }
    devices.clear();
}

} // namespace
