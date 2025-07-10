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

    // 2. Başlangıç Validator'ını oluştur.
    // Başlangıçta "STD" (11-bit) seçili.
    // Düzenli ifade: 1 ile 3 arasında hexadecimal karakter (büyük/küçük harf duyarsız).
    // 7FF -> 3 karakter.
    QRegularExpression rx29bit("^(0[xX])?[0-9a-fA-F]{1,8}$");
    m_hexIdValidator = new QRegularExpressionValidator(rx29bit, this);

    // 3. Validator'ı QLineEdit'a ata.
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
    // Butonun Start/Stop işlevi görmesini sağlayalım.
    ui->receiver_receiveMessages->setEnabled(false);
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

        // Diğer butonları tekrar aktif et
        ui->receiver_setDeviceConfiguration->setEnabled(true);
        ui->receiver_addFilter->setEnabled(true);
    } else {
        // Diğer butonları pasif yap
        ui->receiver_setDeviceConfiguration->setEnabled(false);
        ui->receiver_addFilter->setEnabled(true);

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

    // --- 1. GİRDİ KONTROLÜ VE FORMATLAMA ---
    QString text = currentItem->text().toUpper();
    QRegularExpression hexMatcher("^[0-9A-F]{1,2}$");

    if (!hexMatcher.match(text).hasMatch()) {
        currentItem->setText("00"); // Hatalıysa '00' yap
    } else {
        // Eğer tek karakter girildiyse, başına '0' ekle (örn: F -> 0F)
        if (text.length() == 1) {
            text = "0" + text;
        }
        currentItem->setText(text);
    }
    int currentIndex = row*8 + column;
    int lastIndex = m_lastEditedRow * 8 + m_lastEditedCol;
    if(currentIndex > lastIndex) {
        m_lastEditedRow = row;
        m_lastEditedCol = column;
    }

    // m_lastEditedRow = row;
    // m_lastEditedCol = column;
    // --- 2. VERİ TOPLAMA ---
    // İçeriği değişen her hücreden sonra tüm string'i yeniden oluştur.
    updateConcatenatedHex();

    // --- 3. OTOMATİK İLERLEME (Kullanıcı Deneyimi İçin) ---
    // 'Enter' tuşuna basıldığında bir sonraki hücreye geçişi sağlar.
    // Artık hiçbir hücreyi kilitlemiyoruz veya açmıyoruz!
    int nextRow = row;
    int nextColumn = column + 1;
    if (nextColumn >= ui->transmitter_data->columnCount()) {
        nextColumn = 0;
        nextRow++;
    }

    // İzin verilen hücre sınırını aşmıyorsak ve hala tablo içindeysek ilerle
    int maxCells = (ui->transmitter_messageType->currentText() == "CAN 2.0") ? 8 : 64;
    if ((nextRow * 8 + nextColumn) < maxCells) {
        ui->transmitter_data->setCurrentCell(nextRow, nextColumn);
    }

    // Sinyalleri tekrar aç
    ui->transmitter_data->blockSignals(false);
}

void MainWindow::updateConcatenatedHex()
{
    m_concatenatedHex.clear();
    QStringList hexParts;

    // Eğer hiçbir hücre düzenlenmediyse (program ilk açıldığında veya reset sonrası)
    // m_lastEditedCol -1 olduğu için bu döngü hiç çalışmaz ve string boş kalır. Bu doğru.
    for (int i = 0; i <= (m_lastEditedRow * 8 + m_lastEditedCol); ++i) {
        int row = i / 8;
        int col = i % 8;

        QTableWidgetItem* item = ui->transmitter_data->item(row, col);
        if (item) {
            hexParts.append(item->text());
        } else {
            // Bu durum normalde oluşmamalı ama güvenlik için
            hexParts.append("00");
        }
    }

    m_concatenatedHex = hexParts.join(" ");
    ui->transmitter_dataDisplay->setPlainText(m_concatenatedHex);
}

// Bu fonksiyon tabloyu ilk haline getirir.
void MainWindow::setupHexTable()
{
    ui->transmitter_data->blockSignals(true);

    ui->transmitter_data->setRowCount(8);
    ui->transmitter_data->setColumnCount(8);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem("-");
            item->setTextAlignment(Qt::AlignCenter);
            // setFlags ile ilgili HİÇBİR şey yapmıyoruz.
            // Bırakalım varsayılan olarak hepsi düzenlenebilir olsun.
            ui->transmitter_data->setItem(row, col, item);
        }
    }

    ui->transmitter_data->blockSignals(false);
}

// Bu fonksiyon hem tabloyu hem de veri değişkenlerini sıfırlar.
void MainWindow::resetHexTable()
{
    // Veri string'ini temizle
    m_concatenatedHex.clear();
    ui->transmitter_dataDisplay->clear(); // Görünen ekranı da temizle

    // Takip değişkenlerini sıfırla
    m_lastEditedRow = 0;
    m_lastEditedCol = -1; // -1 ile başlar, böylece ilk hücre (0,0) girildiğinde sınır doğru hesaplanır.

    // Tabloyu başlangıç durumuna getir.
    // Bu, tüm hücreleri "00" yapar ve düzenlenebilir bırakır.
    setupHexTable();

    // ŞİMDİ, mevcut messageType seçimine göre hücreleri ayarla.
    // on_..._currentIndexChanged'ı manuel olarak çağırarak bunu yapabiliriz.
    on_transmitter_messageType_currentIndexChanged(ui->transmitter_messageType->currentIndex());
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
