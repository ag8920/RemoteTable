//------------------------------------------------------------------------------
//     Данный модуль служит для пересчета  принятых
//     данных от гироскопического устройства
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "gyrodata.h"
#include <QDebug>
#include <QDataStream>
#include <QtMath>

//-----------------------------------------------------------
// Назначение: десереализация принятого пакета
//-----------------------------------------------------------
QDataStream &operator>>(QDataStream &in, FastPacket &packet)
{
    //in >> packet.Header;
    in >>packet.cnt    ;
    in >>packet.da1    ;
    in >>packet.da2    ;
    in >>packet.da3    ;
    in >>packet.dv1    ;
    in >>packet.dv2    ;
    in >>packet.dv3    ;
    in >>packet.Tmsk   ;
//    in >>packet.CRC;
    return in;
}
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
GyroData::GyroData(QObject *parent) : QObject(parent)
{
    //this->process();

}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
GyroData::~GyroData()
{
    emit finished();
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void GyroData::process()
{
    Slip=new SlipProtocol;
    connect(Slip,&SlipProtocol::outDecodeArray,
            this,&GyroData::SortData);

    lstVal=new QList<QString>;
    this->FillFirstList(this->packet);

    lstName=new QList<QString>{
            tr("Номер пакета"),
            tr("dat1[град/час]"),
            tr("dat2[град/час]"),
            tr("dat3[град/час]"),
            tr("dvt1[м/с2]"),
            tr("dvt2[м/с2]"),
            tr("dvt3[м/с2]"),
            tr("Время[сек]")};

    this->isAccumulateData=false;
    this->summ=0.;
    this->countPacket=0;
    this->cntsumm=0;
    this->errorPacket=0;
    this->diff=0.;

    tmr=new QTimer();
    tmr->setTimerType(Qt::TimerType::PreciseTimer);
    tmr->setInterval(10);
    tmr->start();

    tmr2=new QTimer;
    tmr2->setInterval(1000);
    tmr2->start();

    connect(tmr,&QTimer::timeout,this,&GyroData::Unpack);
    connect(tmr2,&QTimer::timeout,this,&GyroData::OutData);
}

//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroData::GetData(QByteArray inputArray)
{
    this->inputbuffer.append(inputArray);
}
//-----------------------------------------------------------
// Назначение: распаковка данных
//-----------------------------------------------------------
void GyroData::Unpack()
{
    for(int i=0;i<inputbuffer.size();i++)
    {
        this->ReadByte(this->inputbuffer.at(i));
    }
    inputbuffer.clear();
}

void GyroData::Unpack2(QByteArray inpArray)
{
    for(int i=0;i<inpArray.size();i++)
    {
        this->ReadByte(inpArray.at(i));
    }
}
//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroData::ReadByte(char byte)
{
    static int cnt=0;
    buffer2.push_back(byte);cnt++;
    char r=Slip->SlipDecode(buffer2,decodebuffer,
                            LenghtFastPacket,cnt);
    if(r!=0)cnt=0;
    if(r==1){
        buffer2.clear();
        this->SortData(decodebuffer);
        emit SendDecodeData(decodebuffer);
        decodebuffer.clear();
    }
}
//-----------------------------------------------------------
// Назначение: сортировка принятых данных(занесение в
//             данных структуры)
//-----------------------------------------------------------
void GyroData::SortData(QByteArray data)
{
    static uint32_t prevcnt;
    int32_t diffcnt=0;
    QDataStream stream(data);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);

    countPacket++;
    stream>>packet;

    diffcnt=abs(static_cast<int32_t>(packet.cnt)-static_cast<int32_t>(prevcnt));
    if(diffcnt>1) this->errorPacket++;
    prevcnt=packet.cnt;

    if(this->isAccumulateData){
        this->summ+=packet.da2;
        this->cntsumm++;
        this->diff=summ/cntsumm;
    }
}
void GyroData::OutData()
{
    FillOutList(packet);
    emit outCountPacket(QString::number(countPacket),
                        QString::number(errorPacket));
    emit SendDataToTable(lstVal,lstName);

}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroData::AccumulateData()
{
    this->isAccumulateData=true;
    this->summ=0; //TODO
    this->cntsumm=0;
    this->diff=0;
}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroData::NoAccumulateData()
{
    this->isAccumulateData=false;
}
//-----------------------------------------------------------
// Назначение: сброс данных(при остановке измерений)
//-----------------------------------------------------------
void GyroData::Stop()
{
    this->isAccumulateData=false;
    this->summ=0;
    this->cntsumm=0;
    this->diff=0;
}

//-----------------------------------------------------------
// Назначение: заполнение данных в список
//             для табличного отображения в форме
//-----------------------------------------------------------
void GyroData::FillOutList(FastPacket packet)
{
    if(!lstVal->isEmpty()){
        lstVal->replace(0,QString::number(packet.cnt));
        lstVal->replace(1,QString::number(static_cast<double>(packet.da1)));
        lstVal->replace(2,QString::number(static_cast<double>(packet.da2)));
        lstVal->replace(3,QString::number(static_cast<double>(packet.da3)));
        lstVal->replace(4,QString::number(static_cast<double>(packet.dv1)));
        lstVal->replace(5,QString::number(static_cast<double>(packet.dv2)));
        lstVal->replace(6,QString::number(static_cast<double>(packet.dv3)));
        lstVal->replace(7,QString::number(static_cast<double>(packet.Tmsk)));
    }
}

void GyroData::FillFirstList(FastPacket packet)
{
    lstVal->clear();
    lstVal->append(QString::number(packet.cnt));
    lstVal->append(QString::number(static_cast<double>(packet.da1)));
    lstVal->append(QString::number(static_cast<double>(packet.da2)));
    lstVal->append(QString::number(static_cast<double>(packet.da3)));
    lstVal->append(QString::number(static_cast<double>(packet.dv1)));
    lstVal->append(QString::number(static_cast<double>(packet.dv2)));
    lstVal->append(QString::number(static_cast<double>(packet.dv3)));
    lstVal->append(QString::number(static_cast<double>(packet.Tmsk)));
}


