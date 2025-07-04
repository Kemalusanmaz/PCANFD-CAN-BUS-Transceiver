#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../configuration/include/configuration.hpp"
#include "../receiver/include/receive.hpp"
#include "../transmitter/include/transmit.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString captureStdOut(const std::function<void()>& func);

private slots: // Functions that capture events write these blocks as slot functions.
    void on_receiver_start_clicked(); // on and clicked is a signal name
    void on_receiver_stop_clicked();
    void on_receiver_setDeviceConfiguration_clicked();
    void on_receiver_getDeviceConfiguration_clicked();

private:
    Ui::MainWindow *ui;
    //Backend koddaki nesneler oluşturulur
    CANConfiguraton *m_config;
    CANReceive *m_receive;
    CANTransmit *m_transmit;
};
#endif // MAINWINDOW_H
