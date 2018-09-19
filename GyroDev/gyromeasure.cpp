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
    in >> packet.Header;
    in >>packet.cnt    ;
    in >>packet.da1    ;
    in >>packet.da2    ;
    in >>packet.da3    ;
    in >>packet.dv1    ;
    in >>packet.dv2    ;
    in >>packet.dv3    ;
    in >>packet.Tmsk   ;
    in >>packet.CRC;
    return in;
}
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
GyroMeasure::GyroMeasure(QObject *parent) : QObject(parent)
{
    countPacket=0;
    Slip=new SlipProtocol;
    connect(Slip,&SlipProtocol::outDecodeArray,
            this,&GyroMeasure::SortData);
    lstVal=new QList<QString>;
    lstName=new QList<QString>{
            tr("Заголовок"),tr("Число пакетов"),
            tr("dat1[град/час]"),tr("dat2[град/час]"),tr("dat3[град/час]"),
            tr("dvt1[м/с2]"),tr("dvt2[м/с2]"),tr("dvt3[м/с2]")
            ,tr("Время[сек]"),tr("Контр.сумма")};
    isAccumulateData=false;
    numMeasure=0;
    numPosition=0;

}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void GyroMeasure::Measure(int num)
{
    if(num && numMeasure<num )
    {
        numMeasure=num;
        Azimuth=qRadiansToDegrees(static_cast<float>(atan2((da[0]-da[1]),(da[2]-da[3]))));
    
        meanValue=Azimuth/static_cast<float>(numMeasure);
    
        maxValue>Azimuth?maxValue=Azimuth:maxValue;
    
        minValue<Azimuth?minValue=Azimuth:minValue;
    //TODO : пересчет СКО
        da[0]=0;da[1]=0;da[2]=0;da[3]=0;
    }
    
}
//-----------------------------------------------------------
// Назначение: накопление данных
//-----------------------------------------------------------
void GyroMeasure::Accumulate()
{
    switch (numPosition) {
    case 0:
        da[0]+=packet.da2;
        break;
    case 1:
        da[1]+=packet.da2;
        break;
    case 2:
        da[2]+=packet.da2;
        break;
    case 3:
        da[3]+=packet.da2;
        break;
    default: break;
    }
}
//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroMeasure::GetData(QByteArray inputArray)
{
    Slip->GetSlipByteArray(inputArray);

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
    

    FillOutList(packet);
    if(isAccumulateData)
        this->Accumulate();
    
    emit outCountPacket(QVariant(countPacket).toString());
    emit SendDataToTable(lstVal,lstName);
    
}
//-----------------------------------------------------------
// Назначение: получение текущей позиции 
//-----------------------------------------------------------
void GyroMeasure::GetPosition(int position)
{
    numPosition=position;
}
//-----------------------------------------------------------
// Назначение: количество завершенных измерений 
//-----------------------------------------------------------
void GyroMeasure::GetNumMeasure(int num)
{
    numMeasure=num;
}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroMeasure::AccumulateData()
{
    isAccumulateData=true;
}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroMeasure::NoAccumulateData()
{
    isAccumulateData=false;
}
//-----------------------------------------------------------
// Назначение: заполнение данных в список
//             для табличного отображения в форме
//-----------------------------------------------------------
void GyroMeasure::FillOutList(FastPacket packet)
{
    lstVal->clear();
    ListAppend(lstVal,packet.Header);
    ListAppend(lstVal,packet.cnt);
    ListAppend(lstVal,packet.da1);
    ListAppend(lstVal,packet.da2);
    ListAppend(lstVal,packet.da3);
    ListAppend(lstVal,packet.dv1);
    ListAppend(lstVal,packet.dv2);
    ListAppend(lstVal,packet.dv3);
    ListAppend(lstVal,packet.Tmsk);
    ListAppend(lstVal,packet.CRC);
}
//-----------------------------------------------------------
// Назначение: занесение разнотипных данных в строковый
//            список
//-----------------------------------------------------------
inline void GyroMeasure::ListAppend(QList<QString> *lst, QVariant val)
{
    lst->append(val.toString());
}


