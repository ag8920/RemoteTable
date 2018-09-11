#ifndef GYROMEASURE_H
#define GYROMEASURE_H

#include <QObject>

#include "../comport/slipprotocol.h"


const int LenghtFastPacket=34;
struct FastPacket{
    uint8_t Header;
    uint32_t cnt;
    float da1;
    float da2;
    float da3;
    float dv1;
    float dv2;
    float dv3;
    float Tmsk;
    uint8_t CRC;
} ;

typedef union {
    FastPacket words;
    unsigned char bytes[LenghtFastPacket];
} FastPacketUnion;


class GyroMeasure : public QObject
{
    Q_OBJECT
public:
    explicit GyroMeasure(QObject *parent = nullptr);

signals:

public slots:
    void Measure();
    void GetData(QByteArray inputArray);
    void SortData(QByteArray data);
private:
    SlipProtocol *Slip;
    FastPacketUnion FastBinsPacket;
public:
    friend QDataStream &operator>>(QDataStream &in,FastPacket &packet );
};

#endif // GYROMEASURE_H
