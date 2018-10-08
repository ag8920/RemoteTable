#ifndef GYRODATA_H
#define GYRODATA_H

#include <QObject>
#include <QVariant>
#include <QTimer>
#include "../comport/slipprotocol.h"


const int LenghtFastPacket=32;
struct FastPacket{
//    uint8_t Header;
    uint32_t cnt;
    float da1;
    float da2;
    float da3;
    float dv1;
    float dv2;
    float dv3;
    float Tmsk;
//    uint8_t CRC;
} ;


class GyroData : public QObject
{
    Q_OBJECT
public:
    explicit GyroData(QObject *parent = nullptr);
    ~GyroData();
    void process();
signals:
    void outCountPacket(const QString,const QString);
    void SendDataToTable(QList<QString> *varVal,QList<QString>*varName );
    void SendMeasureData(const QString Azimuth,const QString mean,
                         const QString min,const QString max,const QString sko);
    void SendDecodeData(QByteArray data);
    void finished();
public slots:
    void GetData(QByteArray inputArray);
    void SortData(QByteArray data);

    void AccumulateData();
    void NoAccumulateData();
    void Stop();

    void Unpack();
    void Unpack2(QByteArray inpArray);
    void ReadByte(char byte);
    void OutData();
private:
    SlipProtocol *Slip;
    void FillOutList(FastPacket packet);
    void FillFirstList(FastPacket packet);
public:
    friend QDataStream &operator>>(QDataStream &in,FastPacket &packet );
    uint32_t countPacket;
    int32_t errorPacket;
    FastPacket packet;
    QList<QString> *lstVal;
    QList<QString> *lstName;

    float summ;
    int cntsumm;
    float diff;
    bool isAccumulateData;

    QTimer *tmr;
    QTimer *tmr2;

    QByteArray inputbuffer;
    QByteArray buffer2;
    QByteArray decodebuffer;
};

#endif // GYRODATA_H
