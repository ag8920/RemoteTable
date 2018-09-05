#include "widget.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QGroupBox>

Widget::Widget(QWidget *parent)
    : QMainWindow ()
{

    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
    CreateWidgets();
}
//-----------------------------------------------------------
Widget::~Widget()
{

}
//-----------------------------------------------------------
void Widget::CreateActions()
{
    DeltaPsProtocol = new QAction(tr("Протокол \"Delta_PS\" "));

    Rate2Protocol = new QAction(tr("Протокол \"Rate_2\" "));

    DadvttProtocol = new QAction(tr("Протокол \"Dadvtt\" "));
}
//-----------------------------------------------------------
void Widget::CreateMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(DeltaPsProtocol);
    fileMenu->addAction(Rate2Protocol);
    fileMenu->addAction(DadvttProtocol);
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

    meanVelueLineEdit=new QLineEdit;
    meanVelueLineEdit->setReadOnly(true);

    minValueLineEdit=new QLineEdit;
    minValueLineEdit->setReadOnly(true);

    maxValueLineEdit=new QLineEdit;
    maxValueLineEdit->setReadOnly(true);

    skoLineEdit=new QLineEdit;
    skoLineEdit->setReadOnly(true);

    timeAcculateLineEdit=new QLineEdit;

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
    RightLayout->addWidget(timeAcculateLineEdit,0,1);

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
    measureWidget->show();



}
