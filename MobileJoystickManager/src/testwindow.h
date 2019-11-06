#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QDialog>

namespace Ui
{
    class TestWindow;
}

namespace drv
{
    class Driver;
}

// =============================
// TestWindow
// =============================

class TestWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit TestWindow(QWidget *parent = 0);
    ~TestWindow();

    void init(drv::Driver* drv);
    
private slots:
    void updateDriver();

private:
    Ui::TestWindow *ui;
    drv::Driver* driver;
};

#endif // TESTWINDOW_H
