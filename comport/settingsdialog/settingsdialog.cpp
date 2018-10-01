//------------------------------------------------------------------------------
//     Данный модуль создает виджет окна
//     с параметрами и настройкой COM порта.
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "settingsdialog.h"

#include "ui_settingsdialog.h"

#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QString>


static const QString isBusy = QObject::tr("Занят");
static const QString noBusy = QObject::tr("Свободен");
//-----------------------------------------------------------
// Назначение: консруктор класса
//-----------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);

      ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

      connect(ui->applyButton, SIGNAL(clicked()),
              this, SLOT(apply()));
      connect(ui->serialPortInfoBox, SIGNAL(currentIndexChanged(int)),
              this, SLOT(showPortInfo(int)));
      connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
              this, SLOT(checkCustomBaudRatePolicy(int)));
      connect(ui->updateInfoButton,SIGNAL(clicked()),
              this, SLOT( fillPortsInfo()));

      fillPortsParameters();
      fillPortsInfo();

      updateSettings();
}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
SettingsDialog::~SettingsDialog()
{
    delete ui;
}
//-----------------------------------------------------------
// Назначение: возвращает текущие настройки
//-----------------------------------------------------------
SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}
//-----------------------------------------------------------
// Назначение: отображение информации о COM порте
//-----------------------------------------------------------
void SettingsDialog::showPortInfo(int idx)
{
    if (idx != -1) {
        QStringList list = ui->serialPortInfoBox->itemData(idx).toStringList();
        ui->descriptionLabel->setText(tr("Описание: %1").arg(list.at(1)));
        ui->manufacturerLabel->setText(tr("Производитель: %1").arg(list.at(2)));
        ui->serialNumberLabel->setText(tr("Сер.номер: %1").arg(list.at(3)));
        ui->locationLabel->setText(tr("Размещение: %1").arg(list.at(4)));
        ui->vidLabel->setText(tr("ID произв.: %1").arg(list.at(5)));
        ui->pidLabel->setText(tr("ID : %1").arg(list.at(6)));
        if(list.at(7)==isBusy)
            ui->busyLabel->setText(tr("Состояние: <font color=red>%1</font color>").arg(list.at(7)));
        else
            ui->busyLabel->setText(tr("Состояние: <font color=green>%1</font color>").arg(list.at(7)));
        //ui->busyLabel->setStyleSheet("QLabel{color:red}");
    }
}
//-----------------------------------------------------------
// Назначение: действие по кнопке Apply
//-----------------------------------------------------------
void SettingsDialog::apply()
{
    updateSettings();
    hide();
    emit isUpdateSettings();
}
//-----------------------------------------------------------
// Назначение: установка нестандартных скоротей COM порта
//-----------------------------------------------------------
void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setStyleSheet(" background-color: #232629;"
                            " padding: 5px;"
                            "border-style: solid;"
                            "border: 0px solid #76797C;"
                            "border-radius: 0px;"
                            "color: #eff0f1;");
       edit->setValidator(intValidator);
    }
}
//-----------------------------------------------------------
// Назначение: отображение настраиваемых параметров COM
//-----------------------------------------------------------
void SettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    ui->baudRateBox->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->baudRateBox->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(QStringLiteral("Custom"));

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(QStringLiteral("None"),  QSerialPort::NoParity);
    ui->parityBox->addItem(QStringLiteral("Even"),  QSerialPort::EvenParity);
    ui->parityBox->addItem(QStringLiteral("Odd"),   QSerialPort::OddParity);
    ui->parityBox->addItem(QStringLiteral("Mark"),  QSerialPort::MarkParity);
    ui->parityBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);
}
//-----------------------------------------------------------
// Назначение: отображение информации о COM порте
//-----------------------------------------------------------
void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoBox->clear();
    static const QString blankString = QObject::tr("N/A");


    QString description;
    QString manufacturer;
    QString serialNumber;
    QString busy;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        //info.isBusy()?busy(tr("isBusy")):busy(tr("noBusy"));
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString)
             << (info.isBusy() ? isBusy : noBusy);
        ui->serialPortInfoBox->addItem(list.first(), list);
    }
}

void SettingsDialog::showWidget()
{
    qApp->processEvents();
    fillPortsInfo();
    show();

}
//-----------------------------------------------------------
// Назначение: обновление(запись) текущих настроек
//-----------------------------------------------------------
void SettingsDialog::updateSettings()
{
    currentSettings.name = ui->serialPortInfoBox->currentText();

    if (ui->baudRateBox->currentIndex() == 7) {
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        currentSettings.baudRate =/* static_cast<QSerialPort::BaudRate>*/(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
}

