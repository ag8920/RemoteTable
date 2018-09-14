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


class GyroMeasure : public QObject
{
    Q_OBJECT
public:
    explicit GyroMeasure(QObject *parent = nullptr);

signals:
    void outCountPacket(const QString);
    void SendDataToTable(QList<QString> *varVal,QList<QString>*varName );
public slots:
    void Measure();
    void GetData(QByteArray inputArray);
    void SortData(QByteArray data);
private:
    SlipProtocol *Slip;
    void FillOutList(FastPacket packet);
    inline void ListAppend(QList<QString> *lst, QVariant val);
public:
    friend QDataStream &operator>>(QDataStream &in,FastPacket &packet );
    uint32_t countPacket;
    FastPacket packet;
    QList<QString> *lstVal;
    QList<QString> *lstName;
};

#endif // GYROMEASURE_H