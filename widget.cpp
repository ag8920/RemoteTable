#include "widget.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QGroupBox>

Widget::Widget(QWidget *parent)
    : QMainWindow ()
{

    ConfigTableDevice=new TableDevice;
    ConfigGyroDevice=new GyroDevice;

    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
    CreateWidgets();
    initActionConnections();


    //ConfigTableDevice->show();

}
//-----------------------------------------------------------
Widget::~Widget()
{
    delete ConfigTableDevice;
    delete ConfigGyroDevice;
}

void Widget::closeEvent(QCloseEvent *event)
{
    onWindowClosed();
}
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
}

void Widget::initActionConnections()
{
    connect(ConfigTabelDevAction,SIGNAL(triggered()),ConfigTableDevice,SLOT(show()));
        connect(ConfigGyroDevAction,SIGNAL(triggered()),ConfigGyroDevice,SLOT(show()));
    connect(this,SIGNAL(onWindowClosed()),ConfigTableDevice,SLOT(close()));
    connect(this,SIGNAL(onWindowClosed()),ConfigGyroDevice,SLOT(close()));

}
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

    configMenu = menuBar()->addMenu(tr("&Инструменты"));
    configMenu->addAction(ConfigTabelDevAction);
    configMenu->addAction(ConfigGyroDevAction);
}
//-----------------------------------------------------------
void Widget::CreateToolBars()
{

}
//-----------------------------------------------------------
void Widget::CreateStatusBar()
{

}
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



    azimuthMeasureLineEdit=new QLineEdit;
    azimuthMeasureLineEdit->setReadOnly(true);

    LeftLayout->addWidget(currValueLabel,0,0);
    LeftLayout->addWidget(currValueLineEdit,0,1);



    LeftLayout->addWidget(meanValueLabel,1,0);
    LeftLayout->addWidget(meanVelueLineEdit,1,1);

    LeftLayout->addWidget(minValueLabel,2,0);
    LeftLayout->addWidget(maxValueLineEdit,2,1);

    LeftLayout->addWidget(skoLabel,3,0);
    LeftLayout->addWidget(skoLineEdit,3,1);

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
