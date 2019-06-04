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



struct structCommandWordBIP
{
    uint16_t
        reserve:1,
        setSearchZeroIndicator:1,
        reserve2:1,
        turnOnSynchroEncoder:1,
        setCoefficientEncoder:4;

};
typedef union{
    structCommandWordBIP bits;
    uint16_t words;
} unionCommandWordBIP;

///длина пакета одного данных
const int LenghtOutputPacket=10;
struct outputData
{
    uint32_t cnt;
    unionCommandWordBIP commandWord;
    uint32_t reserve;
};

///структура статусного слова БИП56
struct structStatusBIP{
    uint16_t
        ///параметры БИП достоверны
        validValue:1,
        ///режим определения нуль-индикатора
        searchZeroIndicator:1,
        ///измеренное положение нуль индикатора достоверно
        validValueZeroIndicator:1,
        /// признак включения синхронизации по сигналам энкодера
        synchroEncoder:1,
        /// выбор коэффициента деления для энкодера 1 или 4
        coefficientEncoder:4;
};
typedef union {
    structStatusBIP bits;
    uint16_t words;
} unionStatusBIP;

///длина пакета одного данных  от гироскопического устройства
const int LenghtFastPacket=32;


///структура пакета данных от гироскопического устройства(протокол dadvtt)
struct inputData{
//    uint8_t Header;
    uint32_t cnt;
    uint32_t timeStepDPB;
    unionStatusBIP statusWord;
    float da;
    float dvX;
    float dvY;
    float impulseOfEncoder;
    uint16_t numTurnOPU;
    uint32_t reserve;
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

    void SearchZeroIndicator();
    void GetCoordinate(double *Lat,double *Lon, double *H);

    void resetStatus();
private:
    SlipProtocol *Slip;
    void FillOutList(inputData packet);
    void FillFirstList(inputData packet);
    bool SortData(const QByteArray &data);
    void MeasureRollAndPitch();
    void resetBuffer();

    int tick;
    int count;
    double summDa;
    double summDvX;
    double summDvY;
    double meanDvX;
    double meanDvY;
    bool isAccumulateData;
    int timeAccumulate;
    double Roll;
    double Pitch;
    double Lat;
    double Lon;
    double H;
    double G;
    double summDv1;
    double summDv2;

    /// признак достовернности данных БИП. true - данные достоверны, false - данные не достоверны
    bool validData;
    /// признак режима поиска нуль-индикатора. true - БИП в режиме поиска, false - БИП в покое
    bool modeSearchZero;
    /// признак достоверности измеренного положения нуль-индикатора. true - данные достоверны, false - данные не достоверны
    bool validZero;

    ///Положение срабатывания нуль индикатора(импульсы энкодера)
    float impulseOfEncoder;

    uint32_t outCntPacket;

public:
    friend QDataStream &operator>>(QDataStream &in,inputData &packet );
    uint32_t countPacket;
    int32_t errorPacket;
    inputData packet;
    outputData outpacket;
    QList<QString> *lstVal;
    QList<QString> *lstName;
    QTimer *tmr;
    QTimer *tmr2;
    QByteArray inputbuffer;
    QByteArray buffer2;
    QByteArray decodebuffer;
    int getTick() const;
    double getSummDa() const;
    double getMeanDvX() const;
    double getMeanDvY() const;
    bool getValidZero() const;
    bool getModeSearchZero() const;
    bool getValidData() const;
    float getImpulseOfEncoder() const;
};

#endif // GYRODATA_H
