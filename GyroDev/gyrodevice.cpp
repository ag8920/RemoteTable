//------------------------------------------------------------------------------
//     Данный модуль создает виджет окна
//     с параметрами и настройков гироскопического
//     утройства.
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------

#include "gyrodevice.h"
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
GyroDevice::GyroDevice(QWidget *parent) : QMainWindow(parent)
{
    updateSettingsPort=0;
    SettingsComPort = new SettingsDialog;
    DeviceComPort = new comPort;
    ComPortThread = new QThread;
    ConsoleWidget = new Console;

    Measure = new GyroMeasure;
    MeasureThread=new QThread;

    tableWidget=new QWidget(this);
    m_tableView=new QTableView(tableWidget);
    m_model=new TableModel(this);
    m_delegate=new MyDelegate(this);

    AddThread();
    CreateWidgets();
    CreateTable();
    CreateConnections();
}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
GyroDevice::~GyroDevice()
{
//    this->StopThreads();
    ComPortThread->quit();
}
//-----------------------------------------------------------
// Назначение: открыть порт
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
// Назначение: Закрыть порт
//-----------------------------------------------------------
void GyroDevice::CloseSerialPort()
{
    emit DisconnectComPort();
}
//-----------------------------------------------------------
// Назначение: проверка обновления состояний портов
//-----------------------------------------------------------
void GyroDevice::UpdateSettingsComPort()
{
    OnComPortButton->setEnabled(true);
    updateSettingsPort=1;
}
//-----------------------------------------------------------
// Назначение: обновление счетчика пакетов (число пакетов)
//-----------------------------------------------------------
void GyroDevice::UpdateCountPacketLineEdit(const QString packet)
{
    CountPacketLineEdit->setText(packet);
}
//-----------------------------------------------------------
// Назначение: отображение принятых данных
//             в табличном виде
//-----------------------------------------------------------
void GyroDevice::AdditionalParamsVisible()
{
    if(AdditionalParamButton->isChecked())
        m_tableView->show();
    else
        m_tableView->hide();
}
//-----------------------------------------------------------
// Назначение: установка состояния кнопок
//             при подключении порта
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
// Назначение: установка состояния кнопок
//             при отключении порта
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
// Назначение: создание виджета таблицы
//-----------------------------------------------------------
void GyroDevice::CreateTable()
{
    m_tableView->setModel(m_model);
    m_tableView->setSelectionModel(m_tableView->selectionModel());
    m_tableView->setItemDelegate(m_delegate);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->hide();
    //m_tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
}
//-----------------------------------------------------------
// Назначение: создание виджета окна
//-----------------------------------------------------------
void GyroDevice::CreateWidgets()
{
    ConsoleWidget->setEnabled(true);
    TypeProtocolComboBox=new QComboBox;
//    TypeProtocolComboBox->addItem(QStringLiteral("Delta_PS"));
//    TypeProtocolComboBox->addItem(QStringLiteral("Rate_2"));
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
    AdditionalParamButton->setCheckable(true);

    QGroupBox *GyroSettingsBox=new QGroupBox(tr("Параметры гироскопического устройства"));

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

    QVBoxLayout *LeftAll=new QVBoxLayout;
    LeftAll->addLayout(LeftLayout);
    LeftAll->addWidget(m_tableView);
    GyroSettingsBox->setLayout(LeftAll);

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
    this->statusBar()->showMessage(tr("Выполните настройку COM порта"));

}
//-----------------------------------------------------------
// Назначение:  соединение сигналов и слотов
//-----------------------------------------------------------
void GyroDevice::CreateConnections()
{
    connect(SettingsPortButton,SIGNAL(pressed()),SettingsComPort,SLOT(show()));
    connect(SettingsComPort,&SettingsDialog::isUpdateSettings,
            this,&GyroDevice::UpdateSettingsComPort);

    connect(OnComPortButton,SIGNAL(pressed()),this,SLOT(OpenSerialPort()));
    connect(OffComPortButton,&QPushButton::pressed,this,&GyroDevice::CloseSerialPort);
    connect(AdditionalParamButton,&QPushButton::toggled,
            this,&GyroDevice::AdditionalParamsVisible);

    connect(this,&GyroDevice::ConnectComPort,DeviceComPort,&comPort::ConnectPort);
    connect(DeviceComPort,&comPort::isConnectedPort,this,&GyroDevice::isConnectedComPort);
    connect(DeviceComPort,&comPort::isNotConnectedPort,this,&GyroDevice::isNotConnectedComPort);
    connect(this,&GyroDevice::DisconnectComPort,DeviceComPort,&comPort::DisconnectPort);

//    connect(DeviceComPort,&comPort::dataOutput,
//            ConsoleWidget,&Console::putData);
    connect(ClearConsoleButton,&QPushButton::pressed,
            ConsoleWidget,&Console::clear);

    connect(DeviceComPort,&comPort::dataOutput,
            Measure,&GyroMeasure::GetData);
    connect(Measure,&GyroMeasure::outCountPacket,
            this,&GyroDevice::UpdateCountPacketLineEdit);
    connect(Measure,&GyroMeasure::SendDataToTable,
            m_model,&TableModel::loadData);
}
//-----------------------------------------------------------
// Назначение: выделение нового потока
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


//    Measure->moveToThread(MeasureThread);
//    connect(MeasureThread,&QThread::started,
//            Measure,&GyroMeasure::process);
//    connect(Measure,&GyroMeasure::finished,
//            MeasureThread,&QThread::quit);
//    MeasureThread->start();
}
//-----------------------------------------------------------
// Назначение: остановка всех потоков
//-----------------------------------------------------------
void GyroDevice::StopThreads()
{
    emit StopAll();
}
