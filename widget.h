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
protected:
    void closeEvent(QCloseEvent *event);
public slots:

public:
    QTimer *tmr;

private:
    void CreateActions();
    void initActionConnections();
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();
    void CreateWidgets();
    void CreateConnections();

    void StartTimer();
    void StopTimer();
    QAction *DeltaPsProtocolAction;
    QAction *Rate2ProtocolAction;
    QAction *DadvttProtocolAction;

    QAction *OneMeasurementAction;
    QAction *MultiMeasurementAction;

    QAction *ConfigTabelDevAction;
    QAction *ConfigGyroDevAction;

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
