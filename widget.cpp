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
    CoordDialog = new corrdDialog;
    disp = new Disp;

    CreateActions();
    CreateMenus();
    CreateToolBars();
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
    connect(SetCoordianteAction,&QAction::triggered,
            CoordDialog,&QDialog::show);
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
    QDoubleValidator* regExpDouble=new QDoubleValidator();
    regExpDouble->setLocale(QLocale::English);


    QGroupBox *leftgroupBox = new QGroupBox(tr("Измерения азимута"));
    QGroupBox *rightgroupBox = new QGroupBox(tr("Углы поворота"));

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

    RollLabel=new QLabel(tr("Значение крена:"));
    RollLineEdit=new QLineEdit;
    RollLineEdit->setReadOnly(true);
    PitchLabel=new QLabel(tr("Значение тангажа:     "));
    PitchLineEdit=new QLineEdit;
    PitchLineEdit->setReadOnly(true);


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

    LeftLayout->addWidget(countMeasureLabel,5,0);
    LeftLayout->addWidget(countMeasureLineEdit,5,1);

    LeftLayout->addWidget(timeAccumulateLabel,6,0);
    LeftLayout->addWidget(timeAccumulateLineEdit,6,1);

    RightLayout->addWidget(RollLabel,0,0);
    RightLayout->addWidget(RollLineEdit,0,1);
    RightLayout->addWidget(PitchLabel,1,0);
    RightLayout->addWidget(PitchLineEdit,1,1);

    leftgroupBox->setLayout(LeftLayout);
    rightgroupBox->setLayout(RightLayout);
    MainLayout->addWidget(leftgroupBox);
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
    connect(StartTimerAction,&QAction::triggered,
            disp,&Disp::slotMeasureStart);
    //Сигнал старта измерений (ConfigTableDevice->isMeasuring=true)

    connect(OneMeasurementAction,&QAction::triggered,
            this,&Widget::StartMeasureSlot);
    connect(OneMeasurementAction,&QAction::triggered,
            disp,&Disp::slotMeasureStart);
    connect(OneMeasurementAction,&QAction::triggered,
            disp,&Disp::slotSignleMeasure);

    connect(disp,&Disp::signalMeasureStart,
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
    connect(StopTimerAction,&QAction::triggered,
            disp,&Disp::slotMeasureStop);
    //сброс признака (TableDevice->isMeasuring=false)
    connect(disp,&Disp::signalMeasureStop,
            ConfigTableDevice, &TableDevice::StopMeasure);
    //остановка накопления данных, сброс всех параметров ГК
    connect(disp,&Disp::signalMeasureStop,
            ConfigGyroDevice->Measure,&GyroData::Stop);
    //---------------------------------
    //запуск таймера
    //---------------------------------
    //запуск таймера обратного отсчета при остановке стола
    connect(ConfigTableDevice,&TableDevice::StopRotation,
            disp,&Disp::slotStartTimer);
    //---------------------------------
    //Взаимодействие с поворотным столом
    //---------------------------------
    connect(disp,&Disp::signalGotoPosition,
            ConfigTableDevice,&TableDevice::GoToPosition);
    //сброс абс.координат
    connect(disp,&Disp::signalResetAbsCoord,
            ConfigTableDevice,&TableDevice::ResetAbsCoord);

    //----------------------------------
    //взаимодействие с гироскопом
    //-----------------------------------
    //включение накопления данных ( выполняется при остановке стола)
    connect(disp,&Disp::signalStartAccumulateData,
            ConfigGyroDevice->Measure,
            &GyroData::AccumulateData);
    //окончание накопления данных при начале поворота стола
    //вызывается в функции Measure()
    connect(disp,&Disp::signalStopAccumulateData,
            ConfigGyroDevice->Measure,
            &GyroData::NoAccumulateData);
    connect(CoordDialog,&corrdDialog::outCoordinate,
            ConfigGyroDevice->Measure,&GyroData::GetCoordinate);
    connect(ConfigGyroDevice->Measure,&GyroData::outAngle,
            this,&Widget::viewAngle);
    connect(this->timeAccumulateLineEdit,&QLineEdit::editingFinished,
            this,&Widget::setTime);

}
//-----------------------------------------------------------
// Назначение: Запуск измерений
//-----------------------------------------------------------
void Widget::StartMeasureSlot()
{
    StartTimerAction->setEnabled(false);
    OneMeasurementAction->setEnabled(false);
    StopTimerAction->setEnabled(true);
}
//-----------------------------------------------------------
// Назначение: остановка измерений
//-----------------------------------------------------------
void Widget::StopMeasureSlot()
{
    timeAccumulateLineEdit->setEnabled(true);
    StartTimerAction->setEnabled(true);
    OneMeasurementAction->setEnabled(true);
    StopTimerAction->setEnabled(false);
}

void Widget::viewAzParam(QString Az, QString meanAz, QString minAz, QString maxAz, QString skoAz, QString numMeas)
{
    currValueLineEdit->setText(Az);
    meanVelueLineEdit->setText(meanAz);
    minValueLineEdit->setText(minAz);
    maxValueLineEdit->setText(maxAz);
    skoLineEdit->setText(skoAz);
    countMeasureLineEdit->setText(numMeas);
}

void Widget::viewAngle(QString Roll, QString Pitch)
{
    RollLineEdit->setText(Roll);
    PitchLineEdit->setText(Pitch);
}

void Widget::setTime()
{
    disp->slotSetTime(timeAccumulateLineEdit->text().toInt());
}
