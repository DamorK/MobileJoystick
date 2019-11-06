#include "bluetoothwindow.h"
#include "ui_bluetoothwindow.h"
#include "bth/bluetoothdiscoverer.h"
#include "bth/bluetoothio.h"

#include <QStandardItemModel>
#include <QMessageBox>

Q_DECLARE_METATYPE(SOCKADDR_BTH)

BluetoothWindow::BluetoothWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BluetoothWindow),
    btDisc(new bth::BluetoothDiscoverer)
{
    ui->setupUi(this);

    qRegisterMetaType<SOCKADDR_BTH>("SOCKADDR_BTH");
    connect(ui->btnScan, SIGNAL(clicked()), this, SLOT(scanClicked()));
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(btDisc, SIGNAL(discoveringFinished(QString)),
            this, SLOT(discoveringFinished(QString)),
            Qt::QueuedConnection);
    connect(btDisc, SIGNAL(remoteDeviceFound(QString,QString,SOCKADDR_BTH)),
            this, SLOT(remoteDeviceFound(QString,QString,SOCKADDR_BTH)),
            Qt::QueuedConnection);

    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QStandardItemModel* model = new QStandardItemModel(ui->colDiscoveredDevices);
    model->setColumnCount(2);
    model->setHorizontalHeaderItem(0, new QStandardItem("Device name"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Address"));
    ui->colDiscoveredDevices->setModel(model);
    ui->colDiscoveredDevices->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

BluetoothWindow::~BluetoothWindow()
{
    delete btDisc;
    delete ui;
}

void BluetoothWindow::init(bth::BluetoothIO *btio, drv::Driver *drv)
{
    this->btIO = btio;
    this->driver = drv;
    this->show();
}

void BluetoothWindow::scanClicked()
{
    if (!btDisc->isRunning())
    {
        ui->btnScan->setEnabled(false);
        ui->label->setText("Discovering remote devices...");
        ui->colDiscoveredDevices->model()->removeRows(0, ui->colDiscoveredDevices->model()->rowCount());
        btAddr.clear();
        btDisc->start();
    }
}

void BluetoothWindow::okClicked()
{
    QStandardItemModel* model = (QStandardItemModel*) ui->colDiscoveredDevices->model();
    QItemSelectionModel* selModel = ui->colDiscoveredDevices->selectionModel();

    if (selModel->selectedRows().size() == 0)
    {
        QMessageBox::critical(NULL, "Error", "You have to select a device", QMessageBox::Ok);
        return;
    }

    int row = selModel->selectedRows().front().row();
    QString deviceName = QString("%1 %2").arg(
                model->item(row, 0)->text(),
                model->item(row, 1)->text());

    if (!btIO->addConnection(&btAddr[row], deviceName, driver))
    {
        QMessageBox::critical(NULL, "Error", "Connection failed", QMessageBox::Ok);
        return;
    }

    this->hide();
}

void BluetoothWindow::discoveringFinished(const QString& errorMsg)
{
    if (errorMsg.size() > 0)
    {
        QMessageBox::critical(NULL, "Error", errorMsg, QMessageBox::Ok);
    }
    ui->label->setText("");
    ui->btnScan->setEnabled(true);
}

void BluetoothWindow::remoteDeviceFound(const QString& deviceName, const QString& deviceAddr, const SOCKADDR_BTH& addr)
{
    QStandardItemModel* model = (QStandardItemModel*) ui->colDiscoveredDevices->model();
    QList<QStandardItem*> row;
    row.append(new QStandardItem(deviceName));
    row.append(new QStandardItem(deviceAddr));
    model->appendRow(row);
    ui->colDiscoveredDevices->update();
    btAddr.append(addr);
}
