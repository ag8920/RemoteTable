#include "tabledevice.h"


TableDevice::TableDevice(QWidget *parent) : QMainWindow(parent)
{
    SettingsComPort = new SettingsDialog;
    DeviceComPort = new comPort;
    ComPortThread = new QThread;
    AddThreads();
    CreateWidgets();
    CreateConnections();

}

TableDevice::~TableDevice()
{
    ComPortThread->quit();
}
//-----------------------------------------------------------
void TableDevice::OpenSerialPort()
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
void TableDevice::CloseSerialPort()
{
    emit DisconnectComPort();
}
//-----------------------------------------------------------
void TableDevice::isConnectedComPort(const QString msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(false);
    OnComPortButton->setEnabled(false);
    OffComPortButton->setEnabled(true);
    SendCommandButton->setEnabled(true);
}
//-----------------------------------------------------------
void TableDevice::isNotConnectedComPort(const QString msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(true);
    OnComPortButton->setEnabled(true);
    OffComPortButton->setEnabled(false);
    SendCommandButton->setEnabled(false);
}
void TableDevice::CreateWidgets()
{
     QRegExp regExp("[1-9][0-9]{0,4}");

    QGroupBox *TypeTableGroupBox=new QGroupBox(tr("Тип поворотного утройства"));
    TypeTableComboBox=new QComboBox;
    TypeTableComboBox->addItem(QStringLiteral("Поворотный стол Б"));
    TypeTableLabel = new QLabel(tr("Тип поворотного устройства:"));
    TypeTableLabel->setBuddy(TypeTableGroupBox);

    RateOfTurnLineEdit=new QLineEdit;
    RateOfTurnLineEdit->setValidator(new QRegExpValidator(regExp,this));
    RateOfTurnLabel = new QLabel(tr("Скорость вращения (меток/сек)"));

    CurrPositionLineEdit=new QLineEdit;
    CurrPositionLineEdit->setReadOnly(true);

    CurrPositionLabel=new QLabel("Текущая позиция(метки):");
    CurrPositionLabel->setBuddy(CurrPositionLineEdit);

    OnMotionButton=new QPushButton(tr("Включить привод"));
    OffMotionButton=new QPushButton(tr("Отключить привод"));
    startButton=new QPushButton(tr("Начать вращение"));
    stopButton=new QPushButton(tr("Остановить вращение"));
    SettingsPortButton=new QPushButton(tr("Настройка Com-порта"));
    OnComPortButton=new QPushButton(tr("Подключить"));
    OffComPortButton=new QPushButton(tr("Отключить"));


    PositveRotationCheckBox=new QCheckBox(tr("Положительное направление"));
    NegativeRotationCheckBox=new QCheckBox(tr("Отрицательное направление"));


    QGridLayout *MainLayout=new QGridLayout;
    QGridLayout *SettingsTableLayout=new QGridLayout;

    QGroupBox *TableSettingsBox=new QGroupBox(tr("Настройка поворотного устройства"));


    SettingsTableLayout->addWidget(TypeTableLabel,0,0);
    SettingsTableLayout->addWidget(TypeTableComboBox,0,1);
    SettingsTableLayout->addWidget(CurrPositionLabel,1,0);
    SettingsTableLayout->addWidget(CurrPositionLineEdit,1,1);

    SettingsTableLayout->addWidget(RateOfTurnLabel,2,0);
    SettingsTableLayout->addWidget(RateOfTurnLineEdit,2,1);
    SettingsTableLayout->addWidget(PositveRotationCheckBox,3,0);
    SettingsTableLayout->addWidget(NegativeRotationCheckBox,4,0);

    TableSettingsBox->setLayout(SettingsTableLayout);

    QVBoxLayout *SettingsButtonLayout=new QVBoxLayout;
    SettingsButtonLayout->addWidget(SettingsPortButton);
    SettingsButtonLayout->addWidget(OnComPortButton);
    SettingsButtonLayout->addWidget(OffComPortButton);
    SettingsButtonLayout->addStretch();
    SettingsButtonLayout->addWidget(OnMotionButton);
    SettingsButtonLayout->addWidget(startButton);
    SettingsButtonLayout->addWidget(stopButton);
    SettingsButtonLayout->addWidget(OffMotionButton);



    QGroupBox *SendCommandBox=new QGroupBox(tr("Отправка команд (ручной режим)"));
    QGridLayout *SendCommandLayout=new QGridLayout;
    SendCommandButton=new QPushButton(tr("Отправить"));
    SendCommandButton->setEnabled(false);
    SendCommandLineEdit=new QLineEdit;
    SendCommandLayout->addWidget(SendCommandLineEdit,0,0);
    SendCommandLayout->addWidget(SendCommandButton,0,1);
    SendCommandBox->setLayout(SendCommandLayout);

    QVBoxLayout *LeftLayout=new QVBoxLayout;
    LeftLayout->addWidget(TableSettingsBox);
    LeftLayout->addWidget(SendCommandBox);

    MainLayout->addLayout(LeftLayout,0,0);
    MainLayout->addLayout(SettingsButtonLayout,0,1);

    MainWidget=new QWidget;
    MainWidget->setLayout(MainLayout);
    MainWidget->setStyleSheet("QLineEdit{border-style: outset;border-radius:3px;"
                              "border-width: 1px;"
                              "min-height: 1.2em; min-width:5em;max-width:10em}");
    setCentralWidget(MainWidget);
    this->setWindowTitle(tr("Настройка поворотного устройства"));

    this->statusBar()->showMessage(tr("Выполните настройку Com-порта"));
}

void TableDevice::CreateConnections()
{
    connect(SettingsPortButton,SIGNAL(pressed()),SettingsComPort,SLOT(show()));


    connect(OnComPortButton,SIGNAL(pressed()),this,SLOT(OpenSerialPort()));
    connect(OffComPortButton,&QPushButton::pressed,this,&TableDevice::CloseSerialPort);
    connect(this,&TableDevice::ConnectComPort,DeviceComPort,&comPort::ConnectPort);
    connect(DeviceComPort,&comPort::isConnectedPort,this,&TableDevice::isConnectedComPort);
    connect(DeviceComPort,&comPort::isNotConnectedPort,this,&TableDevice::isNotConnectedComPort);
    connect(this,&TableDevice::DisconnectComPort,DeviceComPort,&comPort::DisconnectPort);
}

void TableDevice::AddThreads()
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
//    connect(this,&TableDevice::StopAll,
//            DeviceComPort,&comPort::Stop);

    ComPortThread->start(QThread::TimeCriticalPriority);
}

void TableDevice::StopThread()
{
    emit StopAll();
}

