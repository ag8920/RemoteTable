#ifndef DISP_H
#define DISP_H

#include <QObject>
#include <QTimer>

#include "TableDev/tabledevice.h"
#include "GyroDev/gyrodevice.h"
#include "Timer/ptimer.h"
#include "loger/loger.h"
#include "coordinatedialog/corrddialog.h"

class Disp : public QObject
{
    Q_OBJECT
public:
    explicit Disp(QObject *parent = nullptr);

signals:
void signalMeasureStart();
void signalMeasureStop();
void signalStartAccumulateData();
void signalStopAccumulateData();
void signalGotoPosition(QVariant position);
void signalResetAbsCoord();
void signalPutLog(QString data);
void signalOutAzParam(QString Az,QString meanAz,QString minAz,QString maxAz,
                      QString skoAz,QString numMeas);
public slots:
void slotMeasureStart();
void slotMeasureStop();
void slotStartTimer();
void slotSetTime(int time);//todo: можно объявить как функцию
void slotDispMeasure();
void slotSignleMeasure();
void slotInpDiff(float diff);
private:
//func
void initVariable();
//variable
int timeSec;
int numPrevMeasure;
int numCurMeasure;
int numPosition;
double Azimuth;
double SummAzimuth;
double MeanAzimuth;
double MinAzimuth;
double MaxAzimuth;
double SKO;
double numerator;
double denumerator;
double pos_0;
double pos_90;
double pos_180;
double pos_270;
double curdif;
//logical variable
bool isSetTime;
bool isSingleMeasure;
//object
QTimer *tmr;
loger *log;


};

#endif // DISP_H
