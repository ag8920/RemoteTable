//------------------------------------------------------------------------------
//     Модуль обработки принятых
//     данных от гироскопического устройства
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "gyrodata.h"
#include <QDebug>
#include <QDataStream>
#include <QtMath>

//-----------------------------------------------------------
// Назначение: пересчет контрольной суммы
//-----------------------------------------------------------
unsigned char crc8[256] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

unsigned char CSUM_CRC8(QByteArray b, int l)
{   unsigned char csum=0;
    for(int i=0;i<l-1;i++) csum=crc8[csum^b[i]];
    return csum;
}

//-----------------------------------------------------------
// Назначение: десереализация принятого пакета
//-----------------------------------------------------------
QDataStream &operator>>(QDataStream &in, FastPacket &packet)
{
    //in >> packet.Header;
    in >>packet.cnt    ;
    in >>packet.da1    ;
    in >>packet.da2    ;
    in >>packet.da3    ;
    in >>packet.dv1    ;
    in >>packet.dv2    ;
    in >>packet.dv3    ;
    in >>packet.Tmsk   ;
    //    in >>packet.CRC;
    return in;
}
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
GyroData::GyroData(QObject *parent) : QObject(parent)
{
    //this->process();

}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
GyroData::~GyroData()
{
    emit finished();
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void GyroData::process()
{
    Slip=new SlipProtocol;
    connect(Slip,&SlipProtocol::outDecodeArray,
            this,&GyroData::SortData);

    lstVal=new QList<QString>;
    this->FillFirstList(this->packet);

    lstName=new QList<QString>{
            tr("Номер пакета"),
            tr("dat1[град/час]"),
            tr("dat2[град/час]"),
            tr("dat3[град/час]"),
            tr("dvt1[м/с2]"),
            tr("dvt2[м/с2]"),
            tr("Готовность"),
            tr("Импульсы")};

    this->isAccumulateData=false;
    this->summ=0.;
    this->countPacket=0;
    this->tick=0;
    this->errorPacket=0;
    this->diff=0.;

    tmr=new QTimer();
    tmr->setTimerType(Qt::TimerType::PreciseTimer);
    tmr->setInterval(10);
    tmr->start();

    tmr2=new QTimer;
    tmr2->setInterval(1000);
    tmr2->start();

    connect(tmr,&QTimer::timeout,this,&GyroData::Unpack);
    connect(tmr2,&QTimer::timeout,this,&GyroData::OutData);


}

//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroData::GetData(const QByteArray &inputArray)
{
    this->inputbuffer.append(inputArray);
}
//-----------------------------------------------------------
// Назначение: распаковка данных
//-----------------------------------------------------------
void GyroData::Unpack()
{
    for(int i=0;i<inputbuffer.size();i++)
    {
        this->ReadByte(this->inputbuffer.at(i));
    }
    inputbuffer.clear();
}

void GyroData::Unpack2(QByteArray inpArray)
{
    for(int i=0;i<inpArray.size();i++)
    {
        this->ReadByte(inpArray.at(i));
    }
}
//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroData::ReadByte(const char &byte)
{
    static int cnt=0;
    buffer2.push_back(byte);
    cnt++;
    char r=Slip->SlipDecode(buffer2,decodebuffer,
                            LenghtFastPacket,cnt);
    if(r!=0)cnt=0;
    if(r==2) buffer2.clear();
    if(r==1){
        buffer2.clear();
        if(this->SortData(decodebuffer))
            emit SendDecodeData(decodebuffer);
        decodebuffer.clear();
    }
}
//-----------------------------------------------------------
// Назначение: сортировка принятых данных(занесение в
//             данных структуры)
//-----------------------------------------------------------
bool GyroData::SortData(const QByteArray &data)
{
    static uint32_t prevcnt;
    int32_t diffcnt=0;
    QDataStream stream(data);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);

    countPacket++;
    stream>>packet;


    //    if(packet.crc==CSUM_CRC8(data,33)){

    diffcnt=abs(static_cast<int32_t>(packet.cnt)-static_cast<int32_t>(prevcnt));
    if(diffcnt>1) this->errorPacket++;
    prevcnt=packet.cnt;

    summDv1+=static_cast<double>(packet.dv1);
    summDv2+=static_cast<double>(packet.dv2);
    count++;

    if(this->isAccumulateData){
        this->tick++;

        this->summ+=static_cast<double>(packet.da2);
        this->diff=summ/tick;
        this->summDvX+=static_cast<double>(packet.dv1);
        this->summDvY+=static_cast<double>(packet.dv2);
        this->diffDvX=summDvX/tick;
        this->diffDvY=summDvY/tick;
        if(this->tick>=this->timeAccumulate){
            this->isAccumulateData=false;
            this->tick=0;
            emit signalStopAcumulateData();
        }
    }
    return true;
    //}else return false;
}

void GyroData::MeasureRollAndPitch()
{
    //go*earth_a pz90.11*
    static double
            go=9.78049,
            earth_a=6378136.,
            betta=0.005317,
            alpha=0.000007;

    count!=0?summDv1=summDv1/count:summDv1=0;
    count!=0?summDv2=summDv2/count:summDv2=0;
    G=(go*earth_a*earth_a)/(earth_a+H)/(earth_a+H)*
            (1.+betta*sin(Lat)*sin(Lat)+alpha*sin(2*Lat)*sin(2*Lat));
    Pitch=qRadiansToDegrees(qAsin(summDv2/G));
    //if(std::isnan(Pitch))Pitch=90.;
    Roll=qRadiansToDegrees(qAsin(-summDv1/sqrt(pow(G,2)-pow(summDv2,2))));
    if(std::isnan(Roll))Roll=90.;
    summDv2=0.;summDv1=0.;
    count=0;
}

int GyroData::getTick() const
{
    return tick;
}

void GyroData::OutData()
{    
    FillOutList(packet);
    emit outCountPacket(QString::number(countPacket),
                        QString::number(errorPacket));
    emit SendDataToTable(lstVal,lstName);

    //MeasureRollAndPitch();
    //emit outAngle(QString::number(Roll),QString::number(Pitch));

}

void GyroData::GetCoordinate(double *Lat, double *Lon, double *H)
{
    this->Lat=qDegreesToRadians(*Lat);
    this->Lon=qDegreesToRadians(*Lon);
    this->H=*H;
}
//-----------------------------------------------------------
// Назначение: управление признаками накопления данных
//-----------------------------------------------------------
void GyroData::AccumulateData(double time)
{
    this->isAccumulateData=true;
    this->timeAccumulate=static_cast<int>(time*400);
    this->summ=0; //TODO
    this->tick=0;
    this->diff=0;
}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroData::NoAccumulateData()
{
    this->isAccumulateData=false;
}
//-----------------------------------------------------------
// Назначение: сброс данных(при остановке измерений)
//-----------------------------------------------------------
void GyroData::Stop()
{
    this->isAccumulateData=false;
    this->summ=0;
    this->tick=0;
    this->diff=0;
}

//-----------------------------------------------------------
// Назначение: заполнение данных в список
//             для табличного отображения в форме
//-----------------------------------------------------------
void GyroData::FillOutList(FastPacket packet)
{
    if(!lstVal->isEmpty()){
        lstVal->replace(0,QString::number(packet.cnt));
        lstVal->replace(1,QString::number(static_cast<double>(packet.da1)));
        lstVal->replace(2,QString::number(static_cast<double>(packet.da2)));
        lstVal->replace(3,QString::number(static_cast<double>(packet.da3)));
        lstVal->replace(4,QString::number(static_cast<double>(packet.dv1)));
        lstVal->replace(5,QString::number(static_cast<double>(packet.dv2)));
        lstVal->replace(6,QString::number(static_cast<double>(packet.dv3)));
        lstVal->replace(7,QString::number(static_cast<double>(packet.Tmsk)));
    }
}

void GyroData::FillFirstList(FastPacket packet)
{
    lstVal->clear();
    lstVal->append(QString::number(packet.cnt));
    lstVal->append(QString::number(static_cast<double>(packet.da1)));
    lstVal->append(QString::number(static_cast<double>(packet.da2)));
    lstVal->append(QString::number(static_cast<double>(packet.da3)));
    lstVal->append(QString::number(static_cast<double>(packet.dv1)));
    lstVal->append(QString::number(static_cast<double>(packet.dv2)));
    lstVal->append(QString::number(static_cast<double>(packet.dv3)));
    lstVal->append(QString::number(static_cast<double>(packet.Tmsk)));
}


