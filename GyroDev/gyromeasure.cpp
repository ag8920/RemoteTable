#include "gyromeasure.h"
#include <QDebug>
#include <QDataStream>

//-----------------------------------------------------------
// Назначение:
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
// Назначение:
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


}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void GyroMeasure::Measure()
{

}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void GyroMeasure::GetData(QByteArray inputArray)
{
    Slip->GetSlipByteArray(inputArray);

}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void GyroMeasure::SortData(QByteArray data)
{

    QDataStream stream(data);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);

    countPacket++;
    stream>>packet;

    FillOutList(packet);
    emit outCountPacket(QVariant(countPacket).toString());
    emit SendDataToTable(lstVal,lstName);

}
//-----------------------------------------------------------
// Назначение:
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
// Назначение:
//-----------------------------------------------------------
inline void GyroMeasure::ListAppend(QList<QString> *lst, QVariant val)
{
    lst->append(val.toString());
}


