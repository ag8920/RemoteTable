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
#include "qcustomplot/plotwidget.h"

class CustomLineEdit;
class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    ///сигнал закрытия окна приложения
    void onWindowClosed();
    ///сигнал начала измерений
    void StartMeasure();
    ///сигнал остановки измерений
    void StopMeasureSignal();
    ///сигнал запуска накопления данных
    void StartAccumulateDataSignal();
    ///сигнал остановки накопления данных
    void StopAccumulateDataSignal();
    ///сигнал перехода к след. позиции стола
    void GotoPosition(QVariant position);
    ///сигнал сброса абсолютных координат стола
    void ResetAbsCoord();
    ///логирование данных
    void PutLog(QString data);

    void buildgraph(int index,double data);
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    ///запускает измерения
    void StartMeasureSlot();
    ///запускает таймер и вызывает признак накопления данных
    void StartTimer();
    ///устанавливает время накопления данных
    bool SetTime();
    ///останавливает измерения
    void StopMeasureSlot();
    ///реализует четырех позиционный алгоритм
    void Measure();

   void  slotbuildgraph();
    void createPlot(QString name);
private slots:
    ///устанавливает признак однократного измерения
    void OneMeasureSlot();
    ///
    void viewAngle(QString Roll,QString Pitch);

public:
    int timeSec;
private:
    ///создает действия
    void CreateActions();
    ///устанавливает соединения СИГНАЛ/СЛОТ
    void initActionConnections();
    ///создание меню
    void CreateMenus();
    ///создание панели инструментов
    void CreateToolBars();
    void CreateStatusBar();
    ///создание виджета окна
    void CreateWidgets();
    ///создание сигнально-слотовых соединений
    void CreateConnections();
    ///инициализация переменных
    void InitVariable();

    ///вызов окна поворотного устройства
    QAction *ConfigTabelDevAction;
    ///вызов окна гироскопического устройства
    QAction *ConfigGyroDevAction;

    ///
    QAction *SetCoordianteAction;
    QAction *ViewPlotAction;
    QAction *stopPlot;
    ///выполнить однократное измерение
    QAction *OneMeasurementAction;
    ///выполнить серию измерений
    QAction *StartTimerAction;
    ///остановить измерения
    QAction *StopTimerAction;

    QMenu *fileMenu;
    QToolBar *toolbar;

    QMenu *configMenu;

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
    ///надпись "Количество измерений"
    QLabel *countMeasureLabel;
    ///поле с значением кол-ва измерений
    QLineEdit *countMeasureLineEdit;
    ///
    QLabel *RollLabel;
    ///
    /*QLineEdit*/CustomLineEdit *RollLineEdit;
    ///
    QLabel *PitchLabel;
    ///
    /*QLineEdit*/CustomLineEdit *PitchLineEdit;
    ///виджет главного окна
    QWidget *measureWidget;
    ///объект класса TableDevice
    TableDevice *ConfigTableDevice;
    ///объект класса GyroDevice
    GyroDevice *ConfigGyroDevice;
    ///
    corrdDialog *CoordDialog;
    ///объект класса loger
    loger *Log;
    ///объект класса QTimer
    QTimer *ptmr;
    QTimer *tmrsec;
    ///признак однократного измерения
    bool isOneMeasure;
    ///номер предыдущего измерения(вспом. переменная)
    int prevMeasure;
    ///номер измерения
    int numMeasure;
    ///номер позиции( необходим для 4-х позиционного алгоритма)
    int numPosition;
    ///значение азимута
    float Azimuth;
    ///сумма значений азимута(при каждом измерении значения суммируются)
    float SummAzimuth;
    ///среднее значение азимута
    float MeanAzimuth;
    ///максимальное значение азимута
    float MaxAzimuth;
    ///минимальное значение азимута
    float MinAzimuth;
    ///вспомогательная переменная
    float numerator;
    ///вспомогательная переменная
    float denumerator;
    ///среднеквадратическая погрешность
    float SKO;
    ///сумма значений da получаемых из гироскопа в положении 0 град.
    float pos_0;
    ///сумма значений da получаемых из гироскопа в положении 180 град.
    float pos_180;
    ///сумма значений da получаемых из гироскопа в положении 90 град.
    float pos_90;
    ///сумма значений da получаемых из гироскопа в положении 270 град.
    float pos_270;
};

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CustomLineEdit(QString name="default",QWidget *parent=nullptr): QLineEdit(parent)
    {
        this->name=name;
    }

    void mouseDoubleClickEvent(QMouseEvent *event){
        QLineEdit::mouseDoubleClickEvent(event);
        emit doubleclick(name);
    }
private:
    QString name;
    double data;
signals:
    void doubleclick(QString name);
};

#endif // WIDGET_H
