#include "disp.h"

enum pos{DEG_90=0,DEG_180,DEG_270,DEG_0};

Disp::Disp(QObject *parent) : QObject(parent)
{
    tmr=new QTimer;
    tmr->setTimerType(Qt::TimerType::PreciseTimer);
    timeSec=600 * 1000;
    log=new loger;

    initVariable();
    connect(tmr,&QTimer::timeout,this,&Disp::slotDispMeasure);
    connect(this,&Disp::signalPutLog,log,&loger::PutLog);
}
//------------------------------------------------------------------------------
void Disp::initVariable()
{
//    this->timeSec=0;
    this->numPrevMeasure=0;
    this->numCurMeasure=0;
    this->numPosition=0;
    this->Azimuth=0.;
    this->SummAzimuth=0;
    this->MeanAzimuth=0;
    this->MinAzimuth=0;
    this->MaxAzimuth=0;
    this->SKO=0;
    this->numerator=0;
    this->denumerator=0;
    this->pos_0=0;
    this->pos_90=0;
    this->pos_180=0;
    this->pos_270=0;
//    this->isSetTime=false;
    this->isSingleMeasure=false;
}
//------------------------------------------------------------------------------
void Disp::slotMeasureStart()
{
//    if(isSetTime){
        this->slotStartTimer();
        emit signalMeasureStart();
        emit signalResetAbsCoord();
//    }else
//        signalMeasureStop();
}
//------------------------------------------------------------------------------
void Disp::slotMeasureStop()
{
    tmr->stop();
    this->initVariable();
    emit signalMeasureStop();

}
//------------------------------------------------------------------------------
void Disp::slotStartTimer()
{
    tmr->setInterval(timeSec);
    tmr->setSingleShot(true);
    tmr->start();
    emit signalStartAccumulateData();
}
//------------------------------------------------------------------------------
void Disp::slotSetTime(int time)
{
    this->timeSec=(time*1000)/4;
//    isSetTime=true;
}

//------------------------------------------------------------------------------
void Disp::slotSignleMeasure()
{
    isSingleMeasure=true;
}

void Disp::slotInpDiff(float diff)
{
    this->curdif=static_cast<double>(diff);
}

//------------------------------------------------------------------------------
void Disp::slotDispMeasure()
{
    emit signalStopAccumulateData();
    switch (numPosition) {
    case DEG_180:
        pos_0=curdif; //todo
        numPosition++;
        emit signalGotoPosition(-200000);
        break;
    case DEG_270:
        pos_180=curdif;//todo
        numPosition++;
        emit signalGotoPosition(-300000);
        break;
    case DEG_90:
        pos_90=curdif;//todo
        numPosition++;
        emit signalGotoPosition(-100000);
        break;
    case DEG_0:
        pos_270=curdif;//todo
        numPosition=0;
        numCurMeasure++;
        emit signalGotoPosition(0);
        break;
    default:break;
    }
    if(numCurMeasure>numPrevMeasure){
        numPrevMeasure=numCurMeasure;

        //пересчет параметров
         Azimuth=qRadiansToDegrees((atan2((pos_270-pos_90),
                                           (pos_0-pos_180))));
         Azimuth<0?Azimuth+=360.0:Azimuth;
         SummAzimuth+=Azimuth;
         if(numCurMeasure==1){
             MeanAzimuth=Azimuth;
             MaxAzimuth=Azimuth;
             MinAzimuth=Azimuth;
         }
         else{
             MeanAzimuth=SummAzimuth/numCurMeasure;
             MaxAzimuth<Azimuth?MaxAzimuth=Azimuth:MaxAzimuth;
             MinAzimuth>Azimuth?MinAzimuth=Azimuth:MinAzimuth;
         }

         numerator+=pow((Azimuth-MeanAzimuth),2);
         numCurMeasure>1?denumerator=numCurMeasure-1:denumerator=1;
         SKO=sqrt(numerator/denumerator);

         emit signalOutAzParam(QString::number(Azimuth),
                               QString::number(MeanAzimuth),
                               QString::number(MaxAzimuth),
                               QString::number(MinAzimuth),
                               QString::number(SKO),
                               QString::number(numCurMeasure));
         if(numCurMeasure==1)
             emit signalPutLog(tr("время\tазимут\t ср.знач\t мин.знач\t макс.знач\t СКО\n"));
             emit signalPutLog(tr("%1\t %2\t %3\t %4\t %5\n")
                            .arg(static_cast<double>(Azimuth))
                            .arg(static_cast<double>(MeanAzimuth))
                            .arg(static_cast<double>(MinAzimuth))
                            .arg(static_cast<double>(MaxAzimuth))
                            .arg(static_cast<double>(SKO))
                         );
         if(isSingleMeasure){
             isSingleMeasure=false;
             this->slotMeasureStop();
         }

    }
}
