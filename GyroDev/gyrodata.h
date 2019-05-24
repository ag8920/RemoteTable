/*!
    @class GyroData
    @brief Класс обработки принятых
    данных от гироскопического устройства
    @author Щербаков Александр
    @date 13.09.2018
*/
#ifndef GYRODATA_H
#define GYRODATA_H

#include <QObject>
#include <QVariant>
#include <QTimer>
#include "../comport/slipprotocol.h"


///длина пакета одного данных  от гироскопического устройства
const int LenghtFastPacket=32;
///структура пакета данных от гироскопического устройства(протокол dadvtt)
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
    void outCountPacket(const QString&,const QString&);
    void outAngle(const QString,const QString);
    void SendDataToTable(QList<QString> *varVal,QList<QString>*varName );
    void SendDecodeData(QByteArray data);
    void finished();

    void signalStopAcumulateData();
    void PutLog(QString data, QString fileName);

    void signalSendData(const QByteArray &data);
public slots:
    void GetData(const QByteArray &inputArray);
    void SendData(const QByteArray &outputArray);

    void AccumulateData(double time);
    void NoAccumulateData();
    void Stop();

    void Unpack();
    void Unpack2(QByteArray inpArray);
    void ReadByte(const char &byte);
    void OutData();

    void GetCoordinate(double *Lat,double *Lon, double *H);
private:
    SlipProtocol *Slip;
    void FillOutList(FastPacket packet);
    void FillFirstList(FastPacket packet);
    bool SortData(const QByteArray &data);
    void MeasureRollAndPitch();
    void resetBuffer();
public:
    friend QDataStream &operator>>(QDataStream &in,FastPacket &packet );
    uint32_t countPacket;
    int32_t errorPacket;
    FastPacket packet;
    QList<QString> *lstVal;
    QList<QString> *lstName;

    double summ;
    int tick;
    double diff;
    double summDvX;
    double summDvY;
    double diffDvX;
    double diffDvY;

    bool isAccumulateData;

    QTimer *tmr;
    QTimer *tmr2;

    QByteArray inputbuffer;
    QByteArray buffer2;
    QByteArray decodebuffer;

    double Roll;
    double Pitch;
    double Lat;
    double Lon;
    double H;
    double G;
    double summDv1;
    double summDv2;
    int count;

    int timeAccumulate;
    int getTick() const;
};

#endif // GYRODATA_H
