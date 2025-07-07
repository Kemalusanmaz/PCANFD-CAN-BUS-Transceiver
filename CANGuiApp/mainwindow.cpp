#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->receiver_stop->setEnabled(false);
    ui->receiver_freeze->setEnabled(false);
    ui->receiver_addFilter->setEnabled(false);
    ui->receiver_deleteAllFilters->setEnabled(false);
    ui->receiver_receiveMessages->setEnabled(false);
    ui->receiver_getDeviceConfiguration->setEnabled(false);
    ui->receiver_setDeviceConfiguration->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_configReceive;
    delete m_receive;
    delete m_transmit;
    delete m_receiveTimer;
}

QString MainWindow::captureStdOut(const std::function<void ()> &func)
{
    // buffer for cout
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();

    // object to catching outputs
    std::ostringstream strCout;

    // forward cout outputs to stream object
    std::cout.rdbuf(strCout.rdbuf());

    try {
        func();
    } catch (...) {
        ui->receiver_logDisplay->appendPlainText("Unknown Error!");
    }
    //replace cout to original buffer
    std::cout.rdbuf(oldCoutStreamBuf);

    return QString::fromStdString(strCout.str());
}

void MainWindow::on_receiver_start_clicked()
{
    m_configReceive = new CANConfiguraton();

    QString output = captureStdOut([&]() {
        m_configReceive->initialize(m_configReceive->getJsonData()["receiverDeviceName"]);
        m_configReceive->initialize(m_configReceive->getJsonData()["transmitterDeviceName"]);
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }

    m_receive = new CANReceive(m_configReceive->getFd());
    m_transmit = new CANTransmit(m_configReceive->getFd());
    m_receive->deleteAllFilters();

    ui->receiver_stop->setEnabled(true);
    ui->receiver_getDeviceConfiguration->setEnabled(true);
    ui->receiver_setDeviceConfiguration->setEnabled(true);
    ui->receiver_addFilter->setEnabled(true);
    ui->receiver_start->setEnabled(false);
}

void MainWindow::on_receiver_stop_clicked()
{
    if (m_receiveTimer && m_receiveTimer->isActive()) {
        m_receiveTimer->stop();
    }

    if(m_receive->getFilters().size() != 0) {
        m_receive->deleteAllFilters();
    }

    QString output = captureStdOut([&]() {
        m_configReceive->terminate();
    });

    ui->receiver_start->setEnabled(true);
    ui->receiver_stop->setEnabled(false);
    ui->receiver_setDeviceConfiguration->setEnabled(false);
    ui->receiver_getDeviceConfiguration->setEnabled(false);
    ui->receiver_receiveMessages->setEnabled(false);
    ui->receiver_addFilter->setEnabled(false);

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_setDeviceConfiguration_clicked()
{
    ui->receiver_receiveMessages->setEnabled(true);
    ui->receiver_setDeviceConfiguration->setEnabled(false);
    ui->receiver_addFilter->setEnabled(true);

    int clock = ui->receiver_clock->value();
    int nominalBitRate = ui->receiver_nominalBitRate->value();
    int dataBitrate= ui->receiver_dataBitRate->value();

    QString output = captureStdOut([&]() {
        m_configReceive->setCanConfig(1, clock, nominalBitRate, dataBitrate);
        // m_configReceive->setCanConfig(0);
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_getDeviceConfiguration_clicked()
{
    QString output = captureStdOut([&]() {
        m_configReceive->getCanConfig();
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_receiveMessages_clicked()
{
    // Butonun Start/Stop işlevi görmesini sağlayalım.

    // Eğer zamanlayıcı zaten oluşturulmadıysa, oluşturalım.
    if (!m_receiveTimer) {
        m_receiveTimer = new QTimer(this);
        // Zamanlayıcının timeout sinyalini, mesajları kontrol eden slotumuza bağlıyoruz.
        connect(m_receiveTimer, &QTimer::timeout, this, &MainWindow::checkForCanMessages);
    }

    // Zamanlayıcı çalışıyor mu?
    if (m_receiveTimer->isActive()) {
        // --- STOP ---
        m_receiveTimer->stop();
        // ui->receiver_receiveMessages->setText("Receive Messages");
        // ui->receiver_logDisplay->appendPlainText("--- Message receiving stopped. ---");

        // Diğer butonları tekrar aktif et
        ui->receiver_setDeviceConfiguration->setEnabled(true);
        ui->receiver_addFilter->setEnabled(true);
        ui->receiver_freeze->setEnabled(false);
    } else {
        // Diğer butonları pasif yap
        ui->receiver_setDeviceConfiguration->setEnabled(false);
        ui->receiver_addFilter->setEnabled(true);
        ui->receiver_freeze->setEnabled(true);

        // Zamanlayıcıyı her 100 milisaniyede bir çalışacak şekilde başlat.
        m_receiveTimer->start(100);
    }
}

void MainWindow::on_receiver_addFilter_clicked()
{
    ui->receiver_deleteAllFilters->setEnabled(true);

    QString output = captureStdOut([&]() {
        std::string idFrom = ui->receive_idFrom->toPlainText().toStdString();
        std::string idTo = ui->receive_idTo->toPlainText().toStdString();
        std::string flag = ui->receive_flags->currentText().toStdString();

        m_receive->addFilter(idFrom, idTo, flag);
        m_receive->addMsgFiltersList();
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_deleteAllFilters_clicked()
{
    QString output = captureStdOut([&]() {
        if(m_receive->m_filters.size() != 0){
            m_receive->deleteAllFilters();
        }
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::checkForCanMessages()
{
    if (!m_receive) return;
    QString output = captureStdOut([&]() {
        m_receive->receiveMessage();
    });

    if (!output.isEmpty()) {
        ui->receiver_receiveMessagesDisplay->appendPlainText(output);
    }
}

void MainWindow::on_transmitter_start_clicked()
{

}

void MainWindow::on_transmitter_stop_clicked()
{

}

void MainWindow::on_transmitter_setDeviceConfiguration_clicked()
{

}

void MainWindow::on_transmitter_getDeviceConfiguration_clicked()
{

}
