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
#include "loger/loger.h"
#include "coordinatedialog/corrddialog.h"
#include "disp.h"

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    ///сигнал закрытия окна приложения
    void onWindowClosed();
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    ///запускает измерения
    void StartMeasureSlot();
    ///останавливает измерения
    void StopMeasureSlot();
private slots:
    void viewAzParam(QString Az,QString meanAz,QString minAz,QString maxAz,
                     QString skoAz,QString numMeas);
    void viewAngle(QString Roll,QString Pitch);
    void setTime();
public:

private:

    //**********************
    //func
    //**********************
    ///создает действия
    void CreateActions();
    ///устанавливает соединения СИГНАЛ/СЛОТ
    void initActionConnections();
    ///создание меню
    void CreateMenus();
    ///создание панели инструментов
    void CreateToolBars();
    ///создание виджета окна
    void CreateWidgets();
    ///создание сигнально-слотовых соединений
    void CreateConnections();

    //***********************
    //object
    //***********************
    ///объект класса TableDevice
    TableDevice *ConfigTableDevice;
    ///объект класса GyroDevice
    GyroDevice *ConfigGyroDevice;
    ///
    corrdDialog *CoordDialog;
    Disp *disp;
    //***********************
    //widget elements
    //***********************
    QMenu *fileMenu;
    QToolBar *toolbar;
    QMenu *configMenu;
    ///вызов окна поворотного устройства
    QAction *ConfigTabelDevAction;
    ///вызов окна гироскопического устройства
    QAction *ConfigGyroDevAction;
    ///
    QAction *SetCoordianteAction;
    ///выполнить однократное измерение
    QAction *OneMeasurementAction;
    ///выполнить серию измерений
    QAction *StartTimerAction;
    ///остановить измерения
    QAction *StopTimerAction;

    ///надпись "Занчение азимута"
    QLabel *currValueLabel;
    ///надпись "Среднее значение азимута"
    QLabel *meanValueLabel;
    ///надпись "Минимальное значение"
    QLabel *minValueLabel;
    ///надпись "Максимальное значение"
    QLabel *maxValueLabel;
    ///надпись "СКО"
    QLabel *skoLabel;
    ///надпись "Время накопления"
    QLabel *timeAccumulateLabel;
    ///надпись "Количество измерений"
    QLabel *countMeasureLabel;
    ///
    QLabel *RollLabel;
    ///
    QLabel *PitchLabel;
    ///поле с значением времени накопления данных
    QLineEdit *timeAccumulateLineEdit;
    ///поле с измеренным значение азимута
    QLineEdit *currValueLineEdit;
    ///поле с средним значением азимута
    QLineEdit *meanVelueLineEdit;
    ///поле с минимальным значением азимута
    QLineEdit *minValueLineEdit;
    ///поле с максимальным значением азимута
    QLineEdit *maxValueLineEdit;
    ///поле со значением СКО
    QLineEdit *skoLineEdit;
    ///поле с значением кол-ва измерений
    QLineEdit *countMeasureLineEdit;
    ///
    QLineEdit *RollLineEdit;
    ///
    QLineEdit *PitchLineEdit;
    ///виджет главного окна
    QWidget *measureWidget;
};

#endif // WIDGET_H
