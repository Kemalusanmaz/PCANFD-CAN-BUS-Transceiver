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
    void on_receiver_receiveMessages_clicked();
    void on_receiver_addFilter_clicked();
    void on_receiver_deleteAllFilters_clicked();
    void checkForCanMessages(); // Zamanlayıcının tetikleyeceği yeni slot

    void on_transmitter_start_clicked(); // on and clicked is a signal name
    void on_transmitter_stop_clicked();
    void on_transmitter_setDeviceConfiguration_clicked();
    void on_transmitter_getDeviceConfiguration_clicked();

private:
    Ui::MainWindow *ui;
    //Backend koddaki nesneler oluşturulur
    CANConfiguraton *m_configReceive;
    CANConfiguraton *m_configTransmit;
    CANReceive *m_receive;
    CANTransmit *m_transmit;
    QTimer* m_receiveTimer; // Zamanlayıcı için bir pointer
};
#endif // MAINWINDOW_H
