//------------------------------------------------------------------------------
//     Данный модуль создает
//     виджет главного окна приложения
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------

#include "widget.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QGroupBox>
#include <QtGlobal>
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
Widget::Widget(QWidget *parent)
    : QMainWindow ()
{
    Q_UNUSED(parent);
    ConfigTableDevice=new TableDevice;
    ConfigGyroDevice=new GyroDevice;
    tmr=new QTimer();

    this->timeSec=0;

    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
    CreateWidgets();
    initActionConnections();
    CreateConnections();



    //ConfigTableDevice->show();

}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
Widget::~Widget()
{
    delete ConfigTableDevice;
    delete ConfigGyroDevice;
}

void Widget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    onWindowClosed();
}
//-----------------------------------------------------------
// Назначение: создание действий
//-----------------------------------------------------------
void Widget::CreateActions()
{
    DeltaPsProtocolAction = new QAction(tr("Протокол \"Delta_PS\" "));

    Rate2ProtocolAction = new QAction(tr("Протокол \"Rate_2\" "));

    DadvttProtocolAction = new QAction(tr("Протокол \"Dadvtt\" "));

    OneMeasurementAction = new QAction(tr("Выполнить однократное измерение"));

    MultiMeasurementAction = new QAction(tr("Выполнить серию измерений"));

    ConfigTabelDevAction = new QAction(tr("Поворотное устройство"));

    ConfigGyroDevAction = new QAction(tr("Гироскопическое устройство"));

    StartTimerAction = new QAction(tr("Запуск"));
    StopTimerAction = new QAction(tr("Стоп"));

}

void Widget::initActionConnections()
{
    connect(ConfigTabelDevAction,&QAction::triggered,
            ConfigTableDevice,&TableDevice::show);
    connect(ConfigGyroDevAction,&QAction::triggered,
            ConfigGyroDevice,&GyroDevice::show);
    connect(this,&Widget::onWindowClosed,
            ConfigTableDevice,&TableDevice::close);
    connect(this,&Widget::onWindowClosed,
            ConfigGyroDevice,&GyroDevice::close);

}
//-----------------------------------------------------------
// Назначение: создание меню
//-----------------------------------------------------------
void Widget::CreateMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
//    fileMenu->addAction(DeltaPsProtocolAction);
//    fileMenu->addAction(Rate2ProtocolAction);
//    fileMenu->addAction(DadvttProtocolAction);
//    fileMenu->addSeparator();
    fileMenu->addAction(OneMeasurementAction);
    fileMenu->addAction(MultiMeasurementAction);
    fileMenu->addAction(StartTimerAction);
    fileMenu->addAction(StopTimerAction);

    configMenu = menuBar()->addMenu(tr("&Инструменты"));
    configMenu->addAction(ConfigTabelDevAction);
    configMenu->addAction(ConfigGyroDevAction);
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Widget::CreateToolBars()
{

}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Widget::CreateStatusBar()
{

}

//-----------------------------------------------------------
// Назначение: создание виджета головного окна
//-----------------------------------------------------------
void Widget::CreateWidgets()
{
    measureWidget = new QWidget;    
    QGridLayout *LeftLayout = new  QGridLayout;
    QGridLayout *RightLayout = new QGridLayout;
    QVBoxLayout *MainLayout = new QVBoxLayout;
    QRegExp regExp("[1-9][0-9]{0,4}");
    QDoubleValidator* regExp2= new QDoubleValidator(0.0,360.0,5);

    QGroupBox *leftgroupBox = new QGroupBox(tr("Измерения азимута"));
    QGroupBox *rightgroupBox = new QGroupBox;

    currValueLabel=new QLabel(tr("Текущее значение:"));
    meanValueLabel=new QLabel(tr("Среднее значение:"));
    minValueLabel=new QLabel(tr("Минимальное значение:"));
    maxValueLabel=new QLabel(tr("Максимальное значение:"));
    skoLabel=new QLabel(tr("СКО:"));

    timeAccumulateLabel=new QLabel(tr("Время накопления данных"));
    azimuthMeasureLabel=new QLabel(tr("Измеренное значение азимута"));

    currValueLineEdit=new QLineEdit;
    currValueLineEdit->setReadOnly(true);
    //currValueLabel->setBuddy(currValueLineEdit);

    meanVelueLineEdit=new QLineEdit;
    meanVelueLineEdit->setReadOnly(true);


    minValueLineEdit=new QLineEdit;
    minValueLineEdit->setReadOnly(true);

    maxValueLineEdit=new QLineEdit;

    maxValueLineEdit->setReadOnly(true);

    skoLineEdit=new QLineEdit;
    skoLineEdit->setReadOnly(true);

    timeAccumulateLineEdit=new QLineEdit;
    timeAccumulateLineEdit->setValidator(new QRegExpValidator(regExp,this));
    timeAccumulateLineEdit->setText("600");


    azimuthMeasureLineEdit=new QLineEdit;
    azimuthMeasureLineEdit->setValidator(regExp2);
    azimuthMeasureLineEdit->setReadOnly(true);

    LeftLayout->addWidget(currValueLabel,0,0);
    LeftLayout->addWidget(currValueLineEdit,0,1);



    LeftLayout->addWidget(meanValueLabel,1,0);
    LeftLayout->addWidget(meanVelueLineEdit,1,1);

    LeftLayout->addWidget(maxValueLabel,2,0);
    LeftLayout->addWidget(maxValueLineEdit,2,1);

    LeftLayout->addWidget(minValueLabel,3,0);
    LeftLayout->addWidget(minValueLineEdit,3,1);


    LeftLayout->addWidget(skoLabel,4,0);
    LeftLayout->addWidget(skoLineEdit,4,1);

    RightLayout->addWidget(timeAccumulateLabel,0,0);
    RightLayout->addWidget(timeAccumulateLineEdit,0,1);

    RightLayout->addWidget(azimuthMeasureLabel,1,0);
    RightLayout->addWidget(azimuthMeasureLineEdit,1,1);


    leftgroupBox->setLayout(LeftLayout);
    rightgroupBox->setLayout(RightLayout);
    //MainLayout->addLayout(LeftLayout);
    MainLayout->addWidget(leftgroupBox);
//    MainLayout->addLayout(RightLayout);
    MainLayout->addWidget(rightgroupBox);

    measureWidget->setLayout(MainLayout);
    setCentralWidget(measureWidget);

    /*
    QPalette pal=measureWidget->palette();
    pal.setColor(QPalette::Text,Qt::green);
    pal.setColor(QPalette::Background,Qt::black);
    measureWidget->setPalette(pal);
    */
    measureWidget->setStyleSheet("QLineEdit{border-style: outset;border-radius:3px;"
                                 "border-width: 1px;"
                                 "min-height: 1.2em;max-height: 2em; "
                                 "min-width:5em;max-width:5em}");


    measureWidget->show();
}
//-----------------------------------------------------------
// Назначение: создание соединений СИГНАЛ-СЛОТ
//-----------------------------------------------------------
void Widget::CreateConnections()
{

    //---------------------------------
    //действия по нажатию кнопки СТАРТ
    //---------------------------------
    //установка вермени измерений и вызов сигнала StartMeasure
    connect(StartTimerAction,&QAction::triggered,
            this,&Widget::StartMeasureSlot);
    //Сигнал старта измерений
    connect(this,&Widget::StartMeasure,
            ConfigTableDevice,&TableDevice::StartMeasure);

    //---------------------------------
    //действия по нажатию кнопки СТОП
    //---------------------------------
    //остановка поворотного стола
    connect(StopTimerAction,&QAction::triggered,
            ConfigTableDevice,&TableDevice::FinishedMotion);
    //вызов StopMeasureSignal
    connect(StopTimerAction,&QAction::triggered,
            this,&Widget::StopMeasureSlot);
    //сброс таймера
    connect(StopTimerAction,&QAction::triggered,
            this,&Widget::StopTimer);
    //сброс признака TableDevice->isMeasuring
    connect(this,&Widget::StopMeasureSignal,
            ConfigTableDevice, &TableDevice::StopMeasure);



    //---------------------------------
    //действия по timeout таймера
    //---------------------------------
    //окончание накопления данных при начале поворота стола
    connect(tmr,&QTimer::timeout,
            ConfigGyroDevice->Measure,
            &GyroMeasure::NoAccumulateData);
    //вызов алгоритма по таймеру
    connect(tmr,&QTimer::timeout,
            this,&Widget::Measure);

    //---------------------------------
    //запуск таймера
    //---------------------------------
    //запуск таймера обратного отсчета при остановке стола
    connect(ConfigTableDevice,&TableDevice::StopRotation,
            this,&Widget::StartTimer);
    //включение накопления данных ( выполняется при остановке стола)
    connect(this,&Widget::AccumulateDataSignal,
            ConfigGyroDevice->Measure,
            &GyroMeasure::AccumulateData);


    //---------------------------------
    //остановка таймера
    //---------------------------------
    //остановка(сброс) таймера при повороте стола
    connect(ConfigTableDevice,&TableDevice::StartRotation,
            this,&Widget::StopTimer);

    //---------------------------------
    //Взаимодействие с поворотным столом
    //---------------------------------
    connect(this,&Widget::GotoPosition,
            ConfigTableDevice,&TableDevice::GoToPosition);
    //сброс абс.координат
    connect(this,&Widget::ResetAbsCoord,
            ConfigTableDevice,&TableDevice::ResetAbsCoord);
}


//-----------------------------------------------------------
// Назначение: Запуск измерений
//-----------------------------------------------------------
void Widget::StartMeasureSlot()
{

    if(this->SetTime()){
        this->StartTimer();
        emit StartMeasure();
        emit ResetAbsCoord();
    }
    else
        emit StopMeasureSignal();
}
//-----------------------------------------------------------
// Назначение: остановка измерений
//-----------------------------------------------------------
void Widget::StopMeasureSlot()
{
   this->timeAccumulateLineEdit->setEnabled(true);
    emit StopMeasureSignal();
}
//-----------------------------------------------------------
// Назначение: запуск таймера и испускание сигнала
//             накопления данных
//-----------------------------------------------------------
void Widget::StartTimer()
{
        tmr->setInterval(timeSec);
        tmr->start();
        emit AccumulateDataSignal();
}
//-----------------------------------------------------------
// Назначение: остановка таймера
//-----------------------------------------------------------
void Widget::StopTimer()
{
    tmr->stop();
    //    emit StopMeasure();
}
//-----------------------------------------------------------
// Назначение: установка времени накопления данных
//-----------------------------------------------------------
bool Widget::SetTime()
{
    if(!timeAccumulateLineEdit->text().isEmpty())
    {
        this->timeSec=(timeAccumulateLineEdit->text().toInt()*1000)/4;
        this->timeAccumulateLineEdit->setEnabled(false);
        return true;
    }
    return false;
}

//-----------------------------------------------------------
// Назначение: четырех позиционный алгоритм
//-----------------------------------------------------------


void Widget::Measure()
{
    static int prevMeasure=0;
    static int numMeasure=0;
    static int numPosition=0;
    static float Azimuth=0;
    static float pos1=0,pos2=0,pos3=0,pos4=0.;

    switch (numPosition) {
    case 0:
        pos1=this->ConfigGyroDevice->Measure->summ;
        this->ConfigGyroDevice->Measure->summ=0;
        //this->ConfigTableDevice->GoToPosition(200000);
        numPosition++;
        emit GotoPosition(200000);
        break;
    case 1:
        pos2=this->ConfigGyroDevice->Measure->summ;
        this->ConfigGyroDevice->Measure->summ=0;
//        this->ConfigTableDevice->GoToPosition(300000);
        numPosition++;
        emit GotoPosition(300000);
        break;
    case 2:
        pos3=this->ConfigGyroDevice->Measure->summ;
        this->ConfigGyroDevice->Measure->summ=0;
//        this->ConfigTableDevice->GoToPosition(100000);
        numPosition++;
        emit GotoPosition(100000);
        break;
    case 3:
        pos4=this->ConfigGyroDevice->Measure->summ;
        this->ConfigGyroDevice->Measure->summ=0;
//        this->ConfigTableDevice->GoToPosition(0);
        numPosition=0;
        numMeasure++;
        emit GotoPosition(0);
    default:
        break;
    }
    if(numMeasure>prevMeasure){
        prevMeasure=numMeasure;
        //пересчет параметров
         Azimuth=qRadiansToDegrees(static_cast<float>(atan2((pos1-pos2),(pos3-pos4))));
         currValueLineEdit->setText(QVariant(Azimuth).toString());
    }

}
