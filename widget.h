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
    void onWindowClosed();
    void StartMeasure();
    void StopMeasureSignal();
    void StartAccumulateDataSignal();
    void StopAccumulateDataSignal();
    void GotoPosition(QVariant position);
    void ResetAbsCoord();
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void StartMeasureSlot();
    void StartTimer();
    bool SetTime();
    void StopMeasureSlot();
    void Measure();

public:
    int timeSec;
private:
    void CreateActions();
    void initActionConnections();
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();
    void CreateWidgets();
    void CreateConnections();
    void InitVariable();

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

    QLineEdit *timeAccumulateLineEdit;
    QLineEdit *azimuthMeasureLineEdit;

    QWidget *measureWidget;

    TableDevice *ConfigTableDevice;
    GyroDevice *ConfigGyroDevice;


    int prevMeasure;
    int numMeasure;
    int numPosition;
    float Azimuth;
    float pos1;
    float pos2;
    float pos3;
    float pos4;

    PTimer *ptmr;
};

#endif // WIDGET_H
