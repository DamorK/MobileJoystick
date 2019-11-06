#ifndef BLUETOOTHDISCOVERER_H
#define BLUETOOTHDISCOVERER_H

#include <QThread>
#include <QString>
#include <WinSock2.h>
#include <ws2bth.h>

namespace bth
{

    class BluetoothDiscoverer : public QThread
    {
        Q_OBJECT

    public:
        virtual void run();

    signals:
        void discoveringFinished(const QString& errorMsg);
        void remoteDeviceFound(const QString& deviceName, const QString& deviceAddr, const SOCKADDR_BTH& addr);

    private:
        void discoverDevices();
        void discoverServices(LPWSTR deviceName, LPSOCKET_ADDRESS remoteAddr);
        void emitErrorMsg(const wchar_t* caption);
    };

}

#endif // BLUETOOTHDISCOVERER_H
