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
    SettingsComPort = new SettingsDialog(nullptr,2);
    DeviceComPort = new comPort;
    ComPortThread = new QThread;
    ConsoleWidget = new Console;

    Measure = new GyroData;
    MeasureThread=new QThread;

    tableWidget=new QWidget(this);
    m_tableView=new QTableView(tableWidget);
    m_model=new TableModel(this);
    m_delegate=new MyDelegate(this);

    log=new loger;
    logThread=new QThread;

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
    logThread->quit();
}
//-----------------------------------------------------------
// Назначение: открыть порт
//-----------------------------------------------------------
void GyroDevice::OpenSerialPort()
{
    if(ComPortButton->isChecked()){
        SettingsDialog::Settings p=SettingsComPort->settings();
        QString name=static_cast<QString>(p.name);
        int baudRate=static_cast<int>(p.baudRate);
        int dataBits=static_cast<int>(p.dataBits);
        int parity=static_cast<int>(p.parity);
        int stopBits=static_cast<int>(p.stopBits);
        int flowControl=static_cast<int>(p.flowControl);
        //emit ConnectComPort(p);
        emit ConnectComPort(name,baudRate,dataBits,parity,stopBits,flowControl);
        ComPortButton->setText(tr("Отключить"));
    } else{
        emit DisconnectComPort();
        ComPortButton->setText(tr("Подключить"));
    }
}
//-----------------------------------------------------------
// Назначение: проверка обновления состояний портов
//-----------------------------------------------------------
void GyroDevice::UpdateSettingsComPort()
{
    ComPortButton->setEnabled(true);
    updateSettingsPort=1;
}
//-----------------------------------------------------------
// Назначение: обновление счетчика пакетов (число пакетов)
//-----------------------------------------------------------
void GyroDevice::UpdateCountPacketLineEdit(const QString &packet,const QString &error)
{
    CountPacketLineEdit->setText(packet);
    CountErrorLineEdit->setText(error);

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
// Назначение: отображение консоли
//-----------------------------------------------------------
void GyroDevice::ConsoleVisible()
{
    if(ConsoleVisibleCheckBox->isChecked())
        ConsoleWidget->show();
    else
        ConsoleWidget->hide();
}

void GyroDevice::SaveData()
{
    if(SaveButton->isChecked()){
        emit BeginRecord();
        SaveButton->setText(tr("Остановить запись"));
    }else{
        emit StopRecord();
        SaveButton->setText(tr("Начать запись"));
    }

}
//-----------------------------------------------------------
// Назначение: установка состояния кнопок
//             при подключении порта
//-----------------------------------------------------------
void GyroDevice::isConnectedComPort(const QString &msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(false);
    //ConsoleWidget->setEnabled(true);

}
//-----------------------------------------------------------
// Назначение: установка состояния кнопок
//             при отключении порта
//-----------------------------------------------------------
void GyroDevice::isNotConnectedComPort(const QString &msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(true);
    if(updateSettingsPort)
        ComPortButton->setEnabled(true);
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
    ConsoleWidget->hide();
    TypeProtocolComboBox=new QComboBox;
    //    TypeProtocolComboBox->addItem(QStringLiteral("Delta_PS"));
    //    TypeProtocolComboBox->addItem(QStringLiteral("Rate_2"));
    TypeProtocolComboBox->addItem(QStringLiteral("Dadvtt"));


    TypeProtocolLabel=new QLabel(tr("Тип протокола:"));
    TypeProtocolLabel->setBuddy(TypeProtocolComboBox);

    CountPacketLineEdit=new QLineEdit;
    CountPacketLineEdit->setReadOnly(true);
    CountPacketLabel=new QLabel(tr("Число принятых пакетов:"));
    CountPacketLabel->setBuddy(CountPacketLineEdit);


    CountErrorLineEdit=new QLineEdit;
    CountErrorLineEdit->setReadOnly(true);
    CountErrorLabel=new QLabel(tr("Число ошибок:"));
    CountErrorLabel->setBuddy(CountErrorLineEdit);

    //скрыта
    ConsoleVisibleCheckBox=new QCheckBox(tr("Показать консоль"));
    ConsoleVisibleCheckBox->setChecked(false);
    ConsoleVisibleCheckBox->hide();

    SettingsPortButton=new QPushButton(tr("Настройка Com-порта"));
    SettingsPortButton->setEnabled(true);
    SettingsPortButton->setAutoDefault(true);

    ComPortButton=new QPushButton(tr("Подключить"));
    ComPortButton->setEnabled(false);
    ComPortButton->setCheckable(true);
    ComPortButton->setAutoDefault(true);

    //скрыта
    ClearConsoleButton=new QPushButton(tr("Очистить"));
    ClearConsoleButton->setEnabled(true);
    ClearConsoleButton->setAutoDefault(true);
    ClearConsoleButton->hide();

    AdditionalParamButton=new QPushButton(tr("Дополнительно ..."));
    AdditionalParamButton->setCheckable(true);
    AdditionalParamButton->setAutoDefault(true);

    SaveButton=new QPushButton(tr("Начать запись"));
    SaveButton->setCheckable(true);
    SaveButton->setAutoDefault(true);

    QGroupBox *GyroSettingsBox=new QGroupBox(tr("Параметры гироскопического устройства"));

    QGridLayout *LeftLayout=new QGridLayout;
    LeftLayout->addWidget(TypeProtocolLabel,0,0);
    LeftLayout->addWidget(TypeProtocolComboBox,0,1);
    LeftLayout->addWidget(CountPacketLabel,1,0);
    LeftLayout->addWidget(CountPacketLineEdit,1,1);
    LeftLayout->addWidget(CountErrorLabel,2,0);
    LeftLayout->addWidget(CountErrorLineEdit,2,1);
    LeftLayout->addWidget(ConsoleVisibleCheckBox,5,0);
    LeftLayout->addWidget(AdditionalParamButton,4,0);

    QVBoxLayout *LeftAll=new QVBoxLayout;
    LeftAll->addLayout(LeftLayout);
    LeftAll->addWidget(m_tableView);
    LeftAll->addWidget(ConsoleVisibleCheckBox);
    GyroSettingsBox->setLayout(LeftAll);

    QVBoxLayout *RightLayout=new QVBoxLayout;
    RightLayout->addWidget(SettingsPortButton);
    RightLayout->addWidget(ComPortButton);
    RightLayout->addWidget(SaveButton);
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
    setCentralWidget(MainWidget);
    this->setWindowIcon(QIcon(":/icons/gyroscope.png"));
    this->setWindowTitle(tr("Параметры гироскопического устройства"));
    this->statusBar()->showMessage(tr("Выполните настройку COM порта"));

}
//-----------------------------------------------------------
// Назначение:  соединение сигналов и слотов
//-----------------------------------------------------------
void GyroDevice::CreateConnections()
{
    connect(SettingsPortButton,&QPushButton::pressed,
            SettingsComPort,&SettingsDialog::showWidget);
    connect(SettingsComPort,&SettingsDialog::isUpdateSettings,
            this,&GyroDevice::UpdateSettingsComPort);

    connect(ComPortButton,&QPushButton::toggled,
            this,&GyroDevice::OpenSerialPort);

    connect(ConsoleVisibleCheckBox,&QCheckBox::clicked,
            this,&GyroDevice::ConsoleVisible);

    connect(AdditionalParamButton,&QPushButton::toggled,
            this,&GyroDevice::AdditionalParamsVisible);

    connect(this,&GyroDevice::ConnectComPort,
            DeviceComPort,&comPort::ConnectPort);
    connect(DeviceComPort,&comPort::isConnectedPort,
            this,&GyroDevice::isConnectedComPort);
    connect(DeviceComPort,&comPort::isNotConnectedPort,
            this,&GyroDevice::isNotConnectedComPort);
    connect(this,&GyroDevice::DisconnectComPort,
            DeviceComPort,&comPort::DisconnectPort);

    //    connect(DeviceComPort,&comPort::dataOutput,
    //            ConsoleWidget,&Console::putData);

    connect(ClearConsoleButton,&QPushButton::pressed,
            ConsoleWidget,&Console::clear);

    connect(DeviceComPort,&comPort::dataOutput,
            Measure,&GyroData::GetData);
    connect(Measure,&GyroData::outCountPacket,
            this,&GyroDevice::UpdateCountPacketLineEdit);
    connect(Measure,&GyroData::SendDataToTable,
            m_model,&TableModel::loadData);


    connect(Measure,&GyroData::SendDecodeData,
            log,&loger::write);
    connect(Measure, &GyroData::PutLog,
            log, &loger::PutLog2);
    connect(SaveButton,&QPushButton::toggled,
            this,&GyroDevice::SaveData);
    connect(this,&GyroDevice::BeginRecord,
            log,&loger::start);
    connect(this,&GyroDevice::StopRecord,
            log,&loger::CloseFile);
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


    Measure->moveToThread(MeasureThread);
    connect(MeasureThread,&QThread::started,
            Measure,&GyroData::process);
    connect(Measure,&GyroData::finished,
            MeasureThread,&QThread::quit);
    MeasureThread->start();

    log->moveToThread(logThread);
    //    connect(logThread,&QThread::started,
    //            log,&loger::start);
    connect(log,&loger::finished,
            logThread,&QThread::quit);
    connect(logThread,&QThread::finished,
            log,&loger::deleteLater);
    connect(log,&loger::finished,
            logThread,&QThread::deleteLater);
    logThread->start();
    qDebug()<< QThread::currentThread();

}
//-----------------------------------------------------------
// Назначение: остановка всех потоков
//-----------------------------------------------------------
void GyroDevice::StopThreads()
{
    emit StopAll();
}
