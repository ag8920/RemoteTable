#include "gyromeasure.h"
#include <QDebug>
#include <QDataStream>

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


GyroMeasure::GyroMeasure(QObject *parent) : QObject(parent)
{
    Slip=new SlipProtocol;
    connect(Slip,&SlipProtocol::outDecodeArray,
            this,&GyroMeasure::SortData);
}
//-----------------------------------------------------------
void GyroMeasure::Measure()
{

}
//-----------------------------------------------------------
void GyroMeasure::GetData(QByteArray inputArray)
{
    Slip->GetSlipByteArray(inputArray);

}
//-----------------------------------------------------------
void GyroMeasure::SortData(QByteArray data)
{

    QDataStream stream(data);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);
    FastPacket packet;
    stream>>packet;

}


