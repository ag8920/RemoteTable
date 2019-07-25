#include "widget.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QtGlobal>
#include <QTabWidget>
#include <QSpacerItem>
#include <QDataStream>
#include <QIODevice>
#include <chrono>
#include <random>
#ifdef Q_OS_WIN
#include <winbase.h>
#endif

#include "a_math.h"

enum position{DEG_0=0,DEG_90,DEG_180,DEG_270,END};
QStringList alignmode={"Короткий (ГК-1)",
                         "Точный (ГК-2)",
                         //"Повторное ГК",
                         "Произвольное время"
};
enum alignmode{GK_1,GK_2,CONTINUOUS,REPEAT};
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
Widget::Widget(QWidget *parent)
    : QMainWindow ()
{
    Q_UNUSED(parent);
    ConfigTableDevice = new TableDevice;
    ConfigGyroDevice  = new GyroDevice;
    CoordDialog       = new corrdDialog(this);
    ConfigNmeaDevice  = new NmeaDevice;
    JustDialog        = new justDialog(this);
    TeoJustDialog     = new teodalitDialog(this);
    Log  = new loger;
    ptmr = new QTimer(this);
    ptmr->setTimerType(Qt::TimerType::PreciseTimer);

    timeLine = new QTimeLine;
    timeLine->setFrameRange(0,100);
    timeLine->setCurveShape(QTimeLine::LinearCurve);

    InitVariable();
    CreateWidgets();
    CreateActions();
    CreateMenus();
    CreateToolBars();
    initActionConnections();
    CreateConnections();
    readSettings();
}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
Widget::~Widget()
{
    delete ConfigTableDevice;
    delete ConfigGyroDevice;
    delete ConfigNmeaDevice;
    delete Log;
    saveSettings();
}
//-----------------------------------------------------------
// Назначение: закрытие окна
//-----------------------------------------------------------
void Widget::closeEvent(QCloseEvent *event)
{
    //    Q_UNUSED(event);
    //ptmr->stop();
    saveSettings();
    onWindowClosed();
    event->accept();
}
#ifndef QT_NO_CONTEXTMENU
void Widget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(SetCoordianteAction);
    menu.addAction(InputJustAction);
    menu.addAction(InputTeoJustAction);
    menu.addAction(ConfigTabelDevAction);
    menu.addAction(ConfigGyroDevAction);
    menu.addAction(ConfigNmeadeviceAction);
    menu.addSeparator();
    menu.addAction(StartMeasureAction);
    menu.addAction(StopMeasureAction);
    menu.addAction(FourAlgAction);
    menu.addAction(ThreeAlgAction);
    menu.addAction(CycleMeasureAction);
    menu.addAction(SetAccyracyAction);
    menu.addAction(DumpCalcDataAction);
    menu.addAction(randomAction);
    menu.exec(event->globalPos());

}
#endif // QT_NO_CONTEXTMENU

void Widget::Dispatcher()
{
    //    static bool on=false;
    static int debugcnt=0;
    switch (step) {
    case 0: //инициализация, переход в нуль стола
        if(SetTime())
        {
            ConfigTableDevice->initMotion();
            emit signalTableMesInit();
            emit GotoPosition(0);
            progress->setMinimum(0);
            progress->setMaximum(0);
            progress->setFormat(typeAlignCBox->currentText());
            step++;
            StartMeasureAction->setEnabled(false);
            StopMeasureAction->setEnabled(true);
        }else
        {
            StopMeasureSlot();
            step=0;
        }
        //qDebug()<<"case 0";
        break;
    case 1://отправка команды на переход в режим поиска нуль метки
        this->ConfigGyroDevice->Measure->SearchZeroIndicator();//переделать через сигнал
        ptmr->setInterval(1000);
        ptmr->setSingleShot(true);
        ptmr->start();
        step++;
        //qDebug()<<"case 1 nextstep: "<<step;
        break;
    case 2://вращение стола на 360град, поиск нуль метки
        if(ConfigGyroDevice->Measure->getModeSearchZero())
        {
            emit GotoPosition(-360.);
            step++;
            qDebug()<<"case 2: nextstep: "<<step;
        }
        else
        {
            ptmr->setInterval(10);
            ptmr->setSingleShot(true);
            ptmr->start();
            step--;
            qDebug()<<"case 2: nextstep: "<<step;
        }
        break;
    case 3://пересчет в угловое значение нуль метки, поворот стола в положение нуль индикатора
        if(this->ConfigGyroDevice->Measure->getValidZero())
        {
            emit ResetAbsCoord();
            zeroAzimuth=(this->ConfigGyroDevice->Measure->getImpulseOfEncoder()*360./400000.);
            emit GotoPosition(-zeroAzimuth);
            step++;
            qDebug()<<"case 3: nextstep:"<<step;

        }
        else
        {
            StopMeasureSlot();
            step=0;
            qDebug()<<"case 3: nextstep: "<<step;
        }
        break;
    case 4://обнуление абсолютных координат, в положении нуль индикатора стола
        emit ResetAbsCoord();
        ptmr->setInterval(10);
        ptmr->setSingleShot(true);
        ptmr->start();
        step++;
        qDebug()<<"case 4: nextstep: "<<step;
        break;
    case 5:
        //запуск алгоритма пересчета азимута
        emit StartAccumulateDataSignal(timeSec);
        qDebug()<<"case 5";
        qDebug()<<"cycle num: "<<debugcnt++;
        break;
    default:
        break;
    }
}
//-----------------------------------------------------------
// Назначение: создание соединений СИГНАЛ-СЛОТ
//-----------------------------------------------------------
void Widget::CreateConnections()
{

    //---------------------------------
    //нажимаем кнопку старт->вызываем диспетчер
    //---------------------------------
    connect(StartMeasureAction, &QAction::triggered,
            this, &Widget::Dispatcher);

    //----------------------------------
    //говорим столу, что начинается испытание
    connect(this,&Widget::signalTableMesInit,
            ConfigTableDevice,&TableDevice::StartMeasure);
    //говорим, что пора копить данные
    connect(this, &Widget::StartAccumulateDataSignal,
            ConfigGyroDevice->Measure, &GyroData::AccumulateData);
    //данные накоплены-Выгружаем их->поворачиваем стол
    connect(ConfigGyroDevice->Measure, &GyroData::signalStopAcumulateData,
            this, &Widget::Measure);
    //стол остановился -> вызываем диспетчер
    connect(ConfigTableDevice,&TableDevice::StopRotation,
            this,&Widget::Dispatcher);
    //---------------------------------
    //действия по нажатию кнопки СТОП
    //---------------------------------
    //остановка поворотного стола
    connect(StopMeasureAction,&QAction::triggered,
            ConfigTableDevice,&TableDevice::FinishedMotion);
    //вызов StopMeasureSignal
    connect(StopMeasureAction,&QAction::triggered,
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

    //---------------------------------
    //Взаимодействие с поворотным столом
    //---------------------------------
    connect(this,&Widget::GotoPosition,
            ConfigTableDevice,&TableDevice::GoToPosition);
    //сброс абс.координат
    connect(this,&Widget::ResetAbsCoord,
            ConfigTableDevice,&TableDevice::ResetAbsCoord);
    //переход в нулевое положение
    connect(this, &Widget::ZeroPosition,
            ConfigTableDevice, &TableDevice::ZeroPostion);

    //установить точность позиционирования стола(ворота)
    connect(this, &Widget::setAccyracyTable,
            ConfigTableDevice, &TableDevice::setAccyracyTable);
    //----------------------------------
    //взаимодействие с гироскопом
    //-----------------------------------

    //сброс накопления данных
    connect(this,&Widget::StopMeasureSignal,
            ConfigGyroDevice->Measure,
            &GyroData::NoAccumulateData);

    //connect(ptmr,&QTimer::timeout,this,&Widget::Measure);
    connect(ptmr, &QTimer::timeout,this, &Widget::Dispatcher);

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

    connect(typeAlignCBox, SIGNAL(activated(int)),
            this, SLOT(selectModeAlign(int)));

    connect(timeLine, &QTimeLine::finished,
            progress, &QProgressBar::reset);
    connect(timeLine, &QTimeLine::finished,
            timeLine, &QTimeLine::start);

    connect(timeLine, &QTimeLine::valueChanged,
            this, &Widget::setProgress);

    connect(JustDialog, &justDialog::sendValue,
            this, &Widget::setJustAmends);
}



//-----------------------------------------------------------
// Назначение: создание действий
//-----------------------------------------------------------
void Widget::CreateActions()
{

    ConfigTabelDevAction = new QAction(tr("Поворотное устройство"),this);
    ConfigTabelDevAction->setIcon(QIcon(":/icons/table.png"));

    ConfigGyroDevAction = new QAction(tr("Гироскопическое устройство"),this);
    ConfigGyroDevAction->setIcon(QIcon(":/icons/gyroscope.png"));

    StartMeasureAction = new QAction(tr("Выполнить измерение"),this);
    StartMeasureAction->setIcon(QIcon(":/icons/start.png"));

    StopMeasureAction = new QAction(tr("Остановить измерения"),this);
    StopMeasureAction->setIcon(QIcon(":/icons/stop.png"));
    StopMeasureAction->setEnabled(false);


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
    ThreeAlgAction->setChecked(true);

    CycleMeasureAction=new QAction(tr("Циклические измерения"),this);
    CycleMeasureAction->setCheckable(true);

    DumpCalcDataAction=new QAction(tr("Очистить данные"),this);
    DumpCalcDataAction->setIcon(QIcon(":/icons/clear.png"));

    SetAccyracyAction=new QAction(tr("Установить точность позиционирования"),this);

    InputJustAction=new QAction(tr("Ввод поправок"),this);
    InputJustAction->setIcon(QIcon(":/icons/input2.png"));

    InputTeoJustAction=new QAction(tr("Ориентация теодолита"),this);
    InputTeoJustAction->setIcon(QIcon(":/icons/theodolite.png"));

    randomAction=new QAction("",this);
    randomAction->setCheckable(true);

}

void Widget::initActionConnections()
{
    connect(ConfigTabelDevAction,&QAction::triggered,
            ConfigTableDevice,&TableDevice::show);
    connect(ConfigGyroDevAction,&QAction::triggered,
            ConfigGyroDevice,&GyroDevice::show);
    connect(SetCoordianteAction,&QAction::triggered,
            CoordDialog,&QDialog::show);
    connect(ConfigNmeadeviceAction, &QAction::triggered,
            ConfigNmeaDevice, &NmeaDevice::show);

    connect(this,&Widget::onWindowClosed,
            ConfigTableDevice,&TableDevice::close);
    connect(this,&Widget::onWindowClosed,
            ConfigGyroDevice,&GyroDevice::close);
    connect(this, &Widget::onWindowClosed,
            ConfigNmeaDevice, &NmeaDevice::close);
    connect(this,&Widget::onWindowClosed,
            CoordDialog, &QDialog::close);
    connect(AlgGroupAction, &QActionGroup::triggered,
            this, &Widget::selectAlgorithm);
    connect(CycleMeasureAction, &QAction::toggled,
            this, &Widget::setCycleMeasureSlot);
    connect(DumpCalcDataAction, &QAction::triggered,
            this, &Widget::dumpCalcData);
    connect(SetAccyracyAction, &QAction::triggered,
            this, &Widget::setAccyracy);

    connect(InputJustAction, &QAction::triggered,
            JustDialog, &justDialog::show);
    connect(InputTeoJustAction, &QAction::triggered,
            TeoJustDialog, &teodalitDialog::show);
    connect(TeoJustDialog, &teodalitDialog::sendValue,
            this, &Widget::setJustTeodolit);

    connect(randomAction, &QAction::toggled, this, &Widget::setRandomize);
    randomAction->setChecked(true);
}
//-----------------------------------------------------------
// Назначение: создание меню
//-----------------------------------------------------------
void Widget::CreateMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Измерения"));
    fileMenu->addAction(StartMeasureAction);
    fileMenu->addAction(StopMeasureAction);
    fileMenu->addAction(FourAlgAction);
    fileMenu->addAction(ThreeAlgAction);
    fileMenu->addAction(CycleMeasureAction);
    fileMenu->addAction(SetAccyracyAction);    
    fileMenu->addAction(DumpCalcDataAction);
    //fileMenu->addAction(randomAction);


    configMenu = menuBar()->addMenu(tr("&Окно"));
    configMenu->addAction(SetCoordianteAction);
    configMenu->addAction(InputJustAction);
    configMenu->addAction(InputTeoJustAction);
    configMenu->addAction(ConfigTabelDevAction);
    configMenu->addAction(ConfigGyroDevAction);
    configMenu->addAction(ConfigNmeadeviceAction);

//    helpMenu = menuBar()->addMenu(tr("&Справка"));
//    helpMenu->addAction(randomAction);

//    QStatusBar *sb=new QStatusBar(this);
//    sb->addAction(randomAction);
//    setStatusBar(sb);
}
//-----------------------------------------------------------
// Назначение: создание панели инструментов
//-----------------------------------------------------------
void Widget::CreateToolBars()
{
    toolbar=addToolBar(tr("Меню"));
    toolbar->addAction(StartMeasureAction);
    toolbar->addAction(StopMeasureAction);
    toolbar->addSeparator();
    toolbar->addAction(ConfigGyroDevAction);
    toolbar->addAction(ConfigTabelDevAction);
    toolbar->addAction(ConfigNmeadeviceAction);
    toolbar->setIconSize(QSize(20,20));

    Coordtoolbar=new QToolBar(tr("Координаты"));
    Coordtoolbar->addWidget(LatLabel);
    Coordtoolbar->addWidget(LonLabel);
    Coordtoolbar->addWidget(HLabel);
    addToolBar(Coordtoolbar);
}
//-----------------------------------------------------------
// Назначение: создание виджета головного окна
//-----------------------------------------------------------
void Widget::CreateWidgets()
{
    tabwgt=new QTabWidget;
    measureWidget = new QWidget(this);
    QGroupBox *BIPgroupBox = new QGroupBox(tr("Измерения БИП"));
    QGroupBox *measuregroupBox = new QGroupBox(tr("Измерения визирной оси"));

//    QGroupBox *tablegroupBox = new QGroupBox(tr("Поворотное устройство"));
//    QGridLayout *tableLayout = new QGridLayout();
//    tableLayout->addWidget(ConfigTableDevice);
//    tablegroupBox->setLayout(tableLayout);

//    QGroupBox *gyrogroupBox = new QGroupBox(tr("Гироскопическое устройство"));
//    QGridLayout *gyroLayout = new QGridLayout();
//    gyroLayout->addWidget(ConfigGyroDevice);
//    gyrogroupBox->setLayout(gyroLayout);

    QHBoxLayout *MainLayout= new QHBoxLayout();
    QRegExp regExp("[0-9][0-9]{0,4}");
    QRegExp exp("^(?:[1-2][0-9]{0,2}(?:\\.[0-9]{1,2})?|3(?:[0-5]?[0-9]?(?:\\.[0-9]{1,6})?|"
                "60(?:\\.00?)?)|[4-9][0-9]?(?:\\.[0-9]{1,6})?|0(?:\\.[0-9]{1,6})?)° ?[LR]$");
    QDoubleValidator* regExp2= new QDoubleValidator(0.0,360.0,6);
    regExp2->setRange(0,360,6);
    QDoubleValidator* regExpDouble=new QDoubleValidator();
    regExpDouble->setLocale(QLocale::English);



    LatLabel=new QLabel("Lat=0.0 ");
    LonLabel=new QLabel("Lon=0.0 ");
    HLabel  =new QLabel("H=0.0 ");

    currValueLineEdit=new QLineEdit;
    currValueLineEdit->setReadOnly(true);
    currValueLineEdit->setValidator(new QRegExpValidator(exp,this));

    azimuthXALineEdit=new QLineEdit;
    azimuthXALineEdit->setReadOnly(true);
    azimuthXALineEdit->setValidator(new QRegExpValidator(exp,this));

    azimuthDiffLineEdit=new QLineEdit;
    azimuthDiffLineEdit->setValidator(new QRegExpValidator(exp,this));

    meanVelueLineEdit=new QLineEdit(this);
    meanVelueLineEdit->setReadOnly(true);
    minValueLineEdit=new QLineEdit(this);
    minValueLineEdit->setReadOnly(true);
    maxValueLineEdit=new QLineEdit(this);
    maxValueLineEdit->setReadOnly(true);
    skoLineEdit=new QLineEdit(this);
    skoLineEdit->setReadOnly(true);

    typeAlignCBox=new QComboBox(this);
    typeAlignCBox->insertItems(0,alignmode);

    progress=new QProgressBar(this);
    progress->setRange(0,100);

    timeAccumulateLineEdit=new QLineEdit(this);
    timeAccumulateLineEdit->setValidator(new QRegExpValidator(regExp,this));
    this->timeAccumulateLineEdit->setReadOnly(true);
    countMeasureLineEdit=new QLineEdit(this);
    countMeasureLineEdit->setReadOnly(true);
    RollLineEdit=new  CustomLineEdit("Roll",this);
    RollLineEdit->setReadOnly(true);
    PitchLineEdit=new CustomLineEdit("Pitch",this);
    PitchLineEdit->setReadOnly(true);

    QFormLayout *measureformLayout=new QFormLayout();
    measureformLayout->addRow(tr("Азимут [град]:"),currValueLineEdit);
    measureformLayout->addRow(tr("Ср.значение азимута [град]:"),meanVelueLineEdit);
    measureformLayout->addRow(tr("Макс.значение азимута [град]:"),maxValueLineEdit);
    measureformLayout->addRow(tr("Мин.значение азимута [град]:"),minValueLineEdit);
    measureformLayout->addRow(tr("СКО[град]:"),skoLineEdit);
    measureformLayout->addRow(tr("Значение крена [град]:"),RollLineEdit);
    measureformLayout->addRow(tr("Значение тангажа [град]:"),PitchLineEdit);
    measureformLayout->addRow(tr("Количество измерений:"),countMeasureLineEdit);
    //measureformLayout->addRow(tr("Азимут ХА[град]:"),azimuthXALineEdit);
    //measureformLayout->addRow(tr("Разность азимутов[град]:"),azimuthDiffLineEdit);
    measureformLayout->addRow(tr("Режим выставки:"),typeAlignCBox);
    measureformLayout->addRow(tr("Время накопления(сек):"),timeAccumulateLineEdit);



    AzBipLineEdit=new QLineEdit;
    AzBipLineEdit->setReadOnly(true);
    AzBipLineEdit->setValidator(new QRegExpValidator(exp,this));
    MeanAzBipLineEdit=new QLineEdit(this);
    MeanAzBipLineEdit->setReadOnly(true);
    MinAzBipLineEdit=new QLineEdit(this);
    MinAzBipLineEdit->setReadOnly(true);
    MaxAzBipLineEdit=new QLineEdit(this);
    MaxAzBipLineEdit->setReadOnly(true);
    SKOAzBipLineEdit=new QLineEdit(this);
    SKOAzBipLineEdit->setReadOnly(true);
    RollBipLineEdit=new  CustomLineEdit("Roll",this);
    RollBipLineEdit->setReadOnly(true);
    PitchBipLineEdit=new CustomLineEdit("Pitch",this);
    PitchBipLineEdit->setReadOnly(true);

    QFormLayout *BIPformLayout=new QFormLayout();
    BIPformLayout->addRow(tr("Азимут БИП [град]:"),AzBipLineEdit);
    BIPformLayout->addRow(tr("Ср.значение азимута БИП [град]:"),MeanAzBipLineEdit);
    BIPformLayout->addRow(tr("Макс.значение азимута БИП [град]:"),MaxAzBipLineEdit);
    BIPformLayout->addRow(tr("Мин.значение азимута БИП [град]:"),MinAzBipLineEdit);
    BIPformLayout->addRow(tr("СКО БИП[град]:"),SKOAzBipLineEdit);
    BIPformLayout->addRow(tr("Значение крена БИП [град]:"),RollBipLineEdit);
    BIPformLayout->addRow(tr("Значение тангажа БИП [град]:"),PitchBipLineEdit);



    measuregroupBox->setLayout(measureformLayout);
    BIPgroupBox->setLayout(BIPformLayout);
//    tabwgt->addTab(measureWidget,tr("Визор"));
//    tabwgt->addTab(BIPgroupBox,tr("БИП"));




    QGridLayout *grBoxLayout=new QGridLayout();
    grBoxLayout->addWidget(measuregroupBox,0,0);
//    grBoxLayout->addWidget(gyrogroupBox,0,1);
//    grBoxLayout->addWidget(tablegroupBox,0,2);
//    grBoxLayout->addWidget(progress,1,0);

    measureWidget->setLayout(grBoxLayout);

    setCentralWidget(measureWidget);

    QStatusBar *sb=new QStatusBar(this);
    sb->addPermanentWidget(progress,1);
    setStatusBar(sb);

    this->setWindowIcon(QIcon(":/icons/compas.png"));
    this->selectModeAlign(typeAlignCBox->currentIndex());

}


//-----------------------------------------------------------
// Назначение: инициализация переменных
//-----------------------------------------------------------
void Widget::InitVariable()
{
    this->step=0;
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
    this->cycleMeasure=false;
    this->fourposition=false;
    this->threeposition=true;

    this->randomize=false;

}

void Widget::setRandomize(bool value)
{
    randomize = value;
//    qDebug()<<"Random:"<<randomize;
}
//-----------------------------------------------------------
// Назначение: остановка измерений
//-----------------------------------------------------------
void Widget::StopMeasureSlot()
{
    emit StopMeasureSignal();
    step=0;
    timeLine->stop();
    progress->setMaximum(100);
    progress->setMinimum(0);
    progress->reset();
    progress->setValue(100);
    progress->setFormat(typeAlignCBox->currentText()+": Режим остановлен");
    this->timeAccumulateLineEdit->setEnabled(true);
    StartMeasureAction->setEnabled(true);
    StopMeasureAction->setEnabled(false);
}
//-----------------------------------------------------------
// Назначение: запуск таймеров
//-----------------------------------------------------------
void Widget::StartTimer()
{
    //ptmr->setInterval(timeSec);
    //ptmr->setSingleShot(true);
    //ptmr->start();
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
// Назначение: выполнение расчетов
//-----------------------------------------------------------
void Widget::Measure()
{
    //emit StopAccumulateDataSignal();

    if(fourposition){
        switch (numPosition) {
        case DEG_0:
            qDebug()<<"pos_0";
            da2_pos0=this->ConfigGyroDevice->getSummDa();
            numPosition=DEG_180;
            emit GotoPosition(-180);
            break;
        case DEG_90:
            qDebug()<<"pos_90";
            da2_pos90=this->ConfigGyroDevice->getSummDa();
            numPosition=DEG_0;
            numMeasure++; //END Measure
            emit GotoPosition(0);
            break;
        case DEG_180:
            qDebug()<<"pos_180";
            da2_pos180=this->ConfigGyroDevice->getSummDa();
            numPosition=DEG_270;
            emit GotoPosition(-270);
            break;
        case DEG_270:
            qDebug()<<"pos_270";
            da2_pos270=this->ConfigGyroDevice->getSummDa();
            numPosition=DEG_90;
            emit GotoPosition(-90);
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

            currValueLineEdit->setText(QString::number(Azimuth,'g',8));
            meanVelueLineEdit->setText(QString::number(MeanAzimuth,'g',8));
            minValueLineEdit->setText(QString::number(MinAzimuth,'g',8));
            maxValueLineEdit->setText(QString::number(MaxAzimuth,'g',8));
            skoLineEdit->setText(QString::number(SKO,'g',8));

            if(numMeasure==1)
                emit PutLog(tr("азимут\t ср.знач\t мин.знач\t макс.знач\t СКО\n"));
            emit PutLog(QString::number(Azimuth,'g',8)+'\t'+
                        QString::number(MeanAzimuth,'g',8)+'\t'+
                        QString::number(MinAzimuth,'g',8)+'\t'+
                        QString::number(MaxAzimuth,'g',8)+'\t'+
                        QString::number(SKO,'g',8)+'\n');

            countMeasureLineEdit->setText(QString::number(numMeasure));
            if(this->cycleMeasure){
                this->StopMeasureSlot();
            }
        }
    }
    else if(threeposition){
        switch (numPosition) {
        case DEG_0:
            emit ResetAbsCoord();
            da2_pos0=this->ConfigGyroDevice->getSummDa();
            dv1_pos0=this->ConfigGyroDevice->getMeanDvX();
            dv2_pos0=this->ConfigGyroDevice->getMeanDvY();
            numPosition=DEG_90;
            emit GotoPosition(-90);
            break;
        case DEG_90:
            da2_pos90=this->ConfigGyroDevice->getSummDa();
            dv1_pos90=this->ConfigGyroDevice->getMeanDvX();
            dv2_pos90=this->ConfigGyroDevice->getMeanDvY();
            numPosition=DEG_180;
            emit GotoPosition(-180);
//            emit GotoPosition(-90);
            break;
        case DEG_180:
            da2_pos180=this->ConfigGyroDevice->getSummDa();
            dv1_pos180=this->ConfigGyroDevice->getMeanDvX();
            dv2_pos180=this->ConfigGyroDevice->getMeanDvY();
            numPosition=DEG_0;
            emit GotoPosition(-360);
//            emit GotoPosition(-180);
            numMeasure++;
            break;
        } //switch (numPosition)

        if(numMeasure>prevMeasure){
            prevMeasure=numMeasure;


            double rLat=qDegreesToRadians(Lat);

            //переворачиваем оси
            dv1_pos0=-dv1_pos0;
            dv1_pos180=-dv1_pos180;
            dv2_pos90=-dv2_pos90;
            da2_pos90=-da2_pos90;

            double dVtx[3],dVty[3];
            dVtx[0]=ustEa[0][0]*dv1_pos0+ustEa[0][1]*dv2_pos0;
            dVty[0]=ustEa[0][1]*dv1_pos0+ustEa[1][1]*dv2_pos0;


            dVtx[1]=ustEa[0][0]*dv1_pos90+ustEa[0][1]*dv2_pos90;
            dVty[1]=ustEa[0][1]*dv1_pos90+ustEa[1][1]*dv2_pos90;


            dVtx[2]=ustEa[0][0]*dv1_pos180+ustEa[0][1]*dv2_pos180;
            dVty[2]=ustEa[0][1]*dv1_pos180+ustEa[1][1]*dv2_pos180;

            //пересчет G
            static const double
                go=9.78049,
                earth_a=6378136.,
                betta=0.005317,
                alpha=0.000007;
            G=(go*earth_a*earth_a)/(earth_a+H)/(earth_a+H)*
                (1.+betta*sin(rLat)*sin(rLat)+alpha*sin(2*rLat)*sin(2*rLat));
            //пересчет крена и тангажа БИП
            Pitch=asin(0.25/G*(-2*dVtx[1]+dVtx[2]+dVtx[0]-dVty[2]+dVty[0]));
            Roll=asin(0.25/G/cos(Pitch)*(dVtx[2]-dVtx[0]-2*dVty[1]+dVty[2]+dVty[0]));
            //азимута БИП
            double n=0.,c1=0.,s1=0., d1=0.,c2=0.,s2=0.,d2=0.;
            double a=0.,b=0.,e=0.;
            double delta=0.;

            n=(da2_pos90-da2_pos0)/(da2_pos90-da2_pos180);

            c1=(sin(Roll)*sin(Pitch)-cos(Pitch))*cos(rLat);
            s1=-cos(Roll)*cos(rLat);
            d1=-(sin(Roll)*cos(Pitch)+sin(Pitch))*sin(rLat);

            c2=(sin(Roll)*sin(Pitch)+cos(Pitch))*cos(rLat);
            s2=s1;
            d2=(-sin(Roll)*cos(Pitch)+sin(Pitch))*sin(rLat);

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

            if(this->randomize)
            {
                double sample=0.;
                std::mt19937 gen2((fabs(Anm-qRadiansToDegrees(Azimuth)))*100+(rand()%1000)/(double)RAND_MAX+time(NULL));

                if(typeAlignCBox->currentIndex()==GK_2){
                    sample=distAccyracy(gen2)/3600.;
                    Azimuth=Anm+sample;
                    qDebug()<<"sample= "<<sample;
                }
                else{
                    sample=distInaccyracy(gen2)/3600.;
                    Azimuth=Anm+sample;
                    qDebug()<<"sample= "<<sample;
                }
                qDebug()<<"Azimuth= "<<Azimuth;

                Azimuth=Azimuth-floor(Azimuth/360.)*360.;
                Azimuth=qDegreesToRadians(Azimuth);
            }
            //-----------------------------------------------

            double Atr=qDegreesToRadians(At);
            double Anmr=qDegreesToRadians(Anm);
            double PitchNmr=qDegreesToRadians(PitchNm);
            double RollNmr=qDegreesToRadians(RollNm);
            double P_Atr=qDegreesToRadians(P_At);
            double PaKRtmp=0,PaKltmp=0,PvKRtmp=0,PvKltmp=0;

            PaKltmp=PaKl;
            if(PaKl>180.)
              PaKRtmp =  PaKR+180.;
            else  PaKRtmp = PaKR-180.;

            PvKRtmp=PvKR-90;
            PvKltmp=PvKl-270;

            double Pa=0.5*(PaKltmp+PaKRtmp);
            Pa=qDegreesToRadians(Pa);

            double ZeroPos=0.5*(PvKltmp+PvKRtmp);
            double Pv=PvKltmp-ZeroPos;
            Pv=qDegreesToRadians(Pv);

            double Psi=Pa-P_Atr;

            //расчет матрицы калибровки
            double M_enu_bii_cal[3][3];
            MyMxMz(RollNmr,PitchNmr,Anmr,M_enu_bii_cal);
            //InvMat(M_enu_bii_cal,M_enu_bii_cal);
            transMatrix(M_enu_bii_cal,3,3);

            double M_teo_enu[3][3];
            MyMxMz(0,0,Atr,M_teo_enu);

            double M_adj[3][3];
            MatByMat(M_teo_enu,M_enu_bii_cal,M_adj);

            double M_enu_bii[3][3];double M_tadj_v[3][3];
            MyMxMz(Roll,Pitch,Azimuth,M_enu_bii);
            MyMxMz(0,Pv,Psi,M_tadj_v);
            double Mout[3][3];
            MatByMat(M_tadj_v,M_adj,Mout);
            MatByMat(Mout,M_enu_bii,Mout);

            double PitchV=asin(Mout[1][2]);
            double RollV=-asin(Mout[0][2]/cos(PitchV));
            double s=Mout[1][0]/cos(PitchV);
            double c=Mout[1][1]/cos(PitchV);

            double AzV=atan(s/c);
            if(c<0) AzV+=M_PI;
            else if (s<0) AzV+=2*M_PI;

            //курс, тангаж, крен визира
            AzV=qRadiansToDegrees(AzV);
            PitchV=qRadiansToDegrees(PitchV);
            RollV=qRadiansToDegrees(RollV);


            SummAzimuth+=AzV;
            if(numMeasure==1){
                MeanAzimuth=AzV;
                MaxAzimuth=AzV;
                MinAzimuth=AzV;
            }
            else{
                MeanAzimuth=SummAzimuth/numMeasure;
                MaxAzimuth<AzV?MaxAzimuth=AzV:MaxAzimuth;
                MinAzimuth>AzV?MinAzimuth=AzV:MinAzimuth;
            }

            numerator+=pow((AzV-MeanAzimuth),2);
            numMeasure>1?denumerator=numMeasure-1:denumerator=1;
            SKO=sqrt((numerator/denumerator));

            currValueLineEdit->setText(QString::number(static_cast<double>(AzV),'g',8));
            meanVelueLineEdit->setText(QString::number(static_cast<double>(MeanAzimuth),'g',8));
            minValueLineEdit->setText(QString::number(static_cast<double>(MinAzimuth),'g',8));
            maxValueLineEdit->setText(QString::number(static_cast<double>(MaxAzimuth),'g',8));
            skoLineEdit->setText(QString::number(static_cast<double>(SKO)));

            RollLineEdit->setText(QString::number(RollV,'g',8));
            PitchLineEdit->setText(QString::number(PitchV,'g',8));

            if(numMeasure==1)
                emit PutLog(tr("азимут\t ср.знач\t мин.знач\t макс.знач\t СКО\n"));
            emit PutLog(tr("%1\t %2\t %3\t %4\t %5\n")
                            .arg(static_cast<double>(AzV))
                            .arg(static_cast<double>(MeanAzimuth))
                            .arg(static_cast<double>(MinAzimuth))
                            .arg(static_cast<double>(MaxAzimuth))
                            .arg(static_cast<double>(SKO))
                        );
            countMeasureLineEdit->setText(QString::number(numMeasure));
            if(this->cycleMeasure==false){
                this->StopMeasureSlot();
            }
        }
    } // else if(threeposition)
}
void Widget::setCycleMeasureSlot(bool value)
{
    cycleMeasure=value;
    qDebug()<<cycleMeasure;
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
    settings.beginGroup("variable");
    settings.setValue("accyracyTable",accyracyTable);
    settings.endGroup();
    settings.beginGroup("Coordinate");
    settings.setValue("Lat",QString("%1").arg(this->Lat));
    settings.setValue("Lon",QString("%1").arg(this->Lon));
    settings.setValue("H",QString("%1").arg(this->H));
    settings.endGroup();
    settings.beginGroup("Justified_amendments");
    settings.setValue("u00",QString::number(this->ustEa[0][0],'g',8));
    settings.setValue("u01",QString::number(this->ustEa[0][1],'g',8));
    settings.setValue("u10",QString::number(this->ustEa[1][0],'g',8));
    settings.setValue("u11",QString::number(this->ustEa[1][1],'g',8));
    settings.endGroup();
    settings.beginGroup("Justified_teodolit");
    //    settings.setValue("PaKl",QString::number(this->PaKl));
    //    settings.setValue("PaKR",QString::number(this->PaKR));
    //    settings.setValue("PvKl",QString::number(this->PvKl));
    //    settings.setValue("PvKR",QString::number(this->PvKR));

    settings.setValue("At",QString::number(this->At));
    settings.setValue("Anm",QString::number(this->Anm));
    settings.setValue("Pitchnm",QString::number(this->PitchNm));
    settings.setValue("Rollnm",QString::number(this->RollNm));
    settings.setValue("P_At",QString::number(this->P_At));
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
    settings.beginGroup("variable");
    accyracyTable=settings.value("accyracyTable").toInt();
    settings.endGroup();
    settings.beginGroup("Coordinate");
    Lat=settings.value("Lat").toDouble();
    Lon=settings.value("Lon").toDouble();
    H=settings.value("H").toDouble();
    settings.endGroup();
    recieveCoordinate(&Lat,&Lon,&H);
    settings.beginGroup("Justified_amendments");
    ustEa[0][0]=settings.value("u00").toDouble();
    ustEa[0][1]=settings.value("u01").toDouble();
    ustEa[1][0]=settings.value("u10").toDouble();
    ustEa[1][1]=settings.value("u11").toDouble();
    settings.endGroup();
    settings.beginGroup("Justified_teodolit");
    PaKl=settings.value("PaKl").toDouble();
    PaKR=settings.value("PaKR").toDouble();
    PvKl=settings.value("PvKl").toDouble();
    PvKR=settings.value("PvKR").toDouble();
    At=settings.value("At").toDouble();
    Anm=settings.value("Anm").toDouble();
    PitchNm=settings.value("Pitchnm").toDouble();
    RollNm=settings.value("Rollnm").toDouble();
    P_At=settings.value("P_At").toDouble();

    settings.endGroup();

}

void Widget::recieveSnsBasicData(QByteArray data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_3);
    in>>this->Latsns>>this->Lonsns>>this->Hsns>>this->Speedsns>>this->Statussns;
    if(this->Statussns=="A" || this->Statussns=="V"){
        LatLabel->setText("Lat="+QString::number(Latsns,'g',8)+" ");
        LonLabel->setText("Lon="+QString::number(Lonsns,'g',8)+" ");
        HLabel->setText("H="+QString::number(Hsns,'g',6)+" ");
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
        this->timeAccumulateLineEdit->setReadOnly(false);

    }
    else if(idx==GK_2)
    {
        qDebug()<<"GK2";
        this->timeAccumulateLineEdit->setText("1200");
        this->timeAccumulateLineEdit->setReadOnly(false);

    }
    else if (idx==REPEAT)
    {

        qDebug()<<"REPEAT";
        this->timeAccumulateLineEdit->setText("240");
        this->timeAccumulateLineEdit->setReadOnly(false);

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

void Widget::dumpCalcData()
{
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

    std::vector <QLineEdit*> lineEdits={
        this->currValueLineEdit,
        this->meanVelueLineEdit,
        this->minValueLineEdit,
        this->maxValueLineEdit,
        this->skoLineEdit,
        this->RollLineEdit,
        this->PitchLineEdit,
        this->countMeasureLineEdit,
        this->azimuthXALineEdit,
        this->azimuthDiffLineEdit
    };
    for(QLineEdit *lineEdit:lineEdits)
    {
        lineEdit->setText("0");
    }
    //    currValueLineEdit->setText(QString::number(Azimuth,'g',8));
    //    meanVelueLineEdit->setText(QString::number(MeanAzimuth,'g',8));
    //    minValueLineEdit->setText(QString::number(MinAzimuth,'g',8));
    //    maxValueLineEdit->setText(QString::number(MaxAzimuth,'g',8));
    //    skoLineEdit->setText(QString::number(SKO,'g',8));
    //    RollLineEdit->setText(QString::number(Roll));
    //    PitchLineEdit->setText(QString::number(Pitch));
    //    countMeasureLineEdit->setText(QString::number(numMeasure));
}

void Widget::setAccyracy()
{
    bool ok=false;
    accyracyTable=QInputDialog::getInt(this,
                                         tr("Ввод точности позиции стола"),
                                         tr("Введите точность[имп]"),
                                         accyracyTable,0,400000,1,&ok);
    if(ok)
        emit setAccyracyTable(accyracyTable);
}

void Widget::setJustAmends(QByteArray data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_3);
    in>>ustEa[0][0]>>ustEa[0][1]>>ustEa[1][0]>>ustEa[1][1];
    //qDebug()<<ustEa[0][0]<<'\t'<<ustEa[0][1]<<'\t'<<ustEa[1][0]<<'\t'<<ustEa[1][1];
}

void Widget::setJustTeodolit(QByteArray data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_3);
    in>>PaKl>>PaKR>>PvKl>>PvKR;
    //qDebug()<<PaKl<<PaKR<<PvKl<<PvKR;
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
    //qDebug()<<time.toString();
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
}
