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
    void AccumulateDataSignal();
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void StartMeasureSlot();
    void StartTimer();
    void StopTimer();
    bool SetTime();
    void StopMeasureSlot();
    void GetMeasureData(const QString Azimuth, const QString mean,
                        const QString min, const QString max, const QString sko);

public:
    QTimer *tmr;
    int timeSec;
private:
    void CreateActions();
    void initActionConnections();
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();
    void CreateWidgets();
    void CreateConnections();


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



};

#endif // WIDGET_H
