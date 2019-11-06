#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class TestWindow;
class BluetoothWindow;

namespace Ui
{
    class MainWindow;
}
namespace bth
{
    class BluetoothIO;
}
namespace drv
{
    class Driver;
    class DriverManager;
}

// =============================
// MainWindow
// =============================

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    void setDriverInfo(int deviceIndex, const QString& status, const QString& remoteDevice);

    Ui::MainWindow *ui;
    TestWindow* testWindow;
    BluetoothWindow* bluetoothWindow;
    bth::BluetoothIO* btIO;
    drv::DriverManager* driverManager;

private slots:
    void connectionAdded(const drv::Driver* drv, const QString& devName);
    void connectionInterrupted(const drv::Driver* drv);
    void testDriverTriggered();
    void connectTriggered();
    void disconnectTriggered();
};

#endif // MAINWINDOW_H
