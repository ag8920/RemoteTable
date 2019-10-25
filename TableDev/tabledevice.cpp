//------------------------------------------------------------------------------
//     Данный модуль создает виджет окна
//     с параметрами и настройкой поворотного
//     утройства.
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------

#include <QtMath>
#include "tabledevice.h"


enum { HEX=1,DEC,OCT,BIN,ASCII,};
enum TypeTable {TABLE1,TABLE2};

//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
TableDevice::TableDevice(QWidget *parent) : QMainWindow(parent),
    accyracyTable(0)
{
    SettingsComPort = new SettingsDialog(nullptr,1);
    DeviceComPort = new comPort;
    ComPortThread = new QThread;
    ConsoleWidget = new Console;
    ConsoleWidget->hide();

    Table1=new tableRS232;
    Table2=new tableRS485;

    tmr = new QTimer;
    isPosition=false;
    isMeasuring=false;
    isRotation=false;
    currPosition=0;
    prevPosition=0;
    numPosition=0;
    numMeasure=0;
    AddThreads();
    CreateWidgets();
    CreateConnections();
    SetTimer();

    readSettings();
    setTypeTable(TypeTableComboBox->currentIndex());


}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-------------------------------------------------------
TableDevice::~TableDevice()
{
    qDebug("TableDevice::~TableDevice()");
    ComPortThread->quit();
    tmr->stop();
}
//-----------------------------------------------------------
// Назначение: открыть порт
//-----------------------------------------------------------
void TableDevice::OpenSerialPort()
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
        if (TypeTableComboBox->currentIndex()==TABLE1)
            emit ConnectComPort(name,baudRate,dataBits,parity,stopBits,flowControl);
        else if (TypeTableComboBox->currentIndex()==TABLE2) {
            Table2->ComPort->ConnectPort(name,baudRate,dataBits,parity,stopBits,flowControl);
            ResetAbsolutCoordButton->hide();
            OnMotionButton->hide();
            OffMotionButton->hide();
            PositveRotationCheckBox->hide();
            AbsolutPositioningCheckBox->hide();
        }
        TypeTableComboBox->setEnabled(false);
        ComPortButton->setText(tr("Отключить"));
        ComPortButton->setIcon(QIcon(":/icons/connect.png"));

    }else{
        if (TypeTableComboBox->currentIndex()==TABLE1)
            emit DisconnectComPort();
        else if (TypeTableComboBox->currentIndex()==TABLE2) {
            Table2->ComPort->DisconnectPort();

        }
        OnMotionButton->show();
        OffMotionButton->show();
        ResetAbsolutCoordButton->show();
        PositveRotationCheckBox->show();
        AbsolutPositioningCheckBox->show();
        TypeTableComboBox->setEnabled(true);
        ComPortButton->setText(tr("Подключить"));
        ComPortButton->setIcon(QIcon(":/icons/disconnect.png"));
    }
}

//-------------------------------------------------------
// Назначение: проверка обновления состояний портов
//-------------------------------------------------------
void TableDevice::UpdateSettingsComPort()
{
    ComPortButton->setEnabled(true);
    updateSettingsPort=1;
}
//-----------------------------------------------------------
// Назначение:  показать/скрыть панель позиционирования
//-----------------------------------------------------------
void TableDevice::PositioningParamVisible()
{
    if(PositioningButton->isChecked())
        PositioningBox->show();
    else
        PositioningBox->hide();
}
//-----------------------------------------------------------
// Назначение:  выполнить позиционирование
//-----------------------------------------------------------
void TableDevice::ExecutePosition()
{
    QByteArray data;
    QString str;
    isPosition=true;

    if (TypeTableComboBox->currentIndex()==TABLE1)
    {
        int speed=Deg2Label(RateOfTurnLineEdit->text().toDouble());
        int pos=Deg2Label(PositioningLineEdit->text().toDouble());
        Table1->setSpeed(QString::number(speed));
        Table1->setAngle(QString::number(pos));
        if(AbsolutPositioningCheckBox->isChecked())Table1->setTypePositioning(";PA=");
        else Table1->setTypePositioning(";PR=");

        Table1->ExecutePos();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->TurnMode();
        Table2->setAngle(PositioningLineEdit->text().toFloat());

    }
}
//-----------------------------------------------------------
// Назначение: возрат в нулевое положение
//-----------------------------------------------------------
void TableDevice::ZeroPostion()
{
    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        //int speed=Deg2Label(RateOfTurnLineEdit->text().toDouble());
        Table1->ZeroPosition();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->TurnMode();
        Table2->setAngle(0.);
    }
}

//-----------------------------------------------------------
// Назначение: получение текущей позиции и
//             определение движения поворотного стола
//-----------------------------------------------------------
void TableDevice::GetPosition(const QByteArray &data)
{
    static QString str=nullptr;
    double currpos=0.;

    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        str=Table1->GetPosition(data);
        if(!str.isEmpty()) {
            currPosition=str.toInt();
            currpos=Label2Deg(currPosition);
            CurrPositionLineEdit->setText(QString::number(currpos,'g',8));
            str=nullptr;
        }
        if(isMeasuring)
        {
            static int count=0;
            if((std::abs(currPosition-nextPosition)<=accyracyTable))count++;//косвенный признак того, что стол остановился
            if((std::abs(currPosition-nextPosition)<=accyracyTable) && isRotation && count>10)
            {
                isRotation=false;
                count=0;
                qDebug()<<"StopRotation";
                emit StopRotation();
            }

            prevPosition=currPosition;
        }

    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
//        CurrPositionLineEdit->setText(QString::number(Table2->getCurrentPos()));
//        if(isMeasuring){
//            if(std::fabs(Table2->getCurrentPos()-nextPosition<=0.0001)){
//                isRotation=false;
//                emit StopRotation();
//            }
//        }
//        prevPosition=Table2->getCurrentPos();
    }

}
//-----------------------------------------------------------
// Назначение: абсолютное позиционирование
//-----------------------------------------------------------
void TableDevice::GoToPosition(double position)
{
    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        int speed=Deg2Label(RateOfTurnLineEdit->text().toDouble());
        nextPosition=Deg2Label(position);
        Table1->setSpeed(QString::number(speed));
        Table1->GoToPosition(QString::number(nextPosition));
        isRotation=true;
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->TurnMode();
        Table2->setAngle(static_cast<float>(position));
        nextPosition=position-floor(position/360.)*360;
        isRotation=true;
    }
}
//-----------------------------------------------------------
// Назначение: Запрос текущего полжения
//-----------------------------------------------------------
void TableDevice::RequestPosition()
{

    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        Table1->RequestPosition();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->RequestPos();
        double currentPos=0;

        currentPos=Table2->getCurrentPos();
        currentPos=currentPos-floor(currentPos/360.)*360.;
        CurrPositionLineEdit->setText(QString::number(currentPos));
        if(isMeasuring){
            if(std::fabs(Table2->getCurrentPos()-nextPosition<=0.0001) && isRotation){
                isRotation=false;
                emit StopRotation();
            }
        }
//        qDebug()<<CurrPositionLineEdit->text();
//        qDebug()<<Table2->getCurrentPos();
        prevPosition=Table2->getCurrentPos();
    }

}
//-----------------------------------------------------------
// Назначение: Включить привод
//-----------------------------------------------------------
void TableDevice::OnMotion()
{

    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        Table1->OnMotion();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {

    }
}
//-----------------------------------------------------------
// Назначение: начать вращение(команда старт)
//-----------------------------------------------------------
void TableDevice::BeginMotion()
{

    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        SettingsRotation();
        Table1->BeginMotion();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->SpinMode();
        Table2->setSpeed(RateOfTurnLineEdit->text().toFloat());
    }
}
//-----------------------------------------------------------
// Назначение: остановить вращение(команда стоп)
//-----------------------------------------------------------
void TableDevice::StopMotion()
{

    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        Table1->StopMotion();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->TurnMode();
    }
}
//-----------------------------------------------------------
// Назначение: отключить привод
//-----------------------------------------------------------
void TableDevice::OffMotion()
{
    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        Table1->OffMotion();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->TurnMode();
    }
}
//-----------------------------------------------------------
// Назначение: остановить вращение(команда стоп)
// и отключить привод
//-----------------------------------------------------------
void TableDevice::FinishedMotion()
{
    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        Table1->FinishedMotion();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        Table2->TurnMode();
    }
}
//-----------------------------------------------------------
// Назначение: сброс абсолютной координаты
//-----------------------------------------------------------
void TableDevice::ResetAbsCoord()
{
    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        Table1->ResetAbsCoord();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {

    }
}
//-----------------------------------------------------------
// Назначение: отправка команд(ручной режим работы)
//-----------------------------------------------------------
void TableDevice::ManualMode()
{
    QByteArray data;
    data=SendCommandLineEdit->text().toLocal8Bit();
    if(TypeTableComboBox->currentIndex()==TABLE1)
        emit OutputToComPort(data);
    else if(TypeTableComboBox->currentIndex()==TABLE2)
        Table2->ComPort->WriteToPort(data);

}
//-----------------------------------------------------------
// Назначение: настройка скорости и направления вращения
//-----------------------------------------------------------
void TableDevice::SettingsRotation()
{
    if(TypeTableComboBox->currentIndex()==TABLE1)
    {
        int speed=Deg2Label(RateOfTurnLineEdit->text().toDouble());
        Table1->setSpeed(QString::number(speed));
        if(PositveRotationCheckBox->isChecked())
            Table1->setDirrection("jv=-");
        else
            Table1->setDirrection("jv=");
        Table1->DirectionTurn();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        //nop
    }
}
//-----------------------------------------------------------
// Назначение: установка формата отображения
//             принятых от стола данных
//-----------------------------------------------------------
void TableDevice::SetFormatConsole()
{
    if(AsciiFormatCheckBox->isChecked())
        emit ConsoleSetFormat(ASCII);
    else
        emit ConsoleSetFormat(HEX);
}
//-----------------------------------------------------------
// Назначение: установка состояния кнопок
//             при подключении порта
//-----------------------------------------------------------
void TableDevice::isConnectedComPort(const QString &msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(false);
    SendCommandButton->setEnabled(true);
}
//-----------------------------------------------------------
// Назначение: установка состояния кнопок
//             при отключении порта
//-----------------------------------------------------------
void TableDevice::isNotConnectedComPort(const QString &msg)
{
    this->statusBar()->showMessage(msg,0);
    SettingsPortButton->setEnabled(true);
    if(updateSettingsPort)
        ComPortButton->setEnabled(true);
    SendCommandButton->setEnabled(false);
}
//-----------------------------------------------------------
// Назначение: создание виджета окна
//-----------------------------------------------------------
void TableDevice::CreateWidgets()
{
    ConsoleWidget->setEnabled(true);
    QRegExp regExp("[0-9][0-9]{0,6}");
    //-----------------------------------------------------------
    TypeTableComboBox=new QComboBox();
    TypeTableComboBox->addItem(QStringLiteral("Поворотный стол 1"));
    TypeTableComboBox->addItem(QStringLiteral("Поворотный стол 2"));

    TypeTableLabel = new QLabel(tr("Тип поворотного устройства:"));
    TypeTableLabel->setBuddy(TypeTableComboBox);

    CurrPositionLineEdit=new QLineEdit;
    CurrPositionLineEdit->setReadOnly(true);
    CurrPositionLabel=new QLabel("Текущая позиция(град.):");
    CurrPositionLabel->setBuddy(CurrPositionLineEdit);
    RateOfTurnLineEdit=new QLineEdit;
    RateOfTurnLineEdit->setValidator(new QRegExpValidator(regExp,this));
    RateOfTurnLineEdit->setText("30");
    RateOfTurnLabel = new QLabel(tr("Скорость вращения (град./сек)"));
    PositveRotationCheckBox=new QCheckBox(tr("Положительное направление"));

    //скрыта
    NegativeRotationCheckBox=new QCheckBox(tr("Отрицательное направление"));
    NegativeRotationCheckBox->hide();

    RequestPostionCheckBox = new QCheckBox(tr("Запрос текущего положения"));
    RequestPostionCheckBox->setChecked(true);
    PositioningButton=new QPushButton(tr("Позиционирование..."));
    PositioningButton->setCheckable(true);
    PositioningButton->setAutoDefault(true);
    PositioningButton->hide();

    //-----------------------------------------------------------
    //скрыта
    ConsoleVisibleCheckBox=new QCheckBox(tr("Показать консоль"));
    ConsoleVisibleCheckBox->hide();
    AsciiFormatCheckBox=new QCheckBox(tr("Отображать в ASCII"));
    AsciiFormatCheckBox->hide();

    QGridLayout *MainLayout=new QGridLayout;
    QGridLayout *SettingsTableLayout=new QGridLayout;
    QGroupBox *TableSettingsBox=new QGroupBox(tr("Параметры поворотного устройства"));

    SettingsTableLayout->addWidget(TypeTableLabel,0,0);
    SettingsTableLayout->addWidget(TypeTableComboBox,0,1);
    SettingsTableLayout->addWidget(CurrPositionLabel,1,0);
    SettingsTableLayout->addWidget(CurrPositionLineEdit,1,1);

    SettingsTableLayout->addWidget(RateOfTurnLabel,2,0);
    SettingsTableLayout->addWidget(RateOfTurnLineEdit,2,1);
    SettingsTableLayout->addWidget(PositveRotationCheckBox,3,0);
    SettingsTableLayout->addWidget(NegativeRotationCheckBox,4,0);
    SettingsTableLayout->addWidget(RequestPostionCheckBox,5,0);
    SettingsTableLayout->addWidget(PositioningButton,3,1);

    TableSettingsBox->setLayout(SettingsTableLayout);
    //-----------------------------------------------------------
    QVBoxLayout *SettingsButtonLayout=new QVBoxLayout;
    OnMotionButton=new QPushButton(tr("Вкл.привод"));
//    OnMotionButton->setIcon(QIcon(":/icons/turnon.png"));
    OnMotionButton->setEnabled(true);
    OnMotionButton->setAutoDefault(true);

    OffMotionButton=new QPushButton(tr("Откл.привод"));
    OffMotionButton->setEnabled(true);
    OffMotionButton->setAutoDefault(true);

    startButton=new QPushButton(tr("Нач.вращение"));
//    startButton->setIcon(QIcon(":/icons/start.png"));
    startButton->setEnabled(true);
    startButton->setAutoDefault(true);

    stopButton=new QPushButton(tr("Остан.вращ."));

//    stopButton->setIcon(QIcon(":/icons/stop.png"));
    stopButton->setEnabled(true);
    stopButton->setAutoDefault(true);

    SettingsPortButton=new QPushButton(tr("Настр.Com-порта"));
    SettingsPortButton->setIcon(QIcon(":/icons/settings2.png"));
    SettingsPortButton->setEnabled(true);
    SettingsPortButton->setAutoDefault(true);

    ComPortButton=new QPushButton(tr("Подключить"));
    ComPortButton->setIcon(QIcon(":/icons/disconnect.png"));
    ComPortButton->setEnabled(false);
    ComPortButton->setCheckable(true);
    ComPortButton->setAutoDefault(true);
    //скрыта
    ClearConsoleButton=new QPushButton(tr("Очистить"));
    ClearConsoleButton->setAutoDefault(true);
    ClearConsoleButton->hide();

    SettingsButtonLayout->addWidget(SettingsPortButton);
    SettingsButtonLayout->addWidget(ComPortButton);
    SettingsButtonLayout->addStretch();
    SettingsButtonLayout->addWidget(OnMotionButton);
    SettingsButtonLayout->addWidget(startButton);
    SettingsButtonLayout->addWidget(stopButton);
    SettingsButtonLayout->addWidget(OffMotionButton);
    //SettingsButtonLayout->addWidget(ResetAbsolutCoordButton);
    //SettingsButtonLayout->addStretch();
    SettingsButtonLayout->addWidget(ClearConsoleButton);
    //SettingsButtonLayout->addWidget(AsciiFormatCheckBox);


    //позиционирование
    //-----------------------------------------------------------
    PositioningBox=new QGroupBox(tr("Позиционирование"));

    PositionCommand=new QMenu;
    PositionCommand->addMenu(tr("Выполнить"));
    PositionCommand->addMenu(tr("Нулевое положение"));
    PositionCommand->addMenu(tr("Обнул.абс.координаты"));

    QGridLayout *PositioningLayout=new QGridLayout;
    ResetAbsolutCoordButton=new QPushButton(tr("Обнул.абс.координаты"));
    ResetAbsolutCoordButton->setAutoDefault(true);

    ExecutePositioningButton=new QPushButton(tr("Выполнить"));
    ExecutePositioningButton->setAutoDefault(true);

    ZeroPositionButton=new QPushButton(tr("Нулевое положение"));
    ZeroPositionButton->setAutoDefault(true);

    PositioningLineEdit=new QLineEdit;
    PositioningLineEdit->setText("180");
    PositioningLabel=new QLabel(tr("угол поворота(град.)"));
    PositioningLabel->setBuddy(PositioningLabel);
    PositioningLineEdit->setValidator(new QRegExpValidator(regExp,this));
    AbsolutPositioningCheckBox=new QCheckBox(tr("Абc.позиционирование"));

    //скрыта
    RelativePositioningCheckBox=new QCheckBox(tr("Относительное позиционирование"));
    RelativePositioningCheckBox->hide();

    PositioningLayout->addWidget(AbsolutPositioningCheckBox,1,0);
    PositioningLayout->addWidget(RelativePositioningCheckBox,0,1);
    PositioningLayout->addWidget(PositioningLabel,0,0);
    PositioningLayout->addWidget(PositioningLineEdit,0,1);
    PositioningLayout->addWidget(ResetAbsolutCoordButton,1,1);
    PositioningLayout->addWidget(ZeroPositionButton,2,1);
    PositioningLayout->addWidget(ExecutePositioningButton,2,0);
    PositioningBox->setLayout(PositioningLayout);
//    PositioningBox->hide();


    //отправка команд
    //-----------------------------------------------------------
    QGroupBox *SendCommandBox=new QGroupBox(tr("Отправка команд (ручной режим)"));
    QGridLayout *SendCommandLayout=new QGridLayout;
    SendCommandButton=new QPushButton(tr("Отправить"));
    SendCommandButton->setAutoDefault(true);
    SendCommandButton->setEnabled(false);

    SendCommandLineEdit=new QLineEdit;
    SendCommandLayout->addWidget(SendCommandLineEdit,0,0);
    SendCommandLayout->addWidget(SendCommandButton,0,1);
    SendCommandBox->setLayout(SendCommandLayout);
    //-----------------------------------------------------------
    QVBoxLayout *LeftLayout=new QVBoxLayout;
    LeftLayout->addWidget(TableSettingsBox);
    LeftLayout->addWidget(PositioningBox);
    LeftLayout->addWidget(SendCommandBox);
    //-----------------------------------------------------------
    MainLayout->addLayout(LeftLayout,0,0);
    MainLayout->addLayout(SettingsButtonLayout,0,1);
    //-----------------------------------------------------------
    QVBoxLayout *GeneralLayout=new QVBoxLayout;
    GeneralLayout->addLayout(MainLayout);
    GeneralLayout->addWidget(ConsoleVisibleCheckBox);
    GeneralLayout->addWidget(AsciiFormatCheckBox);
    GeneralLayout->addWidget(ConsoleWidget);


    MainWidget=new QWidget;
    MainWidget->setLayout(GeneralLayout);
    //    MainWidget->setStyleSheet("QLineEdit{border-style: outset;border-radius:3px;"
    //                              "border-width: 1px;"
    //                              "min-height: 1.2em; min-width:5em;max-width:10em}");
    this->setWindowIcon(QIcon(":/icons/table.png"));
    setCentralWidget(MainWidget);
    this->setWindowTitle(tr("Настройка поворотного устройства"));

    this->statusBar()->showMessage(tr("Выполните настройку COM порта"));

}
//-----------------------------------------------------------
// Назначение: соединение сигналов и слотов
//-----------------------------------------------------------
void TableDevice::CreateConnections()
{
    connect(Table1,&tableRS232::OutputToComPort,this,&TableDevice::OutputToComPort);
    connect(Table2,&tableRS485::OutputToComPort,DeviceComPort,&comPort::writeAndRead);

    connect(SettingsComPort,&SettingsDialog::isUpdateSettings,
            this,&TableDevice::UpdateSettingsComPort);
    connect(SettingsPortButton,&QPushButton::pressed,
            SettingsComPort,&SettingsDialog::showWidget);
    connect(OnMotionButton,&QPushButton::pressed,
            this,&TableDevice::OnMotion);
    connect(OffMotionButton,&QPushButton::pressed,
            this,&TableDevice::OffMotion);
    connect(startButton,&QPushButton::pressed,
            this,&TableDevice::BeginMotion);
    connect(stopButton,&QPushButton::pressed,
            this,&TableDevice::StopMotion);
    connect(ResetAbsolutCoordButton,&QPushButton::pressed,
            this,&TableDevice::ResetAbsCoord);
    connect(ExecutePositioningButton,&QPushButton::pressed,
            this,&TableDevice::ExecutePosition);

    connect(SendCommandButton,&QPushButton::pressed,
            this,&TableDevice::ManualMode);
    connect(ComPortButton,&QPushButton::toggled,
            this,&TableDevice::OpenSerialPort);
    connect(this,&TableDevice::ConnectComPort,
            DeviceComPort,&comPort::ConnectPort);
    connect(DeviceComPort,&comPort::isConnectedPort,
            this,&TableDevice::isConnectedComPort);
    connect(DeviceComPort,&comPort::isNotConnectedPort,
            this,&TableDevice::isNotConnectedComPort);
    connect(this,&TableDevice::DisconnectComPort,
            DeviceComPort,&comPort::DisconnectPort);

    connect(this,&TableDevice::OutputToComPort,
            DeviceComPort,&comPort::WriteToPort);

    connect(DeviceComPort,&comPort::dataOutput,
            ConsoleWidget,&Console::putData);
    connect(ClearConsoleButton,&QPushButton::pressed,
            ConsoleWidget,&Console::clear);
    connect(AsciiFormatCheckBox,&QCheckBox::clicked,
            this,&TableDevice::SetFormatConsole);
    connect (ConsoleVisibleCheckBox,&QCheckBox::clicked,
             this,&TableDevice::ConsoleVisible);

    connect(this,&TableDevice::ConsoleSetFormat,
            ConsoleWidget,&Console::SetFormat);

    connect(PositioningButton,&QPushButton::toggled,
            this,&TableDevice::PositioningParamVisible);
    connect(ZeroPositionButton,&QPushButton::pressed,
            this,&TableDevice::ZeroPostion);

    connect(RequestPostionCheckBox,&QCheckBox::stateChanged,
            this,&TableDevice::SetTimer);
    connect(tmr,&QTimer::timeout,
            this,&TableDevice::RequestPosition);

    connect(DeviceComPort,&comPort::dataOutput,
            this,&TableDevice::GetPosition);
    connect(TypeTableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){ setTypeTable(index);});

}
//-----------------------------------------------------------
// Назначение: выделение нового потока
//-----------------------------------------------------------
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
    ComPortThread->start(QThread::TimeCriticalPriority);

}
//-----------------------------------------------------------
// Назначение: остановка всех потоков
//-----------------------------------------------------------
void TableDevice::StopThread()
{
    emit StopAll();
}

int TableDevice::Deg2Label(double deg)
{
    return static_cast<int>(deg*400000./360.);
}

double TableDevice::Label2Deg(int label)
{
    return (label*360./400000.);
}

void TableDevice::setTypeTable(int value)
{
    typeTable = value;
    //qDebug()<<"typeTable: "<<typeTable;
}

int TableDevice::getTypeTable() const
{
    return typeTable;
}
//-----------------------------------------------------------
// Назначение: установка таймера для опроса текущих координат
//-----------------------------------------------------------
void TableDevice::SetTimer()
{
    if(RequestPostionCheckBox->isChecked()){
        tmr->setInterval(500);
        tmr->start();
    }
    else
        tmr->stop();
}
//-----------------------------------------------------------
// Назначение: установка признака isMeasuring
//-----------------------------------------------------------
void TableDevice::StartMeasure()
{
    this->isMeasuring=true;

    //состояние кнопок
    OnMotionButton->setEnabled(false);
    OffMotionButton->setEnabled(false);
    startButton->setEnabled(false);
    stopButton->setEnabled(false);
    ResetAbsolutCoordButton->setEnabled(false);
    ZeroPositionButton->setEnabled(false);
    ExecutePositioningButton->setEnabled(false);
    SendCommandButton->setEnabled(false);
    RequestPostionCheckBox->setChecked(true);
    RequestPostionCheckBox->setEnabled(false);
    RateOfTurnLineEdit->setEnabled(false);

}
//-----------------------------------------------------------
// Назначение: сброс признака isMeasuring
//-----------------------------------------------------------
void TableDevice::StopMeasure()
{
    this->isMeasuring=false;
    this->ZeroPostion();
    OnMotionButton->setEnabled(true);
    OffMotionButton->setEnabled(true);
    startButton->setEnabled(true);
    stopButton->setEnabled(true);
    ResetAbsolutCoordButton->setEnabled(true);
    ZeroPositionButton->setEnabled(true);
    ExecutePositioningButton->setEnabled(true);
    SendCommandButton->setEnabled(true);
    RequestPostionCheckBox->setEnabled(true);
    RateOfTurnLineEdit->setEnabled(true);
}
//-----------------------------------------------------------
// Назначение: отображение консоли
//-----------------------------------------------------------
void TableDevice::ConsoleVisible()
{
    if(ConsoleVisibleCheckBox->isChecked()){
        ConsoleWidget->show();
        AsciiFormatCheckBox->show();
        this->window()->resize(minimumSizeHint());
        this->window()->adjustSize();
    }else{
        ConsoleWidget->hide();
        AsciiFormatCheckBox->hide();
        this->window()->adjustSize();
        this->window()->resize(minimumSizeHint());
    }
}

void TableDevice::setAccyracyTable(const int &value)
{
    accyracyTable=value;
}

void TableDevice::initMotion()
{
    if (TypeTableComboBox->currentIndex()==TABLE1)
    {
        Table1->initMotion();
    }
    else if(TypeTableComboBox->currentIndex()==TABLE2)
    {
        //nop
    }

}

void TableDevice::slotSetTypeTable(int idx)
{
   typeTable=idx;
}

void TableDevice::readSettings()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("variable");
    accyracyTable=settings.value("accyracyTable").toInt();
}





