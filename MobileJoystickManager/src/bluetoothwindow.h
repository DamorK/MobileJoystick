#ifndef BLUETOOTHWINDOW_H
#define BLUETOOTHWINDOW_H

#include <QDialog>
#include <QList>
#include <WinSock2.h>
#include <ws2bth.h>

namespace Ui
{
    class BluetoothWindow;
}
namespace drv
{
    class Driver;
}
namespace bth
{
    class BluetoothDiscoverer;
    class BluetoothIO;
}

// =============================
// BluetoothWindow
// =============================

class BluetoothWindow : private QDialog
{
    Q_OBJECT
    
public:
    explicit BluetoothWindow(QWidget *parent = 0);
    ~BluetoothWindow();

    void init(bth::BluetoothIO* btio, drv::Driver* drv);

private slots:
    void scanClicked();
    void okClicked();
    void discoveringFinished(const QString& errorMsg);
    void remoteDeviceFound(const QString& deviceName, const QString& deviceAddr, const SOCKADDR_BTH& addr);
    
private:
    Ui::BluetoothWindow *ui;
    bth::BluetoothDiscoverer* btDisc;
    QList<SOCKADDR_BTH> btAddr;

    bth::BluetoothIO* btIO;
    drv::Driver* driver;
};

#endif // BLUETOOTHWINDOW_H
