/*!
*   @file
*   @brief Класс создает виджет главного окна приложения
*   @author Щербаков Александр
*   @version 1.0
*   @date 13.09.2018
*/
#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QWidget>
#include <QtGui>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

#include "TableDev/tabledevice.h"
#include "GyroDev/gyrodevice.h"
#include "Timer/ptimer.h"


class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    void onWindowClosed();///<сигнал закрытия окна приложения
    void StartMeasure();///<сигнал начала измерений
    void StopMeasureSignal();///<сигнал остановки измерений
    void StartAccumulateDataSignal();///<сигнал запуска накопления данных
    void StopAccumulateDataSignal();///<сигнал остановки накопления данных
    void GotoPosition(QVariant position);///<сигнал перехода к след. позиции стола
    void ResetAbsCoord();///<сигнал сброса абсолютных координат стола
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void StartMeasureSlot();///<запускает измерения
    void StartTimer();///<запускает таймер и вызывает признак накопления данных
    bool SetTime();///<устанавливает время накопления данных
    void StopMeasureSlot();///<останавливает измерения
    void Measure();///<реализует четырех позиционный алгоритм
private slots:
    void OneMeasureSlot();///<устанавливает признак однократного измерения
public:
    int timeSec;
private:
    void CreateActions();///<создает действия
    void initActionConnections();///<подключает слоты к действиям
    void CreateMenus();///<создание меню
    void CreateToolBars();///<создание панели инструментов
    void CreateStatusBar();
    void CreateWidgets();///<создание виджета окна
    void CreateConnections();///<создание сигнально-слотовых соединений
    void InitVariable();///<инициализация переменных

    QAction *DeltaPsProtocolAction;
    QAction *Rate2ProtocolAction;
    QAction *DadvttProtocolAction;

    QAction *OneMeasurementAction;
    QAction *MultiMeasurementAction;

    QAction *ConfigTabelDevAction;
    QAction *ConfigGyroDevAction;

    QAction *StartTimerAction;
    QAction *StopTimerAction;

    QMenu *fileMenu;
    QToolBar *toolbar;

    QMenu *configMenu;

    QLabel *currValueLabel;
    QLabel *meanValueLabel;
    QLabel *minValueLabel;
    QLabel *maxValueLabel;
    QLabel *skoLabel;

    QLabel *timeAccumulateLabel;
    QLabel *azimuthMeasureLabel;

    QLineEdit *currValueLineEdit;
    QLineEdit *meanVelueLineEdit;
    QLineEdit *minValueLineEdit;
    QLineEdit *maxValueLineEdit;
    QLineEdit *skoLineEdit;

    QLabel *countMeasureLabel;
    QLineEdit *countMeasureLineEdit;

    QLineEdit *timeAccumulateLineEdit;
    QLineEdit *azimuthMeasureLineEdit;

    QWidget *measureWidget;

    TableDevice *ConfigTableDevice;
    GyroDevice *ConfigGyroDevice;


    bool isOneMeasure; ///< признак однократного измерения
    int prevMeasure;
    int numMeasure;
    int numPosition;
    float Azimuth;
    float SummAzimuth;
    float MeanAzimuth;
    float MaxAzimuth;
    float MinAzimuth;
    float numerator;
    float denumerator;
    float SKO;
    float pos1;
    float pos2;
    float pos3;
    float pos4;

    QTimer *ptmr;
};

#endif // WIDGET_H
