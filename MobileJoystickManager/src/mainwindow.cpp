#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testwindow.h"
#include "bluetoothwindow.h"
#include "bth/bluetoothio.h"
#include "drv/drivermanager.h"
#include <QStandardItem>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    testWindow(new TestWindow),
    bluetoothWindow(new BluetoothWindow),
    btIO(new bth::BluetoothIO),
    driverManager(new drv::DriverManager)
{
    ui->setupUi(this);

    connect(ui->actionTest, SIGNAL(triggered()), this, SLOT(testDriverTriggered()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectTriggered()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectTriggered()));
    connect(btIO, SIGNAL(connectionAdded(const drv::Driver*,QString)), this, SLOT(connectionAdded(const drv::Driver*,QString)));
    connect(btIO, SIGNAL(connectionInterrupted(const drv::Driver*)), this, SLOT(connectionInterrupted(const drv::Driver*)));

    QStandardItemModel* model = new QStandardItemModel(ui->colDrivers);
    model->setColumnCount(3);
    model->setHorizontalHeaderItem(0, new QStandardItem("Driver name"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Status"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Phone name"));
    ui->colDrivers->setModel(model);
    ui->colDrivers->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // find driver
    driverManager->findDriver();
    QList<QStandardItem*> row;

    for (int i = 0; i < driverManager->getInstances().size(); i++)
    {
        row.append(new QStandardItem("Mobile Joystick"));
        row.append(new QStandardItem("UNCONNECTED"));
        row.append(new QStandardItem(""));
        model->appendRow(row);
        row.clear();
    }
}

MainWindow::~MainWindow()
{
    delete btIO;
    delete driverManager;
    delete bluetoothWindow;
    delete testWindow;
    delete ui;
}

void MainWindow::setDriverInfo(int deviceIndex, const QString &status, const QString &remoteDevice)
{
    QStandardItemModel* model = (QStandardItemModel*) ui->colDrivers->model();
    if (deviceIndex >= 0 && deviceIndex < model->rowCount())
    {
        model->item(deviceIndex, 1)->setText(status);
        model->item(deviceIndex, 2)->setText(remoteDevice);
    }
}

void MainWindow::connectionAdded(const drv::Driver *drv, const QString &devName)
{
    int index = driverManager->getDriverIndex(drv);
    setDriverInfo(index, "CONNECTED", devName);
}

void MainWindow::connectionInterrupted(const drv::Driver *drv)
{
    int index = driverManager->getDriverIndex(drv);
    setDriverInfo(index, "UNCONNECTED", "");
}

void MainWindow::testDriverTriggered()
{
    QItemSelectionModel* selection = ui->colDrivers->selectionModel();

    if (selection->selectedRows().size() == 0)
    {
        QMessageBox::critical(NULL, "Error", "You have to select a device", QMessageBox::Ok);
        return;
    }

    int row = selection->selectedRows().front().row();
    if (row < driverManager->getInstances().size())
        testWindow->init(driverManager->getInstances()[row]);
}

void MainWindow::connectTriggered()
{
    QItemSelectionModel* selection = ui->colDrivers->selectionModel();

    if (selection->selectedRows().size() == 0)
    {
        QMessageBox::critical(NULL, "Error", "You have to select a device", QMessageBox::Ok);
        return;
    }

    int row = selection->selectedRows().front().row();
    if (row < driverManager->getInstances().size())
        bluetoothWindow->init(btIO, driverManager->getInstances()[row]);
}

void MainWindow::disconnectTriggered()
{
    QItemSelectionModel* selection = ui->colDrivers->selectionModel();

    if (selection->selectedRows().size() == 0)
    {
        QMessageBox::critical(NULL, "Error", "You have to select a device", QMessageBox::Ok);
        return;
    }

    int row = selection->selectedRows().front().row();
    if (row < driverManager->getInstances().size())
    {
        drv::Driver* driver = driverManager->getInstances()[row];
        btIO->disconnect(driver);
        setDriverInfo(row, "UNCONNECTED", "");
    }
}
