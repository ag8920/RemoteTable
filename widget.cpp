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
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
Widget::Widget(QWidget *parent)
    : QMainWindow ()
{
    Q_UNUSED(parent);
    ConfigTableDevice = new TableDevice;
    ConfigGyroDevice = new GyroDevice;

    ptmr = new QTimer;


    InitVariable();
    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
    CreateWidgets();
    initActionConnections();
    CreateConnections();

//    QueryPerformanceFrequency()

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
    DeltaPsProtocolAction = new QAction(tr("Протокол \"Delta_PS\" "));

    Rate2ProtocolAction = new QAction(tr("Протокол \"Rate_2\" "));

    DadvttProtocolAction = new QAction(tr("Протокол \"Dadvtt\" "));

    OneMeasurementAction = new QAction(tr("Выполнить однократное измерение"));
    OneMeasurementAction->setIcon(QIcon(":/icons/onestart.png"));

    MultiMeasurementAction = new QAction(tr("Выполнить серию измерений"));

    ConfigTabelDevAction = new QAction(tr("Поворотное устройство"));
    ConfigTabelDevAction->setIcon(QIcon(":/icons/table.png"));

    ConfigGyroDevAction = new QAction(tr("Гироскопическое устройство"));
    ConfigGyroDevAction->setIcon(QIcon(":/icons/gyroscope.png"));

    StartTimerAction = new QAction(tr("Запуск"));
    StartTimerAction->setIcon(QIcon(":/icons/start.png"));

    StopTimerAction = new QAction(tr("Стоп"));
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
//    fileMenu->addAction(DeltaPsProtocolAction);
//    fileMenu->addAction(Rate2ProtocolAction);
//    fileMenu->addAction(DadvttProtocolAction);
//    fileMenu->addSeparator();
    fileMenu->addAction(OneMeasurementAction);
    fileMenu->addAction(MultiMeasurementAction);
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
    QRegExp regExp("[1-9][0-9]{0,4}");
    QDoubleValidator* regExp2= new QDoubleValidator(0.0,360.0,5);

    QGroupBox *leftgroupBox = new QGroupBox(tr("Измерения азимута"));
    QGroupBox *rightgroupBox = new QGroupBox;

    currValueLabel=new QLabel(tr("Значение азимута:"));
    meanValueLabel=new QLabel(tr("Среднее значение:"));
    minValueLabel=new QLabel(tr("Минимальное значение:"));
    maxValueLabel=new QLabel(tr("Максимальное значение:"));
    skoLabel=new QLabel(tr("СКО:"));

    timeAccumulateLabel=new QLabel(tr("Время накопления(сек.)"));
    azimuthMeasureLabel=new QLabel(tr("Измеренное значение азимута"));
    azimuthMeasureLabel->hide();

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

    azimuthMeasureLineEdit=new QLineEdit;
    azimuthMeasureLineEdit->setValidator(regExp2);
    azimuthMeasureLineEdit->setReadOnly(true);
    azimuthMeasureLineEdit->hide();

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

    /*
    QPalette pal=measureWidget->palette();
    pal.setColor(QPalette::Text,Qt::green);
    pal.setColor(QPalette::Background,Qt::black);
    measureWidget->setPalette(pal);
    */
//    measureWidget->setStyleSheet("QLineEdit{border-style: outset;border-radius:3px;"
//                                 "border-width: 1px;"
//                                 "min-height: 1.2em;max-height: 2em; "
//                                 "min-width:5em;max-width:5em}");

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
            this,&Widget::OneMeasureSlot);///< @todo доделать однократный запуск
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
            &GyroMeasure::AccumulateData);
    //окончание накопления данных при начале поворота стола
    //вызывается в функции Measure()
    connect(this,&Widget::StopAccumulateDataSignal,
            ConfigGyroDevice->Measure,
            &GyroMeasure::NoAccumulateData);

    connect(ptmr,&QTimer::timeout,this,&Widget::Measure);

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
    this->pos1=0.;
    this->pos2=0.;
    this->pos3=0.;
    this->pos4=0.;
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
///< @todo проверить работу таймера, если результат не удастся вернуться к signgleshot()
void Widget::StartTimer()
{
//        QTimer::singleShot(timeSec,Qt::PreciseTimer,
//                           this,SLOT(Measure()));
    ptmr->setInterval(timeSec);
    ptmr->setSingleShot(true);//(QThread::TimeCriticalPriority);
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
    case 0:
        pos1=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition++;
        emit GotoPosition(200000);
        break;
    case 1:
        pos2=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition++;
        emit GotoPosition(300000);
        break;
    case 2:
        pos3=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition++;
        emit GotoPosition(100000);
        break;
    case 3:
        pos4=this->ConfigGyroDevice->Measure->diff;
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
         Azimuth=qRadiansToDegrees(static_cast<float>(atan2((pos4-pos3),
                                                            (pos1-pos2))));
         Azimuth<0?Azimuth+=360.0:Azimuth;
//         Azimuth=fmodf(Azimuth,360.);
//         if(Azimuth<0)
//             Azimuth+=360.;
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
