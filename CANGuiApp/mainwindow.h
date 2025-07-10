#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../configuration/include/configuration.hpp"
#include "../configuration/include/canChannelSettings.hpp"
#include "../receiver/include/receive.hpp"
#include "../transmitter/include/transmit.hpp"
class QRegularExpressionValidator;

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

     // İçerik değiştiğinde veri toplama ve doğrulama için
    void on_transmitter_data_cellChanged(int row, int column);
    void on_transmitter_send_clicked();
    void on_transmitter_setDeviceConfiguration_clicked();
    void on_transmitter_getDeviceConfiguration_clicked();
    void on_transmitter_flags_currentIndexChanged(int index);
    void on_transmitter_messageType_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    //Backend koddaki nesneler oluşturulur
    CANConfiguraton *m_configReceive;
    CANConfiguraton *m_configTransmit;
    CANReceive *m_receive;
    CANTransmit *m_transmit;
    QTimer* m_receiveTimer; // Zamanlayıcı için bir pointer
    CanChannelSettings *m_settingsReceive;
    CanChannelSettings *m_settingsTransmit;

    void setupHexTable(); // Tabloyu ilk kuran fonksiyon
    void resetHexTable(); // Tabloyu ve veriyi sıfırlayan fonksiyon
    // O ana kadar birleştirilmiş hex string'ini tutar
    void updateConcatenatedHex();
    // Kullanıcının en son hangi hücrede olduğunu takip etmek için
    int m_lastEditedRow = 0;
    int m_lastEditedCol = -1; // -1 ile başlatalım ki ilk hücre (0,0) doğru işlensin
    QString m_concatenatedHex;

    // QLineEdit için bir tamsayı doğrulayıcısı
    QRegularExpressionValidator *m_hexIdValidator;
};
#endif // MAINWINDOW_H
