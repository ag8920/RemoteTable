#ifndef TABLERS485_H
#define TABLERS485_H

#include <QObject>
#include <../comport/comport.h>

const uint8_t START=0x5a;

//command
const uint8_t DRIVE_ANGLE=2;
const uint8_t DRIVE_SPEED=4;
const uint8_t SET_MAX_SPEED=7;
const uint8_t STATE_ANGLE_SPEED=8;
const uint8_t CONTROL=9;
const uint8_t DATE_VERSION=10;
const uint8_t READ_PARAM=254;
const uint8_t WRITE_PARAM=255;
const uint8_t SET_START_ANGLE=0;

//control param
const uint8_t ON_FOR_ANGLE=0xFF;
const uint8_t ON_FOR_SPEED=0xCC;
const uint8_t OFF_FULL=0x00;
const uint8_t ON_CLUTCH=0xF0;
const uint8_t OFF_CLUTH=0x0F;
const uint8_t SHUT_OFF=0x55;

//reports
const uint8_t RES_OUT=0x31;
const uint8_t RES_HRD=0x35;
const uint8_t RES_OK=0x36;
const uint8_t RES_CS=0xC8;
const uint8_t RES_COM=0xCB;
const uint8_t RES_PAR=0xCC;

//size command
const uint8_t SIZE_CONTROL=6;


typedef struct {unsigned char point:4,Addr:4;} _APoint;

//CONTROL COMMAND
typedef struct {
    unsigned char Start;
    _APoint APoint;
    unsigned char CMD;
    unsigned char Data;
    uint16_t CRC;

}_ControlPacketForMat;
typedef union {
    _ControlPacketForMat words;
    unsigned char byte[SIZE_CONTROL];
}ControlPacketForMat;
//ANSWER
 typedef struct {unsigned char ResCod; unsigned char Stop;} _AnswerPacketFormat;




class tableRS485 : public QObject
{
    Q_OBJECT
public:
    explicit tableRS485(QObject *parent = nullptr);
    comPort *ComPort;
    QThread *comPortThread;
signals:
    void OutputToComPort(const QByteArray &data);
public slots:
    void Request();
    void Answer();
    void OnMotion();

    friend QDataStream &operator>>(QDataStream &in, _AnswerPacketFormat &packet );
};

#endif // TABLERS485_H
