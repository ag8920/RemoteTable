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
    connect(ConfigTabelDevAction,SIGNAL(triggered()),ConfigTableDevice,SLOT(show()));
        connect(ConfigGyroDevAction,SIGNAL(triggered()),ConfigGyroDevice,SLOT(show()));
    connect(this,SIGNAL(onWindowClosed()),ConfigTableDevice,SLOT(close()));
    connect(this,SIGNAL(onWindowClosed()),ConfigGyroDevice,SLOT(close()));


    connect(StartTimerAction,&QAction::triggered,this,&Widget::StartTimer);
    connect(StopTimerAction,&QAction::triggered,this,&Widget::StopTimer);

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
                                 "min-height: 1.2em;max-height: 2em; min-width:5em;max-width:5em}");


    measureWidget->show();
}

void Widget::CreateConnections()
{
    connect(StartTimerAction,&QAction::triggered,
            this,&Widget::StartMeasureSlot);
    //запуск перехода в нулевую точку. начало измерений
    connect(this,&Widget::StartMeasure,
            ConfigTableDevice,&TableDevice::DispOfPosition);
    //
    connect(this,&Widget::StartMeasure,
            ConfigTableDevice,&TableDevice::StartMeasure);

    //переход в следующую позцию по таймеру
    connect(tmr,&QTimer::timeout,
            ConfigTableDevice,&TableDevice::DispOfPosition);
    //запуск таймера обратного отсчета при остановке стола
    connect(ConfigTableDevice,&TableDevice::StopRotation,
            this,&Widget::StartTimer);
    //остановка(сброс) таймера при остановке стола
    connect(ConfigTableDevice,&TableDevice::StartRotation,
            this,&Widget::StopTimer);
    //окончание накопления данных при начале поворота стола
    connect(tmr,&QTimer::timeout,
            ConfigGyroDevice->Measure,&GyroMeasure::NoAccumulateData);
    //включение накопления данных при остановке стола
    connect(this,&Widget::AccumulateDataSignal,
            ConfigGyroDevice->Measure, &GyroMeasure::AccumulateData);
    //отправка номера позиции- возможно нужно переделать
    connect(ConfigTableDevice,&TableDevice::SendNumPosition,
            ConfigGyroDevice->Measure,&GyroMeasure::GetPosition);
    //отправка номера измерения
    connect(ConfigTableDevice,&TableDevice::SendNumMeasure,
            ConfigGyroDevice->Measure,&GyroMeasure::Measure);

//    connect(this,&Widget::StartMeasure,
//            ConfigGyroDevice->Measure,&GyroMeasure::AccumulateData);

    //сброс абсолютной координаты при старте измерений
//    connect(StartTimerAction,&QAction::triggered,
//            ConfigTableDevice, &TableDevice::ResetAbsCoord);

    //окончание измерений вызов StopMeasureSignal
    connect(StopTimerAction,&QAction::triggered,
            this,&Widget::StopMeasureSlot);
    //окончание измерений. сброс признака TableDevice->isMeasuring
    connect(this,&Widget::StopMeasureSignal,
            ConfigTableDevice, &TableDevice::StopMeasure);
    //окончание измерений остановка поворотного стола
    connect(StopTimerAction,&QAction::triggered,
            ConfigTableDevice,&TableDevice::FinishedMotion);

    connect(ConfigGyroDevice->Measure,&GyroMeasure::SendMeasureData,
            this,&Widget::GetMeasureData);
}



void Widget::StartMeasureSlot()
{

    if(this->SetTime()){
        emit StartMeasure();
    }
}

void Widget::StartTimer()
{
    if(timeSec)
    {
        tmr->setInterval(timeSec);
        tmr->start();
        emit AccumulateDataSignal();
    }
}

void Widget::StopTimer()
{
    tmr->stop();
    //    emit StopMeasure();
}

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

void Widget::StopMeasureSlot()
{
   this->timeAccumulateLineEdit->setEnabled(true);
    emit StopMeasureSignal();
}

void Widget::GetMeasureData(const QString Azimuth,
                            const QString mean, const QString min,
                            const QString max, const QString sko)
{
    currValueLineEdit->setText(Azimuth);
    meanVelueLineEdit->setText(mean);
    minValueLineEdit->setText(min);
    maxValueLineEdit->setText(max);
    skoLineEdit->setText(sko);
}



