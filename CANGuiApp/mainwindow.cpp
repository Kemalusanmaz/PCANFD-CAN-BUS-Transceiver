#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QTimer>
#include <QRegularExpressionValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->receiver_stop->setEnabled(false);
    ui->receiver_addFilter->setEnabled(false);
    ui->receiver_deleteAllFilters->setEnabled(false);
    ui->receiver_showFilters->setEnabled(false);
    ui->receiver_receiveMessages->setEnabled(false);
    ui->receiver_getDeviceConfiguration->setEnabled(false);
    ui->receiver_setDeviceConfiguration->setEnabled(false);
    resetHexTable();

    connect(ui->transmitter_data, &QTableWidget::cellChanged,
            this, &MainWindow::on_transmitter_data_cellChanged);

    ui->transmitter_stop->setEnabled(false);
    ui->transmitter_getDeviceConfiguration->setEnabled(false);
    ui->transmitter_setDeviceConfiguration->setEnabled(false);
    ui->transmitter_send->setEnabled(false);

    // Create validator for ID field.
    // STD is selected in beginnig.
    // Expression means hex char between 1 to 3. (Upper/Lower case insensitive)
    QRegularExpression rx29bit("^(0[xX])?[0-9a-fA-F]{1,8}$");
    m_hexIdValidator = new QRegularExpressionValidator(rx29bit, this);

    // Declare validator to ID Widget
    ui->transmitter_id->setValidator(m_hexIdValidator);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_configReceive;
    delete m_receive;
    delete m_transmit;
    delete m_receiveTimer;
    delete m_settingsReceive;
    delete m_settingsTransmit;
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
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }

    m_receive = new CANReceive(m_configReceive->getFd());
    m_settingsReceive = new CanChannelSettings();
    m_receive->deleteAllFilters();

    ui->receiver_stop->setEnabled(true);
    ui->receiver_getDeviceConfiguration->setEnabled(true);
    ui->receiver_setDeviceConfiguration->setEnabled(true);
    ui->receiver_addFilter->setEnabled(true);
    ui->receiver_showFilters->setEnabled(true);
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
    ui->receiver_showFilters->setEnabled(false);

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_setDeviceConfiguration_clicked()
{
    ui->receiver_receiveMessages->setEnabled(true);
    ui->receiver_setDeviceConfiguration->setEnabled(false);
    ui->receiver_addFilter->setEnabled(true);
    ui->receiver_showFilters->setEnabled(true);

    int clock = ui->receiver_clock->value();
    int nominalBitRate = ui->receiver_nominalBitRate->value();
    int dataBitrate= ui->receiver_dataBitRate->value();

    m_settingsReceive->setClock(clock);
    m_settingsReceive->setNominalBitRate(nominalBitRate);
    m_settingsReceive->setDataBitRate(nominalBitRate);
    QString output = captureStdOut([&]() {
        m_configReceive->setCanConfig(*m_settingsReceive);
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
    ui->receiver_receiveMessages->setEnabled(false);
    // Create timer
    if (!m_receiveTimer) {
        m_receiveTimer = new QTimer(this);
        // timeout signal of timer is connected chechForCanMessages
        connect(m_receiveTimer, &QTimer::timeout, this, &MainWindow::checkForCanMessages);
    }

    // Check Timer is active
    if (m_receiveTimer->isActive()) {
        // --- STOP ---
        m_receiveTimer->stop();

        // Activate buttons
        ui->receiver_setDeviceConfiguration->setEnabled(true);
        ui->receiver_addFilter->setEnabled(true);
    } else {
        // Deactivate buttons
        ui->receiver_setDeviceConfiguration->setEnabled(false);
        ui->receiver_addFilter->setEnabled(true);

        // Start timer as it works every 100 ms
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

void MainWindow::on_receiver_showFilters_clicked()
{
    QString output = captureStdOut([&]() {
        m_receive->getFilterList();
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
    m_configTransmit = new CANConfiguraton();
    m_settingsTransmit = new CanChannelSettings();

    QString output = captureStdOut([&]() {
        m_configTransmit->initialize(m_configTransmit->getJsonData()["transmitterDeviceName"]);
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }

    m_transmit = new CANTransmit(m_configTransmit->getFd());

    ui->transmitter_stop->setEnabled(true);
    ui->transmitter_getDeviceConfiguration->setEnabled(true);
    ui->transmitter_setDeviceConfiguration->setEnabled(true);
    ui->transmitter_send->setEnabled(true);
    ui->transmitter_start->setEnabled(false);
}

void MainWindow::on_transmitter_stop_clicked()
{
    QString output = captureStdOut([&]() {
        m_configTransmit->terminate();
    });

    ui->transmitter_start->setEnabled(true);
    ui->transmitter_stop->setEnabled(false);
    ui->transmitter_setDeviceConfiguration->setEnabled(false);
    ui->transmitter_getDeviceConfiguration->setEnabled(false);

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_transmitter_setDeviceConfiguration_clicked()
{
    ui->transmitter_setDeviceConfiguration->setEnabled(false);

    int clock = ui->transmitter_clock->value();
    int nominalBitRate = ui->transmitter_nominalBitRate->value();
    int dataBitrate= ui->transmitter_dataBitRate->value();

    m_settingsTransmit->setClock(clock);
    m_settingsTransmit->setNominalBitRate(nominalBitRate);
    m_settingsTransmit->setDataBitRate(dataBitrate);
    QString output = captureStdOut([&]() {
        m_configTransmit->setCanConfig(*m_settingsTransmit);
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_transmitter_getDeviceConfiguration_clicked()
{
    QString output = captureStdOut([&]() {
        m_configTransmit->getCanConfig();
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_transmitter_data_cellChanged(int row, int column)
{
    QTableWidgetItem* currentItem = ui->transmitter_data->item(row, column);
    if (!currentItem) return;

    // Sinyal döngüsünü engellemek için sinyalleri geçici olarak kapat
    ui->transmitter_data->blockSignals(true);

    QString originalText = currentItem->text();
    QString formattedText = originalText.toUpper().trimmed();

    // 1. Kullanıcı hücreyi tamamen sildiyse veya sadece boşluk varsa
    if (formattedText.isEmpty()) {
        // Hücreyi varsayılan "boş" durumuna getir
        currentItem->setText("-");
    }
    // 2. Eğer geçerli bir hex girişi varsa
    else {
        QRegularExpression hexMatcher("^[0-9A-F]{1,2}$");
        if (hexMatcher.match(formattedText).hasMatch()) {
            // Eğer tek karakter girildiyse, başına '0' ekle
            if (formattedText.length() == 1) {
                formattedText = "0" + formattedText;
            }
            // Hücreye formatlanmış metni geri yaz (büyük harf vb.)
            currentItem->setText(formattedText);
        }
        // 3. Eğer giriş geçersizse (ne boş ne de geçerli hex)
        else {
            // Hatalı girişi geri al ve kullanıcıyı uyar
            // Hatalı giriş öncesi hücrenin durumuna geri dönmek zor olabilir.
            // En basit çözüm, onu 'boş' durumuna getirmektir.
            currentItem->setText("-");
        }
    }

    // Her durumda, değişiklikten sonra veri string'ini yeniden oluştur.
    updateConcatenatedHex();

    // Sinyalleri tekrar aç
    ui->transmitter_data->blockSignals(false);
}

void MainWindow::updateConcatenatedHex()
{
    m_concatenatedHex.clear();
    QStringList hexParts; // Geçerli hex değerlerini tutacak bir liste

    bool dataEnded = false; // Verinin bittiği yeri işaretleyen bayrak
    for (int row = 0; row < ui->transmitter_data->rowCount(); ++row) {
        for (int col = 0; col < ui->transmitter_data->columnCount(); ++col) {
            QTableWidgetItem* item = ui->transmitter_data->item(row, col);

            // Eğer hücre boş değilse ve metin geçerli bir hex ise
            if (item && item->text() != "-") {
                // Eğer daha önce veri bitmişse, bu bir "aradaki boşluk" hatasıdır.
                if (dataEnded) {
                    m_concatenatedHex.clear(); // Hatalı olduğu için string'i temizle
                    ui->transmitter_dataDisplay->setPlainText(m_concatenatedHex);
                    return; // Fonksiyondan çık
                }
                hexParts.append(item->text());
            } else {
                // Hücre boşsa veya "-" içeriyorsa, verinin burada bittiğini varsay.
                dataEnded = true;
            }
        }
    }

    m_concatenatedHex = hexParts.join(" ");
    ui->transmitter_dataDisplay->setPlainText(m_concatenatedHex);
}

// Bu fonksiyon tabloyu ilk haline getirir.
void MainWindow::setupHexTable()
{
    ui->transmitter_data->blockSignals(true);
    // ... (satır/sütun ayarlama kısmı aynı) ...
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Başlangıçta "-" veya boş bırakmak daha anlamlı
            QTableWidgetItem *item = new QTableWidgetItem("-");
            item->setTextAlignment(Qt::AlignCenter);
            ui->transmitter_data->setItem(row, col, item);
        }
    }
    ui->transmitter_data->blockSignals(false);
}

// Bu fonksiyon hem tabloyu hem de veri değişkenlerini sıfırlar.
void MainWindow::resetHexTable()
{
    m_concatenatedHex.clear();
    ui->transmitter_dataDisplay->clear();
    setupHexTable(); // Tabloyu ilk haline getirir
}

void MainWindow::on_transmitter_send_clicked()
{
    std::string messageType = ui->transmitter_messageType->currentText().toStdString();
    int messageTypeInt = 0;

    if(messageType == "CAN FD"){
        messageTypeInt = 0;
    }else if(messageType == "CAN 2.0"){
        messageTypeInt = 1;
    }
    uint32_t id = ui->transmitter_id->text().toUInt(nullptr,16);

    std::string flags = ui->transmitter_flags->currentText().toStdString();

    // 1. Gönderilecek verinin boş olup olmadığını kontrol et.
    if (m_concatenatedHex.isEmpty()) {
        return;
    }

    // 2. VERİYİ GÖNDERME İŞLEMİ
    // Burada, m_concatenatedHex string'ini alıp
    // m_transmitter nesnenizin sendMessage fonksiyonuna gönderirsiniz.
    // m_concatenatedHex "AA BB CC" formatında olduğu için önce boşlukları temizlemeniz gerekebilir.
    m_concatenatedHex = m_concatenatedHex.remove(' ');
    // dataToSend.remove(' '); // Boşlukları kaldır -> "AABBCC"

    QString output = captureStdOut([&]() {
        m_transmit->sendMessage(messageTypeInt, id, flags,
                                m_concatenatedHex.toStdString());
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }

    resetHexTable();
}

void MainWindow::on_transmitter_flags_currentIndexChanged(int index)
{
    // Seçili metni al
    QString selectedFlag = ui->transmitter_flags->itemText(index);

    if (selectedFlag == "EXT") {
        // --- 29-bit (0 - 1FFFFFFF) ---
        // Düzenli ifade: 1 ile 8 arasında hexadecimal karakter.
        // 1FFFFFFF -> 8 karakter.
        QRegularExpression rx29bit("^(0[xX])?[0-9a-fA-F]{1,8}$");

        // Validator'ın düzenli ifadesini güncelle.
        m_hexIdValidator->setRegularExpression(rx29bit);
    } else {
        // --- 11-bit (0 - 7FF) ---
        // Düzenli ifade: 1 ile 3 arasında hexadecimal karakter.
        QRegularExpression rx11bit("^(0[xX])?[0-9a-fA-F]{1,3}$");

        // Validator'ın düzenli ifadesini güncelle.
        m_hexIdValidator->setRegularExpression(rx11bit);
    }
    // Kullanıcı önceden geçersiz bir değer girmiş olabileceğinden,
    // QLineEdit'ın içeriğini temizlemek iyi bir fikirdir.
    ui->transmitter_id->clear();
}

void MainWindow::on_transmitter_messageType_currentIndexChanged(int index)
{
    QString selectedType = ui->transmitter_messageType->itemText(index);
    int editableCellCount = (selectedType == "CAN 2.0") ? 8 : 64;

    ui->transmitter_data->blockSignals(true);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QTableWidgetItem *item = ui->transmitter_data->item(row, col);
            if (!item) continue;

            int cellIndex = row * 8 + col;

            if (cellIndex < editableCellCount) {
                // Bu hücre kullanılabilir olmalı
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
                item->setBackground(QBrush(Qt::white));
            } else {
                // Bu hücre kullanılamaz olmalı
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); // Düzenlenemez
                item->setBackground(QBrush(Qt::lightGray));
            }
        }
    }

    // Değişiklik sonrası veri string'ini yeniden hesapla ve imleci başa al
    updateConcatenatedHex();
    ui->transmitter_data->setCurrentCell(0, 0);

    ui->transmitter_data->blockSignals(false);
}

void MainWindow::on_busFeatures_busState_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getCanState();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getCanState();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getDeviceId_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getDeviceId();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getDeviceId();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setDeviceId_clicked()
{
    uint32_t deviceId = ui->busFeatures_deviceId->text().toInt(nullptr,16);

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setDeviceId(deviceId);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setDeviceId(deviceId);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
    ui->busFeatures_deviceId->clear();
}

void MainWindow::on_busFeatures_getBitTimingRanges_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getBitTimingRanges();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getBitTimingRanges();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getChannelFeatures_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getChannelFeatures();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getChannelFeatures();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getAcceptanceFilter_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getAcceptanceFilter();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getAcceptanceFilter();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getFrameDelayTime_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getFrameDelayTime();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getFrameDelayTime();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getTimestampMode_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getTimeStampMode();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getTimeStampMode();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setTimestampMode_clicked()
{
    std::string tsMode = ui->busFeatures_timestampMode->currentText().toStdString();

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setHwTimestampMode(tsMode);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setHwTimestampMode(tsMode);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getDriverVersion_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getDriverVersion();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getDriverVersion();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getFirmwareVersion_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getFirmwareVersion();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getFirmwareVersion();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getInputOutputInfo_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getIoInfo();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getIoInfo();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setAllowedMessageType_clicked()
{
    std::string msgType = ui->busFeatures_allowedMessageType->currentText().toStdString();

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setAllowedMsgs(msgType);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setAllowedMsgs(msgType);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setIFrameDelay_clicked()
{
    uint32_t iFrameDelay = ui->busFeatures_iFrameDelay->text().toInt(nullptr,16);

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setIFrameDelay(iFrameDelay);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setIFrameDelay(iFrameDelay);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
    ui->busFeatures_iFrameDelay->clear();
}

void MainWindow::on_busFeatures_setMassStorageMode_clicked()
{
    std::string msgType = ui->busFeatures_massStorageMode->currentText().toStdString();

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setMassStorageMode(msgType);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setMassStorageMode(msgType);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setDriverClockRefTime_clicked()
{
    uint32_t driverClockRefTime = ui->busFeatures_driverClockRefTime->text().toInt(nullptr,16);

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setDrvClockRef(driverClockRefTime);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setDrvClockRef(driverClockRefTime);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
    ui->busFeatures_driverClockRefTime->clear();
}

void MainWindow::on_busFeatures_setLingerMode_clicked()
{
    std::string linger = ui->busFeatures_lingerMode->currentText().toStdString();

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setLinger(linger);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setLinger(linger);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setSelfAck_clicked()
{
    std::string selfAck = ui->busFeatures_selfAck->currentText().toStdString();

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setSelfAck(selfAck);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setSelfAck(selfAck);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setBRSIgnore_clicked()
{
    std::string brsIgnore = ui->busFeatures_BRSIgnore->currentText().toStdString();

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setBRSIgnore(brsIgnore);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setBRSIgnore(brsIgnore);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getSerialNumber_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getSerialNumber();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getSerialNumber();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getHWDeviceNumber_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getHCDeviceNumber();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getHCDeviceNumber();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_setHWDeviceNumber_clicked()
{
    uint8_t hwDeviceNumber = ui->busFeatures_hwDeviceNumber->text().toInt(nullptr,16);

    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->setHCDeviceNumber(hwDeviceNumber);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->setHCDeviceNumber(hwDeviceNumber);
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
    ui->busFeatures_hwDeviceNumber->clear();
}

void MainWindow::on_busFeatures_getAdapterName_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getAdapterName();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getAdapterName();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}

void MainWindow::on_busFeatures_getPartNumber_clicked()
{
    if(ui->busFeatures_transmission->currentText().toStdString() == "Receiver"){
        if(m_receive) {
            QString output = captureStdOut([&]() {
                m_configReceive->getPartNumber();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }

        }
    }else if(ui->busFeatures_transmission->currentText().toStdString() == "Transmitter"){
        if(m_transmit) {

            QString output = captureStdOut([&]() {
                m_configTransmit->getPartNumber();
            });

            if (!output.isEmpty()) {
                ui->busFeatures_logDisplay->appendPlainText(output);
            }
        }
    }
}
