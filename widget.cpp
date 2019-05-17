#include "widget.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QtGlobal>
#include <QSpacerItem>
#include <QDataStream>
#include <QIODevice>
#include <chrono>
#include <QtGlobal>
#ifdef Q_OS_WIN
#include <winbase.h>
#endif

enum position{DEG_90=0,DEG_180,DEG_270,DEG_0};
QStringList alignmode={"Короткий (ГК-1)","Точный (ГК-2)","Повторное ГК","Непрерывное ГК"};
enum alignmode{GK_1,GK_2,REPEAT,CONTINUOUS};
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
Widget::Widget(QWidget *parent)
    : QMainWindow ()
{
    Q_UNUSED(parent);
    ConfigTableDevice = new TableDevice;
    ConfigGyroDevice = new GyroDevice;
    CoordDialog = new corrdDialog;
    ConfigNmeaDevice=new NmeaDevice;
    Log = new loger;

    ptmr = new QTimer;
    ptmr->setTimerType(Qt::TimerType::PreciseTimer);

    timeLine=new QTimeLine;
    timeLine->setFrameRange(0,100);
    timeLine->setCurveShape(QTimeLine::LinearCurve);
    //tmrsec=new QTimer;
    //tmrsec->setTimerType(Qt::TimerType::VeryCoarseTimer);
    //tmrsec->setInterval(1000);
    //tmrsec->start();

    //setAttribute(Qt::WA_DeleteOnClose);//указывает на необходимость удаления окна при его закрытии

    //tablers=new tableRS485;
    //tablers->setAngle();

    InitVariable();
    CreateWidgets();
    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();

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
    delete ConfigNmeaDevice;
    saveSettings();
}
//-----------------------------------------------------------
// Назначение: закрытие окна
//-----------------------------------------------------------
void Widget::closeEvent(QCloseEvent *event)
{
    //    Q_UNUSED(event);
    ptmr->stop();
    saveSettings();
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


    SetCoordianteAction = new QAction(tr("Задать координаты"),this);
    SetCoordianteAction->setIcon(QIcon(":/icons/coordinate.png"));

    ViewPlotAction = new QAction(tr("Графики"),this);
    stopPlot=new QAction(tr("остановить"),this);

    ConfigNmeadeviceAction = new QAction(tr("СНС"),this);
    ConfigNmeadeviceAction->setIcon(QIcon(":/icons/satellite.png"));

    FourAlgAction=new QAction(tr("4-x позиционный алгоритм"),this);
    FourAlgAction->setCheckable(true);
    ThreeAlgAction=new QAction(tr("3-х позиционный алгоритм"),this);
    ThreeAlgAction->setCheckable(true);
    AlgGroupAction=new QActionGroup(this);
    AlgGroupAction->addAction(FourAlgAction);
    AlgGroupAction->addAction(ThreeAlgAction);
    FourAlgAction->setChecked(true);

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
    connect(this, &Widget::onWindowClosed,
            ConfigNmeaDevice, &NmeaDevice::close);
    connect(SetCoordianteAction,&QAction::triggered,
            CoordDialog,&QDialog::show);
    connect(ConfigNmeadeviceAction, &QAction::triggered,
            ConfigNmeaDevice, &NmeaDevice::show);
    //    connect(ViewPlotAction,&QAction::triggered,
    //            tablers,&tableRS485::setAngle);
    connect(AlgGroupAction, &QActionGroup::triggered,
            this, &Widget::selectAlgorithm);
}
//-----------------------------------------------------------
// Назначение: создание меню
//-----------------------------------------------------------
void Widget::CreateMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Меню"));
    fileMenu->addAction(SetCoordianteAction);
    fileMenu->addAction(OneMeasurementAction);
    fileMenu->addAction(StartTimerAction);
    fileMenu->addAction(StopTimerAction);
    fileMenu->addAction(FourAlgAction);
    fileMenu->addAction(ThreeAlgAction);



    configMenu = menuBar()->addMenu(tr("&Окно"));
    configMenu->addAction(ConfigTabelDevAction);
    configMenu->addAction(ConfigGyroDevAction);
    configMenu->addAction(ConfigNmeadeviceAction);
    //    configMenu->addAction(ViewPlotAction);
    //    configMenu->addAction(stopPlot);
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
    toolbar->addAction(ConfigNmeadeviceAction);
    toolbar->setIconSize(QSize(20,20));
    //toolbar->setFloatable(false);
    //toolbar->setMovable(false);


    Coordtoolbar=new QToolBar(tr("Координаты"));
    Coordtoolbar->addWidget(LatLabel);
    Coordtoolbar->addWidget(LonLabel);
    Coordtoolbar->addWidget(HLabel);
    addToolBar(Coordtoolbar);

//    QWidget *pwgt=new QWidget;
//    QVBoxLayout *layout=new QVBoxLayout();
//    layout->addWidget(LatLabel);
//    layout->addWidget(LonLabel);
//    layout->addWidget(HLabel);
//    layout->addStretch();
//    pwgt->setLayout(layout);
//    pdock=new QDockWidget;
//    pdock->setWidget(pwgt);
//    pdock->setAllowedAreas(Qt::AllDockWidgetAreas);
//    pdock->setFloating(false);
//    pdock->setWindowTitle(tr("СНС"));
//    addDockWidget(Qt::TopDockWidgetArea,pdock);

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
    QHBoxLayout *MainLayout= new QHBoxLayout;
    QRegExp regExp("[0-9][0-9]{0,4}");
    QRegExp exp("^(?:[1-2][0-9]{0,2}(?:\\.[0-9]{1,2})?|3(?:[0-5]?[0-9]?(?:\\.[0-9]{1,6})?|"
               "60(?:\\.00?)?)|[4-9][0-9]?(?:\\.[0-9]{1,6})?|0(?:\\.[0-9]{1,6})?)° ?[LR]$");
    QDoubleValidator* regExp2= new QDoubleValidator(0.0,360.0,6);
    regExp2->setRange(0,360,6);
    QDoubleValidator* regExpDouble=new QDoubleValidator();
    regExpDouble->setLocale(QLocale::English);

    QGroupBox *measuregroupBox = new QGroupBox(tr("Измерения"));


    LatLabel=new QLabel("Lat=0.0 ");
    LonLabel=new QLabel("Lon=0.0 ");
    HLabel=new QLabel("H=0.0 ");


    currValueLineEdit=new QLineEdit;
    currValueLineEdit->setReadOnly(true);
    currValueLineEdit->setValidator(new QRegExpValidator(exp,this));

    azimuthXALineEdit=new QLineEdit;
    azimuthXALineEdit->setReadOnly(true);
    azimuthXALineEdit->setValidator(new QRegExpValidator(exp,this));


    azimuthDiffLineEdit=new QLineEdit;
    azimuthDiffLineEdit->setValidator(new QRegExpValidator(exp,this));


    //currValueLineEdit->setText("400");
    meanVelueLineEdit=new QLineEdit;
    meanVelueLineEdit->setReadOnly(true);
    minValueLineEdit=new QLineEdit;
    minValueLineEdit->setReadOnly(true);
    maxValueLineEdit=new QLineEdit;
    maxValueLineEdit->setReadOnly(true);
    skoLineEdit=new QLineEdit;
    skoLineEdit->setReadOnly(true);

    typeAlignCBox=new QComboBox(this);
    typeAlignCBox->insertItems(0,alignmode);

    progress=new QProgressBar();
    progress->setRange(0,100);



    timeAccumulateLineEdit=new QLineEdit;
    timeAccumulateLineEdit->setValidator(new QRegExpValidator(regExp,this));
    //timeAccumulateLineEdit->setText("600");
    this->timeAccumulateLineEdit->setReadOnly(true);
    countMeasureLabel=new QLabel(tr("Количество измерений"));
    countMeasureLineEdit=new QLineEdit;
    countMeasureLineEdit->setReadOnly(true);
    RollLineEdit=new  CustomLineEdit("Roll")/*QLineEdit*/;
    RollLineEdit->setReadOnly(true);
    PitchLineEdit=new CustomLineEdit("Pitch");
    PitchLineEdit->setReadOnly(true);

    QFormLayout *measureformLayout=new QFormLayout();
    measureformLayout->addRow(tr("Азимут БИП:"),currValueLineEdit);
    measureformLayout->addRow(tr("Ср.значение азимута БИП:"),meanVelueLineEdit);
    measureformLayout->addRow(tr("Макс.значение азимута БИП:"),maxValueLineEdit);
    measureformLayout->addRow(tr("Мин.значение азимута БИП:"),minValueLineEdit);
    measureformLayout->addRow(tr("СКО:"),skoLineEdit);
    measureformLayout->addRow(tr("Значение крена БИП:"),RollLineEdit);
    measureformLayout->addRow(tr("Значение тангажа БИП:"),PitchLineEdit);
    measureformLayout->addRow(tr("Количество измерений:"),countMeasureLineEdit);
    measureformLayout->addRow(tr("Азимут ХА:"),azimuthXALineEdit);
    measureformLayout->addRow(tr("Разность азимутов:"),azimuthDiffLineEdit);
    measureformLayout->addRow(tr("Режим выставки:"),typeAlignCBox);
    measureformLayout->addRow(tr("Время накопления(сек):"),timeAccumulateLineEdit);


    measuregroupBox->setLayout(measureformLayout);
    QVBoxLayout *grBoxLayout=new QVBoxLayout();
    grBoxLayout->addWidget(measuregroupBox);
    grBoxLayout->addWidget(progress);
    QVBoxLayout *vbox=new QVBoxLayout();
    vbox->addLayout(grBoxLayout);
    MainLayout->addLayout(vbox);
    measureWidget->setLayout(MainLayout);
    setCentralWidget(measureWidget);

    this->setWindowIcon(QIcon(":/icons/compas.png"));
    measureWidget->show();

    this->selectModeAlign(typeAlignCBox->currentIndex());
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

    //connect(ptmr,&QTimer::timeout,this,&Widget::Measure);
    connect(ConfigGyroDevice->Measure, &GyroData::signalStopAcumulateData,
            this, &Widget::Measure);

    connect(this,&Widget::PutLog,Log,&loger::PutLog);

    connect(CoordDialog,&corrdDialog::outCoordinate,
            ConfigGyroDevice->Measure,&GyroData::GetCoordinate);
    connect(CoordDialog, &corrdDialog::outCoordinate,
            this, &Widget::recieveCoordinate);
    connect(ConfigGyroDevice->Measure,&GyroData::outAngle,
            this,&Widget::viewAngle);

    connect(ConfigNmeaDevice, &NmeaDevice::sendBasicData,
            this, &Widget::recieveSnsBasicData);

    connect(this, &Widget::sendCoordinate,ConfigGyroDevice->Measure, &GyroData::GetCoordinate);

    //    connect(tmrsec,&QTimer::timeout,this,&Widget::slotbuildgraph);
    //    connect(this,&Widget::buildgraph,plotWidget,&PlotWidget::realtimeDataSlot);
    //    connect(stopPlot,&QAction::triggered,tmrsec,&QTimer::stop);

    //    connect(RollLineEdit,&CustomLineEdit::doubleclick,this,&Widget::createPlot);
    //    connect(PitchLineEdit,&CustomLineEdit::doubleclick,this,&Widget::createPlot);

    connect(typeAlignCBox, SIGNAL(activated(int)),
             this, SLOT(selectModeAlign(int)));

//    connect(timeLine, &QTimeLine::frameChanged,
//            progress, &QProgressBar::setValue
//            );
    connect(timeLine, &QTimeLine::finished,
            progress, &QProgressBar::reset);
    connect(timeLine, &QTimeLine::finished,
            timeLine, &QTimeLine::start);

    connect(timeLine, &QTimeLine::valueChanged,
            this, &Widget::setProgress);
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
    this->da2_pos0=0.;
    this->da2_pos180=0.;
    this->da2_pos90=0.;
    this->da2_pos270=0.;
    this->Azimuth=0.;
    this->SummAzimuth=0.;
    this->MeanAzimuth=0.;
    this->MinAzimuth=0.;
    this->MaxAzimuth=0.;
    this->SKO=0.;
    this->numerator=0.;
    this->denumerator=0.;
    this->isOneMeasure=false;
    this->fourposition=true;
    this->threeposition=false;
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
    //this->InitVariable();
    this->timeAccumulateLineEdit->setEnabled(true);
    emit StopMeasureSignal();

    timeLine->stop();
    progress->reset();
    progress->setValue(100);
    progress->setFormat(typeAlignCBox->currentText()+": Режим остановлен");

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
    //ptmr->setInterval(timeSec);
    //ptmr->setSingleShot(true);
    //ptmr->start();
    emit StartAccumulateDataSignal(timeSec);
    timeLine->setDuration(timeAccumulateLineEdit->text().toInt()*1000);
    timeLine->start();

}
//-----------------------------------------------------------
// Назначение: установка времени накопления данных
//-----------------------------------------------------------
bool Widget::SetTime()
{
    if(!timeAccumulateLineEdit->text().isEmpty())
    {
        if(fourposition){
            this->timeSec=(timeAccumulateLineEdit->text().toDouble()/**1000*/)/4.;
        }
        else if (threeposition) {
            this->timeSec=(timeAccumulateLineEdit->text().toDouble()/**1000*/)/3.;
        }
        this->timeAccumulateLineEdit->setEnabled(false);
        return true;
    }
    return false;
}

//-----------------------------------------------------------
// Назначение: расчет параметров
//-----------------------------------------------------------
void Widget::Measure()
{
    //emit StopAccumulateDataSignal();

    if(fourposition){
    switch (numPosition) {
    case DEG_180:
        da2_pos0=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition=DEG_270;
        emit GotoPosition(-180);
        break;
    case DEG_270:
        da2_pos180=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition=DEG_0;
        emit GotoPosition(-270);
        break;
    case DEG_90:
        da2_pos90=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition=DEG_180;
        emit GotoPosition(-90);
        break;
    case DEG_0:
        da2_pos270=this->ConfigGyroDevice->Measure->diff;
        this->ConfigGyroDevice->Measure->diff=0;
        numPosition=DEG_90;
        numMeasure++;
        emit GotoPosition(0);
        break;
    default: break;
    }
    if(numMeasure>prevMeasure){
        prevMeasure=numMeasure;

        //пересчет параметров
        Azimuth=qRadiansToDegrees(atan2((da2_pos270-da2_pos90),(da2_pos0-da2_pos180)));
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

        numerator+=pow((Azimuth-MeanAzimuth),2);
        numMeasure>1?denumerator=numMeasure-1:denumerator=1;
        SKO=sqrt((numerator/denumerator));

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
        if(this->isOneMeasure){
            this->isOneMeasure=false;
            this->StopMeasureSlot();
        }
    }
    }
    else if(threeposition){
        switch (numPosition) {
        case DEG_90:
            da2_pos0=this->ConfigGyroDevice->Measure->diff;
            dv1_pos0=this->ConfigGyroDevice->Measure->diffDvX;
            dv2_pos0=this->ConfigGyroDevice->Measure->diffDvY;
            this->ConfigGyroDevice->Measure->diff=0;
            this->ConfigGyroDevice->Measure->diffDvX=0;
            this->ConfigGyroDevice->Measure->diffDvY=0;

            numPosition=DEG_180;
            emit GotoPosition(-90);//todo передалать в правильный угол
            break;

        case DEG_180:
            da2_pos90=this->ConfigGyroDevice->Measure->diff;
            dv1_pos90=this->ConfigGyroDevice->Measure->diffDvX;
            dv2_pos90=this->ConfigGyroDevice->Measure->diffDvY;
            this->ConfigGyroDevice->Measure->diff=0;
            this->ConfigGyroDevice->Measure->diffDvX=0;
            this->ConfigGyroDevice->Measure->diffDvY=0;

            numPosition=DEG_0;
            emit GotoPosition(-180);//todo передалать в правильный угол
            break;
        case DEG_0:
            da2_pos180=this->ConfigGyroDevice->Measure->diff;
            dv1_pos180=this->ConfigGyroDevice->Measure->diffDvX;
            dv2_pos180=this->ConfigGyroDevice->Measure->diffDvY;
            this->ConfigGyroDevice->Measure->diff=0;
            this->ConfigGyroDevice->Measure->diffDvX=0;
            this->ConfigGyroDevice->Measure->diffDvY=0;

            numPosition=DEG_90;
            emit GotoPosition(0);//todo передалать в правильный угол
            numMeasure++;
            break;
        } //switch (numPosition)
        if(numMeasure>prevMeasure){
            prevMeasure=numMeasure;
            //пересчет G
            static double
                    go=9.78049,
                    earth_a=6378136.,
                    betta=0.005317,
                    alpha=0.000007;
            G=(go*earth_a*earth_a)/(earth_a+H)/(earth_a+H)*
                    (1.+betta*sin(Lat)*sin(Lat)+alpha*sin(2*Lat)*sin(2*Lat));
            //пересчет крена и тангажа
            Roll=asin(0.25/G*(-2*dv1_pos90+dv1_pos180+dv1_pos0-dv2_pos180+dv2_pos0));
            Pitch=asin(0.25/G/cos(Roll)*(dv1_pos180-dv1_pos0-2*dv2_pos90+dv2_pos180+dv2_pos0));

            //азимута
            double n=0.,c1=0.,s1=0., d1=0.,c2=0.,s2=0.,d2=0.;
            double a=0.,b=0.,e=0.;
            double delta=0.;
            n=(da2_pos90-da2_pos0/da2_pos90-da2_pos180);
            c1=(sin(Roll)*sin(Pitch)-cos(Pitch))*cos(Lat);
            s1=-cos(Roll)*cos(Lat);
            d1=-(sin(Roll)*cos(Pitch)+sin(Pitch))*sin(Lat);

            c2=(sin(Roll)*sin(Pitch)+cos(Pitch))*cos(Lat);
            s2=s1;
            d2=(-sin(Roll)*cos(Pitch)+sin(Pitch))*sin(Lat);

            a=c1-n*c2;
            b=s1*(1-n);
            e=n*d2-d1;

            if((da2_pos90-da2_pos180)>0){
                Azimuth=-asin(e/sqrt(a*a+b*b))-asin(a/sqrt(a*a+b*b))+M_PI;
                delta=fabs(a*cos(Azimuth)+b*sin(Azimuth)-e);
                if(delta>1e-11){
                    Azimuth=-asin(e/sqrt(a*a+b*b))+asin(a/sqrt(a*a+b*b));
                }
            }
            else{
                Azimuth=asin(e/sqrt(a*a+b*b))-asin(a/sqrt(a*a+b*b));
                delta=fabs(a*cos(Azimuth)+b*sin(Azimuth)-e);
                if(delta>1e-11){
                    Azimuth=asin(e/sqrt(a*a+b*b))+asin(a/sqrt(a*a+b*b))-M_PI;
                }
            }
            if(Azimuth<0) Azimuth=Azimuth+2*M_PI;
            Azimuth=qRadiansToDegrees(Azimuth);

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

            numerator+=pow((Azimuth-MeanAzimuth),2);
            numMeasure>1?denumerator=numMeasure-1:denumerator=1;
            SKO=sqrt((numerator/denumerator));

            currValueLineEdit->setText(QString::number(static_cast<double>(Azimuth)));
            meanVelueLineEdit->setText(QString::number(static_cast<double>(MeanAzimuth)));
            minValueLineEdit->setText(QString::number(static_cast<double>(MinAzimuth)));
            maxValueLineEdit->setText(QString::number(static_cast<double>(MaxAzimuth)));
            skoLineEdit->setText(QString::number(static_cast<double>(SKO)));

            RollLineEdit->setText(QString::number(Roll));
            PitchLineEdit->setText(QString::number(Pitch));

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
            if(this->isOneMeasure){
                this->isOneMeasure=false;
                this->StopMeasureSlot();
            }
        }
    } // else if(threeposition)
}



void Widget::OneMeasureSlot()
{
    this->isOneMeasure=true;

}

//сохранение настроек приложения
void Widget::saveSettings()
{
    //QSettings settings(ORGANIZATION_NAME,APPLICATION_NAME);
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.setValue("size",size());
    settings.beginGroup("MainWindow");
    settings.setValue("pos",pos());
    settings.endGroup();

}
//чтение настроек приложения
void Widget::readSettings()
{
    //QSettings settings(ORGANIZATION_NAME,APPLICATION_NAME);
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    resize(settings.value("size",QSize(400,400)).toSize());
    move(settings.value("pos",QPoint(200,200)).toPoint());
    settings.endGroup();
}

void Widget::recieveSnsBasicData(QByteArray data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_3);
    in>>this->Latsns>>this->Lonsns>>this->Hsns>>this->Speedsns>>this->Statussns;
    LatLabel->setText("Lat="+QString::number(Latsns,'g',8)+" ");
    LonLabel->setText("Lon="+QString::number(Lonsns,'g',8)+" ");
    HLabel->setText("H="+QString::number(Hsns,'g',6)+" ");
    if(this->Statussns=="A" || this->Statussns=="V"){
        emit sendCoordinate(&this->Latsns,&this->Lonsns,&this->Hsns);
        this->Lat=this->Latsns;
        this->Lon=this->Lonsns;
        this->H=this->Hsns;
    }

}

//прием и отображение координат спутника
void Widget::recieveCoordinate(double *Lat, double *Lon, double *H)
{
    LatLabel->setText("Lat="+QString::number(*Lat,'g',8)+" ");
    LonLabel->setText("Lon="+QString::number(*Lon,'g',8)+" ");
    HLabel->setText("H="+QString::number(*H,'g',6)+" ");

    this->Lat=*Lat;
    this->Lon=*Lon;
    this->H=*H;
}
//отображение введнных координат
void Widget::viewAngle(QString Roll, QString Pitch)
{
    RollLineEdit->setText(Roll);
    PitchLineEdit->setText(Pitch);
}

//выбор типа выставки
void Widget::selectModeAlign(int idx)
{
    if(idx==GK_1)
    {
        qDebug()<<"GK1";
        this->timeAccumulateLineEdit->setText("480");
        this->timeAccumulateLineEdit->setReadOnly(true);
    }
    else if(idx==GK_2)
    {
        qDebug()<<"GK2";
        this->timeAccumulateLineEdit->setText("1200");
        this->timeAccumulateLineEdit->setReadOnly(true);
    }
    else if (idx==REPEAT)
    {

        qDebug()<<"REPEAT";
        this->timeAccumulateLineEdit->setText("240");
        this->timeAccumulateLineEdit->setReadOnly(true);
    }
    else if (idx==CONTINUOUS)
    {
        qDebug()<<"CONTINUOUS";
        this->timeAccumulateLineEdit->setReadOnly(false);
    }
}



//выбор типа алгоритма
void Widget::selectAlgorithm()
{
    if(FourAlgAction->isChecked()){
        fourposition=true;
        threeposition=false;
    }
    else if (ThreeAlgAction->isChecked()) {
        threeposition=true;
        fourposition=false;
    }
}
//настройка прогресс-бара
void Widget::setProgress()
{
    QString text;
    QTime time;
    int hour,min,sec,msec;
    msec=timeLine->currentTime()%1000;
    sec=timeLine->currentTime()%60000 /1000;
    min=timeLine->currentTime()%3600000/60000;
    hour=timeLine->currentTime()/3600000;
    time=QTime(hour,min,sec,msec);
    qDebug()<<time.toString();
    if(typeAlignCBox->currentIndex()==GK_1) text=QString(alignmode.at(GK_1)+": %p% (%1)").arg(time.toString("hh:mm:ss.zz"));
    else if(typeAlignCBox->currentIndex()==GK_2) text=QString(alignmode.at(GK_2)+": %p% (%1)").arg(time.toString("hh:mm:ss.zz"));
    else if(typeAlignCBox->currentIndex()==REPEAT) text=QString(alignmode.at(REPEAT)+": %p% (%1)").arg(time.toString("hh:mm:ss.zz"));
    else if(typeAlignCBox->currentIndex()==CONTINUOUS) text=QString(alignmode.at(CONTINUOUS)+": %p% (%1)").arg(time.toString("hh:mm:ss.zz"));
    progress->setFormat(text);
    progress->setTextVisible(true);
    //progress->setValue(timeLine->currentFrame());

    progress->setValue(this->ConfigGyroDevice->Measure->getTick()*100/(this->timeSec*400));

}

void Widget::slotbuildgraph()
{
    emit buildgraph(0,RollLineEdit->text().toDouble());
}

void Widget::createPlot(QString name)
{
    PlotWidget *plot=new PlotWidget;
    plot->graphAdd(name);
    plot->show();
    connect(this,&Widget::buildgraph,plot,&PlotWidget::realtimeDataSlot);
    //    connect(tmrsec,&QTimer::timeout,this,&Widget::slotbuildgraph);
}
