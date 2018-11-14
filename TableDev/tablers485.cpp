
#include "tablers485.h"

QDataStream &operator>>(QDataStream &in, _AnswerPacketFormat &packet)
{
    in>>packet.ResCod;
    in>>packet.Stop;
}


tableRS485::tableRS485(QObject *parent) : QObject(parent)
{
    ComPort=new comPort();

}

void tableRS485::OnMotion()
{

    QByteArray request;
    QByteArray answer;
    ControlPacketForMat CMD;
    _AnswerPacketFormat Answ;
    unsigned char addr=1;
    unsigned char point=1;


    CMD.words.Start=START;
    CMD.words.APoint.Addr=addr;
    CMD.words.APoint.point=point;
    CMD.words.CMD=CONTROL;
    CMD.words.Data=ON_FOR_ANGLE;
    CMD.words.CRC=CMD.words.APoint.Addr+CMD.words.CMD+CMD.words.Data;

    for (int i=0; i<SIZE_CONTROL;++i)
        request.append(CMD.byte[i]);
    answer=ComPort->writeAndRead(request);
    QDataStream stream(answer);
    stream>>Answ;


    //    emit OutputToComPort(data);
}



