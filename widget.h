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
#include <QMdiArea>

#include "TableDev/tabledevice.h"
#include "GyroDev/gyrodevice.h"
#include "Timer/ptimer.h"
#include "loger/loger.h"
#include "coordinatedialog/corrddialog.h"
#include "qcustomplot/plotwidget.h"
#include "justdialog/justdialog.h"
#include "tdialog/teodalitdialog.h"

#include "TableDev/tablers485.h"
#include "NmeaDev/nmeadevice.h"

#include "qcgaugewidget/compaswidget.h"

typedef void (*Azcalc)(double g, double phi, double gt,
                       double tt, const double Ax[3],
                       const double Ay[3], const double N[3],
                       unsigned int light,
                       unsigned int rev,
                       double *b_gamma, double *theta, double *Az,
                       unsigned int *err);
typedef void (*TeoReCalc)(double A,double Pitch, double Roll,
                          double At, double Anm,double RollNm,
                          double PitchNm,double Pat,double PaKl,
                          double PaKR, double PvKL, double PvKR,
                          double *Av, double *Pitchv, double *Rollv);
extern QStringList alignmode;

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
    void StartAccumulateDataSignal(double time);
    ///сигнал остановки накопления данных
    void StopAccumulateDataSignal();
    ///сигнал перехода к след. позиции стола
    void GotoPosition(double position);

    ///сигнал сброса абсолютных координат стола
    void ResetAbsCoord();
    void ZeroPosition();

    void signalTableMesInit();
    ///логирование данных
    void PutLog(QString data);
    void sendCoordinate(double *Lat,double *Lon,double *H);
    void buildgraph(int index,double data);

    void setAccyracyTable(const int &value);
protected:
    void closeEvent(QCloseEvent *event) override;
//    void resizeEvent(QResizeEvent *event) override;
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU
public slots:

    void Dispatcher();
    ///запускает таймер и вызывает признак накопления данных
    void StartTimer();
    ///устанавливает время накопления данных
    bool SetTime();
    ///останавливает измерения
    void StopMeasureSlot();
    ///выполнение расчетов
    void Measure();
    void slotbuildgraph();
    void createPlot(QString name);
    void saveSettings();
    void readSettings();
    void recieveSnsBasicData(QByteArray data);
    void recieveCoordinate(double *Lat, double *Lon, double *H);
private slots:
    ///устанавливает признак однократного измерения
    void setCycleMeasureSlot(bool value);
    ///
    void viewAngle(QString Roll,QString Pitch);

    void selectModeAlign(int idx);
    void setProgress();

    void selectAlgorithm();

    void dumpCalcData();

    void setAccyracy();

    void setJustAmends(QByteArray data);
    void setJustTeodolit(QByteArray data);

    void slotShowFullScreen();

public:
    double timeSec;
    bool getRandomize() const;

    void setRandomize(bool value);

private:

    Azcalc azimcalc;
    TeoReCalc teorecalc;

    QTabWidget *tabwgt;
    QDockWidget *pdock;
    QLabel *LatLabel;
    QLabel *LonLabel;
    QLabel *HLabel;
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
    ///инициализация переменных
    void InitVariable();

    ///вызов окна поворотного устройства
    QAction *ConfigTabelDevAction;
    ///вызов окна гироскопического устройства
    QAction *ConfigGyroDevAction;
    ///вызов окна СНС устройства
    QAction *ConfigNmeadeviceAction;
    QAction *SetCoordianteAction;
    QAction *ViewPlotAction;
    QAction *stopPlot;
    ///выполнить серию измерений
    QAction *StartMeasureAction;
    ///остановить измерения
    QAction *StopMeasureAction;
    QAction *FourAlgAction;
    QAction *ThreeAlgAction;
    QActionGroup *AlgGroupAction;
    QAction *CycleMeasureAction;
    QAction *DumpCalcDataAction;

    QAction *SetAccyracyAction;
    QAction *InputJustAction;
    QAction *InputTeoJustAction;

    QAction *actionfullScreen;
    QAction *actionExit;

    QMenu *fileMenu;
    QMenu *configMenu;
    QMenu *helpMenu;
    QToolBar *toolbar;
    QToolBar *Coordtoolbar;
    QToolBar *typeAlignbar;
    QStatusBar *sb;

    QComboBox *typeAlignCBox;
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
    CustomLineEdit *RollLineEdit;
    ///
    CustomLineEdit *PitchLineEdit;
    QLineEdit *azimuthXALineEdit;
    QLineEdit *azimuthDiffLineEdit;



    QLineEdit *AzBipLineEdit;
    QLineEdit *MeanAzBipLineEdit;
    QLineEdit *MaxAzBipLineEdit;
    QLineEdit *MinAzBipLineEdit;
    QLineEdit *SKOAzBipLineEdit;
    QLineEdit *RollBipLineEdit;
    QLineEdit *PitchBipLineEdit;


    QProgressBar *progress;
    QTimeLine *timeLine;
    ///виджет главного окна
    QWidget *measureWidget;
    ///объект класса TableDevice
    TableDevice *ConfigTableDevice;
    ///объект класса GyroDevice
    GyroDevice *ConfigGyroDevice;
    ///
    corrdDialog *CoordDialog;
    NmeaDevice *ConfigNmeaDevice;
    ///объект класса loger
    loger *Log;
    ///объект класса QTimer
    QTimer *ptmr;
//    QTimer *tmrsec;

    QMdiArea *mdiArea;

    justDialog *JustDialog;
    teodalitDialog *TeoJustDialog;

    ///признак однократного измерения
    bool cycleMeasure;
    ///номер предыдущего измерения(вспом. переменная)
    int prevMeasure;
    ///номер измерения
    int numMeasure;
    ///номер позиции( необходим для 4-х позиционного алгоритма)
    int numPosition;
    ///значение азимута
    double Azimuth;
    ///сумма значений азимута(при каждом измерении значения суммируются)
    double SummAzimuth;
    ///среднее значение азимута
    double MeanAzimuth;
    ///максимальное значение азимута
    double MaxAzimuth;
    ///минимальное значение азимута
    double MinAzimuth;
    ///вспомогательная переменная
    double numerator;
    ///вспомогательная переменная
    double denumerator;
    ///среднеквадратическая погрешность
    double SKO;
    ///сумма значений da получаемых из гироскопа в положении 0 град.
    double da2_pos0;
    ///сумма значений da получаемых из гироскопа в положении 180 град.
    double da2_pos180;
    ///сумма значений da получаемых из гироскопа в положении 90 град.
    double da2_pos90;
    ///сумма значений da получаемых из гироскопа в положении 270 град.
    double da2_pos270;

    ///угол нуль метки от момента старта
    double zeroAzimuth;

    double dv1_pos0,dv1_pos90,dv1_pos180;
    double dv2_pos0,dv2_pos90,dv2_pos180;
    double H, Lat, Lon,G;
    double Roll, Pitch;
    double Latsns,Lonsns,Hsns,Speedsns;
    QString Statussns;
    bool fourposition;
    bool threeposition;

    ///матрица юстировочных поправок
    double ustEa[2][2];
    double PaKl,PaKR,PvKl,PvKR;
    ///азимут теодолита
    double At;
    ///азимут нуль-метки
    double Anm;
    ///тангаж нуль-метки в юстировочном положении
    double PitchNm;
    ///крен нуль-метки в юстировочном положении
    double RollNm;
    ///отсчет по горизонтальной оси теодолита в юстировочном положении
    double P_At;
    ///
    double PitchV;
    double RollV;
    double AzV;

    int step;
    int accyracyTable;



    QAction *randomAction;
    bool randomize;
    std::random_device rnd{};
    std::mt19937 gen{rnd()};
    std::normal_distribution <double> distAccyracy{0.,16.};
    std::normal_distribution <double> distInaccyracy{0.,48.};

    ///поправка по тангажу
    double gt;
    ///поправка по крену
    double tt;
    ///неколлинеарность
    unsigned int light;
    ///признак того что БИИ стоит вверх ногами
    unsigned int rev;
    ///проблемы возникшие по ходу расчета azimcalcfull
    QLabel *errLabel;
    unsigned int err;


    compasWidget *compas;
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
