#include "testwindow.h"
#include "ui_testwindow.h"
#include "drv/driver.h"
#define _WINSOCKAPI_
#include <Windows.h>

TestWindow::TestWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestWindow),
    driver(NULL)
{
    ui->setupUi(this);
    connect(ui->xValue, SIGNAL(valueChanged(int)), this, SLOT(updateDriver()));
    connect(ui->yValue, SIGNAL(valueChanged(int)), this, SLOT(updateDriver()));
    connect(ui->btn1, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btn2, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btn3, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btn4, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btn5, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btn6, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btn7, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btn8, SIGNAL(clicked()), this, SLOT(updateDriver()));
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(hide()));

    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
}

TestWindow::~TestWindow()
{
    delete ui;
}

void TestWindow::init(drv::Driver* drv)
{
    this->driver = drv;
    this->ui->xValue->setValue(0);
    this->ui->yValue->setValue(0);
    this->ui->btn1->setChecked(false);
    this->ui->btn2->setChecked(false);
    this->ui->btn3->setChecked(false);
    this->ui->btn4->setChecked(false);
    this->ui->btn5->setChecked(false);
    this->ui->btn6->setChecked(false);
    this->ui->btn7->setChecked(false);
    this->ui->btn8->setChecked(false);
    this->show();
}

#define BTN_STATE(btn, bit) ((btn)->isChecked() ? (1 << (bit)) : 0)

void TestWindow::updateDriver()
{
    if (driver)
    {
        msg::DriverMessage message;
        message.msgType = msg::SET_REPORT;
        message.x = ui->xValue->value();
        message.y = ui->yValue->value();
        message.buttons = 0;
        message.buttons |= BTN_STATE(ui->btn1, 0);
        message.buttons |= BTN_STATE(ui->btn2, 1);
        message.buttons |= BTN_STATE(ui->btn3, 2);
        message.buttons |= BTN_STATE(ui->btn4, 3);
        message.buttons |= BTN_STATE(ui->btn5, 4);
        message.buttons |= BTN_STATE(ui->btn6, 5);
        message.buttons |= BTN_STATE(ui->btn7, 6);
        message.buttons |= BTN_STATE(ui->btn8, 7);

        driver->handleMessage(message);
    }
}
