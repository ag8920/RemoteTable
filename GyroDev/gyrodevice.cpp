#include "gyrodevice.h"

GyroDevice::GyroDevice(QWidget *parent) : QMainWindow(parent)
{
    updateSettingsPort=0;
    SettingsComPort = new SettingsDialog;
    DeviceComPort = new comPort;
    ComPortThread = new QThread;
    ConsoleWidget = new Console;

    Measure = new GyroMeasure;

    AddThread();
    CreateWidgets();
    CreateConnections();
}
//-----------------------------------------------------------
GyroDevice::~GyroDevice()
{
//    this->StopThreads();
    ComPortThread->quit();
}
//-----------------------------------------------------------
void GyroDevice::OpenSerialPort()
{
    SettingsDialog::Settings p=SettingsComPort->settings();
    QString name=static_cast<QString>(p.name);
    int baudRate=static_cast<int>(p.baudRate);
    int dataBits=static_cast<int>(p.dataBits);
    int parity=static_cast<int>(p.parity);
    int stopBits=static_cast<int>(p.stopBits);
    int flowControl=static_cast<int>(p.flowControl);
    //emit ConnectComPort(p);
    emit ConnectComPort(name,baudRate,dataBits,parity,stopBits,flowControl);
}
//-----------------------------------------------------------
void GyroDevice::CloseSerialPort()
{
    emit DisconnectComPort();
}
//-----------------------------------------------------------
void GyroDevice::UpdateSettingsComPort()
{
    OnComPortButton->setEnabled(true);
    updateSettingsPort=1;
}
//-----------------------------------------------------------
void GyroDevice::isConnectedComPort(const QString msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(false);
    OnComPortButton->setEnabled(false);
    OffComPortButton->setEnabled(true);
//    ConsoleWidget->setEnabled(true);

}
//-----------------------------------------------------------
void GyroDevice::isNotConnectedComPort(const QString msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(true);
    if(updateSettingsPort)
        OnComPortButton->setEnabled(true);
    OffComPortButton->setEnabled(false);
//    ConsoleWidget->setEnabled(false);

}
//-----------------------------------------------------------
void GyroDevice::CreateWidgets()
{
    ConsoleWidget->setEnabled(true);
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
    OnComPortButton->setEnabled(false);
    OffComPortButton=new QPushButton(tr("Отключить"));
    OffComPortButton->setEnabled(false);

    ClearConsoleButton=new QPushButton(tr("Очистить"));
    ClearConsoleButton->setEnabled(true);

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
    RightLayout->addWidget(ClearConsoleButton);

    QGridLayout *MainLayout=new QGridLayout;
    MainLayout->addWidget(GyroSettingsBox,0,0);
    MainLayout->addLayout(RightLayout,0,1);

    QVBoxLayout *GeneralLayout=new QVBoxLayout;
    GeneralLayout->addLayout(MainLayout);
    GeneralLayout->addWidget(ConsoleWidget);

    MainWidget=new QWidget;
    MainWidget->setLayout(GeneralLayout);
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
    connect(SettingsComPort,&SettingsDialog::isUpdateSettings,
            this,&GyroDevice::UpdateSettingsComPort);

    connect(OnComPortButton,SIGNAL(pressed()),this,SLOT(OpenSerialPort()));
    connect(OffComPortButton,&QPushButton::pressed,this,&GyroDevice::CloseSerialPort);
    connect(this,&GyroDevice::ConnectComPort,DeviceComPort,&comPort::ConnectPort);
    connect(DeviceComPort,&comPort::isConnectedPort,this,&GyroDevice::isConnectedComPort);
    connect(DeviceComPort,&comPort::isNotConnectedPort,this,&GyroDevice::isNotConnectedComPort);
    connect(this,&GyroDevice::DisconnectComPort,DeviceComPort,&comPort::DisconnectPort);

    connect(DeviceComPort,&comPort::dataOutput,
            ConsoleWidget,&Console::putData);
    connect(ClearConsoleButton,&QPushButton::pressed,
            ConsoleWidget,&Console::clear);

    connect(DeviceComPort,&comPort::dataOutput,
            Measure,&GyroMeasure::GetData);
}
//-----------------------------------------------------------
void GyroDevice::AddThread()
{
    DeviceComPort->moveToThread(ComPortThread); //помещаем класс в поток
    DeviceComPort->thisPort.moveToThread(ComPortThread);//помещаем порт в поток

    connect(ComPortThread,&QThread::started,
            DeviceComPort,&comPort::processPort);
    connect(DeviceComPort,&comPort::finishedPort,
            ComPortThread,&QThread::quit);
    connect(ComPortThread,&QThread::finished,
            DeviceComPort,&comPort::deleteLater);
    connect(DeviceComPort,&comPort::finishedPort,
            ComPortThread,&QThread::deleteLater);
    connect(this,&GyroDevice::StopAll,
            DeviceComPort,&comPort::Stop);

    ComPortThread->start(QThread::TimeCriticalPriority);
}
//-----------------------------------------------------------
void GyroDevice::StopThreads()
{
    emit StopAll();
}
