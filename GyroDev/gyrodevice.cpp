#include "gyrodevice.h"

GyroDevice::GyroDevice(QWidget *parent) : QMainWindow(parent)
{
    SettingsComPort = new SettingsDialog;
    DeviceComPort = new comPort;
    CreateWidgets();
    CreateConnections();
}
//-----------------------------------------------------------
void GyroDevice::OpenSerialPort()
{
    SettingsDialog::Settings p=SettingsComPort->settings();
    emit ConnectComPort(&p);
}
//-----------------------------------------------------------
void GyroDevice::CloseSerialPort()
{
    emit DisconnectComPort();
}
//-----------------------------------------------------------
void GyroDevice::isConnectedComPort(const QString msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(false);
    OnComPortButton->setEnabled(false);
    OffComPortButton->setEnabled(true);
}
//-----------------------------------------------------------
void GyroDevice::isNotConnectedComPort(const QString msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(true);
    OnComPortButton->setEnabled(true);
    OffComPortButton->setEnabled(false);
}
//-----------------------------------------------------------
void GyroDevice::CreateWidgets()
{
    TypeProtocolComboBox=new QComboBox;
    TypeProtocolComboBox->addItem(QStringLiteral("Delta_PS"));
    TypeProtocolComboBox->addItem(QStringLiteral("Rate_2"));
    TypeProtocolComboBox->addItem(QStringLiteral("Dadvtt"));
    TypeProtocolLabel=new QLabel(tr("Тип протокола:"));
    TypeProtocolLabel->setBuddy(TypeProtocolComboBox);

    CountPacketLineEdit=new QLineEdit;
    CountPacketLineEdit->setReadOnly(true);
    CountPacketLabel=new QLabel(tr("число пакетов:"));
    CountPacketLabel->setBuddy(CountPacketLineEdit);

    CountErrorLineEdit=new QLineEdit;
    CountErrorLineEdit->setReadOnly(true);
    CountErrorLabel=new QLabel(tr("Число ошибок:"));
    CountErrorLabel->setBuddy(CountErrorLineEdit);

    ValueLineEdit=new QLineEdit;
    ValueLineEdit->setReadOnly(true);
    ValueLabel=new QLabel(tr("Показания:"));
    ValueLabel->setBuddy(ValueLineEdit);

    SettingsPortButton=new QPushButton(tr("Настройка Com-порта"));
    SettingsPortButton->setEnabled(true);
    OnComPortButton=new QPushButton(tr("Подключить"));
    OnComPortButton->setEnabled(true);
    OffComPortButton=new QPushButton(tr("Отключить"));
    OffComPortButton->setEnabled(false);

    AdditionalParamButton=new QPushButton(tr("Дополнительно ..."));

    QGroupBox *GyroSettingsBox=new QGroupBox(tr("Настройка гироскопического устройства"));

    QGridLayout *LeftLayout=new QGridLayout;
    LeftLayout->addWidget(TypeProtocolLabel,0,0);
    LeftLayout->addWidget(TypeProtocolComboBox,0,1);
    LeftLayout->addWidget(CountPacketLabel,1,0);
    LeftLayout->addWidget(CountPacketLineEdit,1,1);
    LeftLayout->addWidget(CountErrorLabel,2,0);
    LeftLayout->addWidget(CountErrorLineEdit,2,1);
    LeftLayout->addWidget(ValueLabel,3,0);
    LeftLayout->addWidget(ValueLineEdit,3,1);
    LeftLayout->addWidget(AdditionalParamButton,4,0);

    GyroSettingsBox->setLayout(LeftLayout);

    QVBoxLayout *RightLayout=new QVBoxLayout;
    RightLayout->addWidget(SettingsPortButton);
    RightLayout->addWidget(OnComPortButton);
    RightLayout->addWidget(OffComPortButton);
    RightLayout->addStretch();

    QGridLayout *MainLayout=new QGridLayout;
    MainLayout->addWidget(GyroSettingsBox,0,0);
    MainLayout->addLayout(RightLayout,0,1);

    MainWidget=new QWidget;
    MainWidget->setLayout(MainLayout);
    MainWidget->setStyleSheet("QLineEdit{border-style: outset;border-radius:3px;"
                              "border-width: 1px;"
                              "min-height: 1.2em;max-height: 2em; min-width:5em;max-width:5em}");
    setCentralWidget(MainWidget);
    this->setWindowTitle(tr("Параметры гироскопического устройства"));
    this->statusBar()->showMessage(tr("Выполните настройку Com-порта"));

}
//-----------------------------------------------------------
void GyroDevice::CreateConnections()
{
    connect(SettingsPortButton,SIGNAL(pressed()),SettingsComPort,SLOT(show()));


    connect(OnComPortButton,SIGNAL(pressed()),this,SLOT(OpenSerialPort()));
    connect(OffComPortButton,&QPushButton::pressed,this,&GyroDevice::CloseSerialPort);
    connect(this,&GyroDevice::ConnectComPort,DeviceComPort,&comPort::ConnectPort);
    connect(DeviceComPort,&comPort::isConnectedPort,this,&GyroDevice::isConnectedComPort);
    connect(DeviceComPort,&comPort::isNotConnectedPort,this,&GyroDevice::isNotConnectedComPort);
    connect(this,&GyroDevice::DisconnectComPort,DeviceComPort,&comPort::DisconnectPort);

}
