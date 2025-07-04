#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_config = new CANConfiguraton();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QString output = captureStdOut([&]() {
        m_config->initialize(m_config->getJsonData()["receiverDeviceName"]);
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_stop_clicked()
{
    QString output = captureStdOut([&]() {
        m_config->terminate();
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_setDeviceConfiguration_clicked()
{
    int clock = ui->receiver_clock->value();
    int nominalBitRate = ui->receiver_nominalBitRate->value();
    int dataBitrate= ui->receiver_dataBitRate->value();

    QString output = captureStdOut([&]() {
        m_config->setCanConfig(1, clock, nominalBitRate, dataBitrate);
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_getDeviceConfiguration_clicked()
{
    QString output = captureStdOut([&]() {
        m_config->getCanConfig();
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}
