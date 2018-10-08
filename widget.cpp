#include "widget.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QGroupBox>
#include <QtGlobal>
#include <chrono>

#ifdef Q_OS_WIN
#include <winbase.h>
#endif

enum position{DEG_90=0,DEG_180,DEG_270,DEG_0};
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
Widget::Widget(QWidget *parent)
    : QMainWindow ()
{
    Q_UNUSED(parent);
    ConfigTableDevice = new TableDevice;
    ConfigGyroDevice = new GyroDevice;
    Log = new loger;

    ptmr = new QTimer;
    ptmr->setTimerType(Qt::TimerType::PreciseTimer);

    InitVariable();
    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
    CreateWidgets();
    initActionConnections();
    CreateConnections();
}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
Widget::~Widget()
{
    delete ConfigTableDevice;
    delete ConfigGyroDevice;
}
//-----------------------------------------------------------
// Назначение: закрытие окна
//-----------------------------------------------------------
void Widget::closeEvent(QCloseEvent *event)
{
//    Q_UNUSED(event);
    ptmr->stop();    
    onWindowClosed();
    event->accept();
}
//-----------------------------------------------------------
// Назначение: создание действий
//-----------------------------------------------------------
void Widget::CreateActions()
{

    OneMeasurementAction = new QAction(tr("Выполнить однократное измерение"),this);
    OneMeasurementAction->setIcon(QIcon(":/icons/onestart.png"));

    ConfigTabelDevAction = new QAction(tr("Поворотное устройство"),this);
    ConfigTabelDevAction->setIcon(QIcon(":/icons/table.png"));

    ConfigGyroDevAction = new QAction(tr("Гироскопическое устройство"),this);
    ConfigGyroDevAction->setIcon(QIcon(":/icons/gyroscope.png"));

    StartTimerAction = new QAction(tr("Выполнить серию измерений"),this);
    StartTimerAction->setIcon(QIcon(":/icons/start.png"));

    StopTimerAction = new QAction(tr("Остановить измерения"),this);
    StopTimerAction->setIcon(QIcon(":/icons/stop.png"));
    StopTimerAction->setEnabled(false);


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
    fileMenu = menuBar()->addMenu(tr("&Меню"));
    fileMenu->addAction(OneMeasurementAction);
    fileMenu->addAction(StartTimerAction);
    fileMenu->addAction(StopTimerAction);

    configMenu = menuBar()->addMenu(tr("&Окно"));
    configMenu->addAction(ConfigTabelDevAction);
    configMenu->addAction(ConfigGyroDevAction);
}
//-----------------------------------------------------------
// Назначение: создание панели инструментов
//-----------------------------------------------------------
void Widget::CreateToolBars()
{
    toolbar=addToolBar(tr("Меню"));
    toolbar->addAction(OneMeasurementAction);
    toolbar->addAction(StartTimerAction);
    toolbar->addAction(StopTimerAction);
    toolbar->addSeparator();
    toolbar->addAction(ConfigGyroDevAction);
    toolbar->addAction(ConfigTabelDevAction);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setFloatable(false);
    toolbar->setMovable(false);
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
    QRegExp regExp("[0-9][0-9]{0,4}");
    QDoubleValidator* regExp2= new QDoubleValidator(0.0,360.0,5);

    QGroupBox *leftgroupBox = new QGroupBox(tr("Измерения азимута"));
    QGroupBox *rightgroupBox = new QGroupBox;

    currValueLabel=new QLabel(tr("Значение азимута:"));
    meanValueLabel=new QLabel(tr("Среднее значение:"));
    minValueLabel=new QLabel(tr("Минимальное значение:"));
    maxValueLabel=new QLabel(tr("Максимальное значение:"));
    skoLabel=new QLabel(tr("СКО:"));

    timeAccumulateLabel=new QLabel(tr("Время накопления(сек.)"));

    currValueLineEdit=new QLineEdit;
    currValueLineEdit->setReadOnly(true);
    currValueLineEdit->setValidator(regExp2);
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

    countMeasureLabel=new QLabel(tr("Количество измерений"));
    countMeasureLineEdit=new QLineEdit;
    countMeasureLineEdit->setReadOnly(true);

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

    RightLayout->addWidget(countMeasureLabel,2,0);
    RightLayout->addWidget(countMeasureLineEdit,2,1);

    leftgroupBox->setLayout(LeftLayout);
    rightgroupBox->setLayout(RightLayout);
    //MainLayout->addLayout(LeftLayout);
    MainLayout->addWidget(leftgroupBox);
//    MainLayout->addLayout(RightLayout);
    MainLayout->addWidget(rightgroupBox);

    measureWidget->setLayout(MainLayout);
    setCentralWidget(measureWidget);

   this->setWindowIcon(QIcon(":/icons/compas.png"));
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
    //Сигнал старта измерений (ConfigTableDevice->isMeasuring=true)
    connect(this,&Widget::StartMeasure,
            ConfigTableDevice,&TableDevice::StartMeasure);

    connect(OneMeasurementAction,&QAction::triggered,
            this,&Widget::StartMeasureSlot);
    connect(OneMeasurementAction,&QAction::triggered,
            this,&Widget::OneMeasureSlot);
    //---------------------------------
    //действия по нажатию кнопки СТОП
    //---------------------------------
    //остановка поворотного стола
    connect(StopTimerAction,&QAction::triggered,
            ConfigTableDevice,&TableDevice::FinishedMotion);
    //вызов StopMeasureSignal
    connect(StopTimerAction,&QAction::triggered,
            this,&Widget::StopMeasureSlot);
    //сброс признака (TableDevice->isMeasuring=false)
    connect(this,&Widget::StopMeasureSignal,
            ConfigTableDevice, &TableDevice::StopMeasure);

    //остановка накопления данных, сброс всех параметров ГК
    connect(this,&Widget::StopMeasureSignal,
            ConfigGyroDevice->Measure,&GyroData::Stop);
    //---------------------------------
    //запуск таймера
    //---------------------------------
    //запуск таймера обратного отсчета при остановке стола
    connect(ConfigTableDevice,&TableDevice::StopRotation,
            this,&Widget::StartTimer);
    //---------------------------------
    //Взаимодействие с поворотным столом
    //---------------------------------
    connect(this,&Widget::GotoPosition,
            ConfigTableDevice,&TableDevice::GoToPosition);
    //сброс абс.координат
    connect(this,&Widget::ResetAbsCoord,
            ConfigTableDevice,&TableDevice::ResetAbsCoord);

    //----------------------------------
    //взаимодействие с гироскопом
    //-----------------------------------
    //включение накопления данных ( выполняется при остановке стола)
    connect(this,&Widget::StartAccumulateDataSignal,
            ConfigGyroDevice->Measure,
            &GyroData::AccumulateData);
    //окончание накопления данных при начале поворота стола
    //вызывается в функции Measure()
    connect(this,&Widget::StopAccumulateDataSignal,
            ConfigGyroDevice->Measure,
            &GyroData::NoAccumulateData);

    connect(ptmr,&QTimer::timeout,this,&Widget::Measure);

    connect(this,&Widget::PutLog,Log,&loger::PutLog);
}
//-----------------------------------------------------------
// Назначение: инициализация переменных
//-----------------------------------------------------------
void Widget::InitVariable()
{
    this->timeSec=0.;
    this->prevMeasure=0;
    this->numMeasure=0;
    this->numPosition=0;
    this->Azimuth=0.;
    this->pos_0=0.;
    this->pos_180=0.;
    this->pos_90=0.;
    this->pos_270=0.;
    this->Azimuth=0.;
    this->SummAzimuth=0.;
    this->MeanAzimuth=0.;
    this->MinAzimuth=0.;
    this->MaxAzimuth=0.;
    this->SKO=0.;
    this->numerator=0.;
    this->denumerator=0.;
    this->isOneMeasure=false;
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

        StartTimerAction->setEnabled(false);
        OneMeasurementAction->setEnabled(false);
        StopTimerAction->setEnabled(true);
    }
    else
        emit StopMeasureSignal();
}
//-----------------------------------------------------------
// Назначение: остановка измерений
//-----------------------------------------------------------
void Widget::StopMeasureSlot()
{
    ptmr->stop();
    this->InitVariable();
    this->timeAccumulateLineEdit->setEnabled(true);
    emit StopMeasureSignal();

    StartTimerAction->setEnabled(true);
    OneMeasurementAction->setEnabled(true);
    StopTimerAction->setEnabled(false);
}
//-----------------------------------------------------------
// Назначение: запуск таймера и вызов  сигнала
//             накопления данных
//-----------------------------------------------------------
void Widget::StartTimer()
{
    ptmr->setInterval(timeSec);
    ptmr->setSingleShot(true);
    ptmr->start();
    emit StartAccumulateDataSignal();
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
    emit StopAccumulateDataSignal();
    switch (numPosition) {
    case DEG_180:
        pos_0=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition++;
        emit GotoPosition(-200000);
        break;
    case DEG_270:
        pos_180=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition++;
        emit GotoPosition(-300000);
        break;
    case DEG_90:
        pos_90=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition++;
        emit GotoPosition(-100000);
        break;
    case DEG_0:
        pos_270=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition=0;
        numMeasure++;
        emit GotoPosition(0);
        break;
    default: break;
    }
    if(numMeasure>prevMeasure){
        prevMeasure=numMeasure;

        //пересчет параметров
         Azimuth=qRadiansToDegrees(static_cast<float>(atan2((pos_270-pos_90),
                                                            (pos_0-pos_180))));
         Azimuth<0?Azimuth+=360.0:Azimuth;
         SummAzimuth+=Azimuth;
         if(numMeasure==1){
             MeanAzimuth=Azimuth;
             MaxAzimuth=Azimuth;
             MinAzimuth=Azimuth;
         }
         else{
             MeanAzimuth=SummAzimuth/numMeasure;
             MaxAzimuth<Azimuth?MaxAzimuth=Azimuth:MaxAzimuth;
             MinAzimuth>Azimuth?MinAzimuth=Azimuth:MinAzimuth;
         }

         numerator+=powf((Azimuth-MeanAzimuth),2);
         numMeasure>1?denumerator=numMeasure-1:denumerator=1;
         SKO=sqrt(numerator/denumerator);

         currValueLineEdit->setText(QString::number(static_cast<double>(Azimuth)));
         meanVelueLineEdit->setText(QString::number(static_cast<double>(MeanAzimuth)));
         minValueLineEdit->setText(QString::number(static_cast<double>(MinAzimuth)));
         maxValueLineEdit->setText(QString::number(static_cast<double>(MaxAzimuth)));
         skoLineEdit->setText(QString::number(static_cast<double>(SKO)));

         if(numMeasure==1)
             emit PutLog(tr("время\tазимут\t ср.знач\t мин.знач\t макс.знач\t СКО\n"));
         emit PutLog(tr("%1\t %2\t %3\t %4\t %5\n")
                            .arg(static_cast<double>(Azimuth))
                            .arg(static_cast<double>(MeanAzimuth))
                            .arg(static_cast<double>(MinAzimuth))
                            .arg(static_cast<double>(MaxAzimuth))
                            .arg(static_cast<double>(SKO))
                         );
        countMeasureLineEdit->setText(QString::number(numMeasure));
         if(isOneMeasure){
             isOneMeasure=false;
             this->StopMeasureSlot();
         }
    }

}

void Widget::OneMeasureSlot()
{
    this->isOneMeasure=true;
}
