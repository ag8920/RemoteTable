//------------------------------------------------------------------------------
//     Данный модуль служит для пересчета  принятых
//     данных от гироскопического устройства
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "gyromeasure.h"
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
GyroMeasure::GyroMeasure(QObject *parent) : QObject(parent)
{
    this->process();

}

GyroMeasure::~GyroMeasure()
{
    emit finished();
}

void GyroMeasure::process()
{
    Slip=new SlipProtocol;
    connect(Slip,&SlipProtocol::outDecodeArray,
            this,&GyroMeasure::SortData);
    lstVal=new QList<QString>;
    lstName=new QList<QString>{
            /*tr("Заголовок"),*/tr("Число пакетов"),
            tr("dat1[град/час]"),tr("dat2[град/час]"),tr("dat3[град/час]"),
            tr("dvt1[м/с2]"),tr("dvt2[м/с2]"),tr("dvt3[м/с2]")
            ,tr("Время[сек]")/*,tr("Контр.сумма")*/};

    this->isAccumulateData=false;
    this->summ=0;
    this->countPacket=0;
    this->cntsumm=0;

    tmr=new QTimer;
    tmr->setInterval(10);
    tmr->start();

    connect(tmr,&QTimer::timeout,this,&GyroMeasure::Unpack);
    connect(tmr,&QTimer::timeout,this,&GyroMeasure::OutData);
}

//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroMeasure::GetData(QByteArray inputArray)
{
    this->inputbuffer.append(inputArray);
}
//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroMeasure::Unpack()
{
    for(int i=0;i<inputbuffer.size();i++)
    {
        this->ReadByte(this->inputbuffer.at(i));
    }
    inputbuffer.clear();
}
//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroMeasure::ReadByte(char byte)
{
    static int cnt=0;
    buffer2.push_back(byte);cnt++;
    char r=Slip->SlipDecode(buffer2,decodebuffer,
                            LenghtFastPacket,cnt);
    if(r!=0)cnt=0;
    if(r==1){
        buffer2.clear();
        this->SortData(decodebuffer);
        decodebuffer.clear();

    }
}
//-----------------------------------------------------------
// Назначение: сортировка принятых данных(занесение в
//             данных структуры)
//-----------------------------------------------------------
void GyroMeasure::SortData(QByteArray data)
{

    QDataStream stream(data);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);

    countPacket++;
    stream>>packet;    

    if(this->isAccumulateData){
        this->summ+=packet.da2;
        this->cntsumm++;
    }
    
//    FillOutList(packet);
//    emit outCountPacket(QString::number(countPacket));
//    emit SendDataToTable(lstVal,lstName);
}
void GyroMeasure::OutData()
{
    FillOutList(packet);
    emit outCountPacket(QString::number(countPacket));
    emit SendDataToTable(lstVal,lstName);

}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroMeasure::AccumulateData()
{
    this->isAccumulateData=true;
    this->summ=0; //TODO
    this->cntsumm=0;
}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroMeasure::NoAccumulateData()
{
    this->isAccumulateData=false;
}

//-----------------------------------------------------------
// Назначение: заполнение данных в список
//             для табличного отображения в форме
//-----------------------------------------------------------
void GyroMeasure::FillOutList(FastPacket packet)
{
    lstVal->clear();
//    ListAppend(lstVal,packet.Header);
    ListAppend(lstVal,packet.cnt);
    ListAppend(lstVal,packet.da1);
    ListAppend(lstVal,packet.da2);
    ListAppend(lstVal,packet.da3);
    ListAppend(lstVal,packet.dv1);
    ListAppend(lstVal,packet.dv2);
    ListAppend(lstVal,packet.dv3);
    ListAppend(lstVal,packet.Tmsk);
//    ListAppend(lstVal,packet.CRC);
}
//-----------------------------------------------------------
// Назначение: занесение разнотипных данных в строковый
//            список
//-----------------------------------------------------------
inline void GyroMeasure::ListAppend(QList<QString> *lst, QVariant val)
{
    lst->append(val.toString());
}


