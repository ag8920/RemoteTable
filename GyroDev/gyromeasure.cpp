//------------------------------------------------------------------------------
//     Данный модуль служит для пересчета  принятых
//     данных от гироскопического устройства
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "gyromeasure.h"
#include <QDebug>
#include <QDataStream>
#include <QtMath>

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
GyroMeasure::GyroMeasure(QObject *parent) : QObject(parent)
{
    //this->process();

}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
GyroMeasure::~GyroMeasure()
{
    emit finished();
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void GyroMeasure::process()
{
    Slip=new SlipProtocol;
    connect(Slip,&SlipProtocol::outDecodeArray,
            this,&GyroMeasure::SortData);

    lstVal=new QList<QString>;
    this->FillFirstList(this->packet);

    lstName=new QList<QString>{
            tr("Номер пакета"),
            tr("dat1[град/час]"),
            tr("dat2[град/час]"),
            tr("dat3[град/час]"),
            tr("dvt1[м/с2]"),
            tr("dvt2[м/с2]"),
            tr("dvt3[м/с2]"),
            tr("Время[сек]")};

    this->isAccumulateData=false;
    this->summ=0.;
    this->countPacket=0;
    this->cntsumm=0;
    this->errorPacket=0;
    this->diff=0.;

    tmr=new QTimer;
    tmr->setInterval(10);
    tmr->start();

    tmr2=new QTimer;
    tmr2->setInterval(1000);
    tmr2->start();

    connect(tmr,&QTimer::timeout,this,&GyroMeasure::Unpack);
    connect(tmr2,&QTimer::timeout,this,&GyroMeasure::OutData);
}

//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroMeasure::GetData(QByteArray inputArray)
{
    this->inputbuffer.append(inputArray);

    ///> @todo проверить данный способ - показания стали чуть хуже, имеются пропуски пакетов
//    this->Unpack2(this->inputbuffer);
//    this->inputbuffer.clear();
}
//-----------------------------------------------------------
// Назначение: распаковка данных
//-----------------------------------------------------------
void GyroMeasure::Unpack()
{
    for(int i=0;i<inputbuffer.size();i++)
    {
        this->ReadByte(this->inputbuffer.at(i));
    }
    inputbuffer.clear();
}

void GyroMeasure::Unpack2(QByteArray inpArray)
{
    for(int i=0;i<inpArray.size();i++)
    {
        this->ReadByte(inpArray.at(i));
    }
}
//-----------------------------------------------------------
// Назначение: прием данных
//-----------------------------------------------------------
void GyroMeasure::ReadByte(char byte)
{
    static int cnt=0;
    buffer2.push_back(byte);cnt++;
    char r=Slip->SlipDecode(buffer2,decodebuffer,
                            LenghtFastPacket,cnt);
    if(r!=0)cnt=0;
    if(r==1){
        buffer2.clear();
        this->SortData(decodebuffer);
        decodebuffer.clear();
    }
}
//-----------------------------------------------------------
// Назначение: сортировка принятых данных(занесение в
//             данных структуры)
//-----------------------------------------------------------
void GyroMeasure::SortData(QByteArray data)
{
    static uint32_t prevcnt;
    int32_t diffcnt=0;
    QDataStream stream(data);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);

    countPacket++;
    stream>>packet;

    diffcnt=abs(static_cast<int32_t>(packet.cnt)-static_cast<int32_t>(prevcnt));
    if(diffcnt>1) this->errorPacket++;
    prevcnt=packet.cnt;

    if(this->isAccumulateData){
        this->summ+=packet.da2;
        this->cntsumm++;
        this->diff=summ/cntsumm;
    }
}
void GyroMeasure::OutData()
{
    FillOutList(packet);
    emit outCountPacket(QString::number(countPacket),
                        QString::number(errorPacket));
    emit SendDataToTable(lstVal,lstName);

}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroMeasure::AccumulateData()
{
    this->isAccumulateData=true;
    this->summ=0; //TODO
    this->cntsumm=0;
    this->diff=0;
}
//-----------------------------------------------------------
// Назначение: управление признаком накопления данных 
//-----------------------------------------------------------
void GyroMeasure::NoAccumulateData()
{
    this->isAccumulateData=false;
}

//-----------------------------------------------------------
// Назначение: заполнение данных в список
//             для табличного отображения в форме
//-----------------------------------------------------------
void GyroMeasure::FillOutList(FastPacket packet)
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

void GyroMeasure::FillFirstList(FastPacket packet)
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


