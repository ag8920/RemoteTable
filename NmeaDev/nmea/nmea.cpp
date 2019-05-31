#include "nmea.h"
#include "tok.h"
#include <math.h>
#include <QIODevice>
#include <QDataStream>
nmea::nmea(QObject *parent)
{
    Q_UNUSED(parent);
    RmcMsg={};
    GsvMsg={};
    GgaMsg={};
    GllMsg={};
    GsaMsg={};
    VtgMsg={};
}

nmea::~nmea()
{
    emit finished();
}

void nmea::process()
{
    tmr=new QTimer;
    tmr->setTimerType(Qt::TimerType::PreciseTimer);
    tmr->setInterval(1000);
    tmr->start();

    connect(tmr, &QTimer::timeout,this, &nmea::unpackData);
}

uint8_t nmea::calcCrc(QByteArray msg)
{
    uint8_t crc=0;
    for(int i=0;i<msg.size();i++)
    {
        if(msg[i]=='$') continue;
        crc=crc^msg[i];
    }
    return crc;
}

void nmea::GetData(const QByteArray &inputData)
{
    this->inputBuffer.append(inputData);
}

void nmea::unpackData()
{
    for(int i=0;i<inputBuffer.size();i++)
    {
        this->readByte(this->inputBuffer.at(i));
    }
    inputBuffer.clear();
}

void nmea::readByte(const char &byte)
{
    static char prevbyte;
    if(byte!=0x0D && byte!=0x0A)
        unpackBuffer.push_back(byte);
    if(prevbyte==0x0D && byte ==0x0A)
    {
        this->SortData(unpackBuffer);
        unpackBuffer.clear();
    }
    prevbyte=byte;

}

void nmea::SortData(const QByteArray &inputData)
{
    if(!inputData.isEmpty()){
        if(inputData.startsWith("$GNRMC"))
        {
           if(parse_GPRMC(inputData))
               OutRmcParametres();

        }
        else if(inputData.startsWith("$GNVTG"))
        {
            parse_GPVTG(inputData);
        }
        else if(inputData.startsWith("$GNGGA"))
        {
            parse_GPGGA(inputData);
        }
        else if(inputData.startsWith("$GNGSA"))
        {
            parse_GPGSA(inputData);
        }
        else if(inputData.startsWith("$GNGSV"))
        {
            parse_GPGSV(inputData);
        }
        else if(inputData.startsWith("$GNGLL"))
        {
            parse_GPGLL( inputData);
        }
    }
}

bool nmea::parse_GPGSV(QByteArray msg)
{
    //todo доделать по числу спутников
    QList<QByteArray>line=msg.split('*');
    QList<QByteArray>data=line.value(0).split(',');
    if(line.at(1).toInt(nullptr,16)==calcCrc(line.value(0)))
    {
        GsvMsg.ID=QString(data.value(0));
        GsvMsg.NumberOfMsg=data.value(1).toInt();
        GsvMsg.MsgNum=data.value(2).toInt();
        GsvMsg.SatellitesView=data.value(3).toInt();
        GsvMsg.SatelliteID=data.value(4).toInt();
        GsvMsg.Elevation=data.value(5).toDouble();
        GsvMsg.Azimuth=data.value(6).toDouble();
        GsvMsg.SNR=data.value(7).toInt();

        return  true;
    }
    return false;
}
//время, местоположение и годность навигационного решения
bool nmea::parse_GPGGA(QByteArray msg)
{
    QList<QByteArray>line=msg.split('*');
    QList<QByteArray>data=line.value(0).split(',');
    if(line.at(1).toInt(nullptr,16)==calcCrc(line.value(0)))
    {
        GgaMsg.ID=QString(data.value(0));
        _nmea_parse_time(data.value(1),data.value(1).size(),&GgaMsg.UTC_time);
        GgaMsg.Lat=Ndeg2degree(data.value(2).toDouble());
        GgaMsg.NS_ind=QString(data.value(3));
        if(GgaMsg.NS_ind=="S")GgaMsg.Lat=-GgaMsg.Lat;
        GgaMsg.Lon=Ndeg2degree(data.value(4).toDouble());
        GgaMsg.EW_ind=QString(data.value(5));
        if(GgaMsg.EW_ind=="W") GgaMsg.Lon=-GgaMsg.Lon;
        switch(data.value(6).toInt()){
        case 0:
            GgaMsg.PFI=tr("определение места не получено");
            break;
        case 1:
        case 2:
        case 3:
            GgaMsg.PFI=tr("позиция определена");
            break;
        default:
            GgaMsg.PFI=tr("определение места не получено");
            break;
        }
        GgaMsg.SatellitUsed=data.value(7).toInt();
        GgaMsg.HDOP=data.value(8).toDouble();
        GgaMsg.H=data.value(9).toDouble();
        GgaMsg.Units1=QString(data.value(10));
        GgaMsg.GS=data.value(11).toDouble();
        GgaMsg.Units2=QString(data.value(12));
        GgaMsg.AgeOfDiff=data.value(13).toDouble();
        GgaMsg.IDstation=data.value(14).toInt();
        GgaMsg.crc=static_cast<uint8_t>(line.at(1).toInt(nullptr,16));
        return true;
    }
    return false;
}

bool nmea::parse_GPGLL( QByteArray msg)
{
    QList<QByteArray>line=msg.split('*');
    QList<QByteArray>data=line.value(0).split(',');
    if(line.at(1).toInt(nullptr,16)==calcCrc(line.value(0)))
    {
        GllMsg.ID=QString(data.value(0));
        GllMsg.Lat=Ndeg2degree(data.value(1).toDouble());
        GllMsg.SN_ind=QString(data.value(2));
        if(GllMsg.SN_ind=="S")GllMsg.Lat=-GllMsg.Lat;
        GllMsg.Lon=Ndeg2degree(data.value(3).toDouble());
        GllMsg.EW_ind=QString(data.value(4));
        if(GllMsg.EW_ind=="W")GllMsg.Lon=-GllMsg.Lon;
        _nmea_parse_time(data.value(5),data.value(5).size(),&GllMsg.UTC);
        GllMsg.Status=QString(data.value(6));
        GllMsg.ModeOrient=QString(data.value(7));
        GllMsg.crc=static_cast<uint8_t>(line.at(1).toInt(nullptr,16));
        return true;
    }
    return false;
}

bool nmea::parse_GPGSA(QByteArray msg)
{
    QList<QByteArray>line=msg.split('*');
    QList<QByteArray>data=line.value(0).split(',');

    if(line.at(1).toInt(nullptr,16)==calcCrc(line.value(0)))
    {
        int i=0;
        GsaMsg.ID=QString(data.value(i++));
        GsaMsg.Mode1=QString(data.value(i++));
        GsaMsg.Mode2=QString(data.value(i++));
        GsaMsg.satellite1=data.value(i++).toInt();
        GsaMsg.satellite2=data.value(i++).toInt();
        GsaMsg.satellite3=data.value(i++).toInt();
        GsaMsg.satellite4=data.value(i++).toInt();
        GsaMsg.satellite5=data.value(i++).toInt();
        GsaMsg.satellite6=data.value(i++).toInt();
        GsaMsg.satellite7=data.value(i++).toInt();
        GsaMsg.satellite8=data.value(i++).toInt();
        GsaMsg.satellite9=data.value(i++).toInt();
        GsaMsg.satellite10=data.value(i++).toInt();
        GsaMsg.satellite11=data.value(i++).toInt();
        GsaMsg.satellite12=data.value(i++).toInt();
        GsaMsg.PDOP=data.value(i++).toDouble();
        GsaMsg.HDOP=data.value(i++).toDouble();
        GsaMsg.VDOP=data.value(i).toDouble();
        GsaMsg.crc=static_cast<uint8_t>(line.at(1).toInt(nullptr,16));
        return true;
    }
    return false;
}

bool nmea::parse_GPRMC(QByteArray msg)
{
    QList<QByteArray>line=msg.split('*');
    QList<QByteArray>data=line.value(0).split(',');


    if(line.at(1).toInt(nullptr,16)==calcCrc(line.value(0)))
    {
        int i=0;
        RmcMsg.ID=QString(data.value(i++));
        _nmea_parse_time(data.value(i),data.value(i).size(),&RmcMsg.UTC);
        i++;
        RmcMsg.Status=QString(data.value(i++));
        RmcMsg.Lat=Ndeg2degree(data.value(i++).toDouble());
        RmcMsg.NS_id=QString(data.value(i++));
        if(RmcMsg.NS_id=="S") RmcMsg.Lat=-RmcMsg.Lat;
        RmcMsg.Lon=Ndeg2degree(data.value(i++).toDouble());
        RmcMsg.EW_id=QString(data.value(i++));
        if(RmcMsg.EW_id=="W") RmcMsg.Lon=-RmcMsg.Lon;
        RmcMsg.Speed=data.value(i++).toDouble();
        RmcMsg.Course=data.value(i++).toDouble();
        _nmea_parse_time(data.value(i),data.value(i).size(),&RmcMsg.UTC);
        i++;
        RmcMsg.MagneticVariation=data.value(i++).toDouble();
        RmcMsg.EW_id2=QString(data.value(i));
        if(RmcMsg.EW_id2=="W")RmcMsg.MagneticVariation=-RmcMsg.MagneticVariation;
        RmcMsg.crc=static_cast<uint8_t>(line.at(1).toInt(nullptr,16));
        return true;
    }
    return false;
}

bool nmea::parse_GPVTG(QByteArray msg)
{
    QList<QByteArray>line=msg.split('*');
    QList<QByteArray>data=line.value(0).split(',');
    if(line.at(1).toInt(nullptr,16)==calcCrc(line.value(0)))
    {
        int i=0;
        VtgMsg.ID=QString(data.value(i++));
        VtgMsg.THdg=data.value(i++).toDouble();
        VtgMsg.Reference1=QString(data.value(i++));
        VtgMsg.MagnThdg=data.value(i++).toDouble();
        VtgMsg.Reference2=QString(data.value(i++));
        VtgMsg.Speed=data.value(i++).toDouble();
        VtgMsg.Units=QString(data.value(i++));
        VtgMsg.Speed_KMH=data.value(i++).toDouble();
        VtgMsg.Units_KMH=QString(data.value(i++));
        VtgMsg.crc=static_cast<uint8_t>(line.at(1).toInt(nullptr,16));
        return true;
    }
    return false;
}

double nmea::Ndeg2degree(double val)
{
    double deg=(static_cast<int>(val/100));
    val=(deg+(val-deg*100)/60);
    return val ;
}

double nmea::degree2Ndeg(double val)
{
    double intPart;
    double fraPart;
    fraPart=modf(val,&intPart);
    return intPart*100+fraPart*60;
}

int nmea::_nmea_parse_time(const char *buff, int buff_sz, NMEA0183::nmeaTIME *res)
{
    int success = 0;
    switch(buff_sz)
    {
    case sizeof("ddmmyy") - 1:
        success = (3 == nmea_scanf(buff, buff_sz,
                                   "%2d%2d%2d", &(res->day), &(res->mon), &(res->year)
                                   ));
        break;
        //    case sizeof("hhmmss") - 1:
        //        success = (3 == nmea_scanf(buff, buff_sz,
        //                                   "%2d%2d%2d", &(res->hour), &(res->min), &(res->sec)
        //                                   ));
        //        break;
    case sizeof("hhmmss.s") - 1:
    case sizeof("hhmmss.ss") - 1:
    case sizeof("hhmmss.sss") - 1:
        success = (4 == nmea_scanf(buff, buff_sz,
                                   "%2d%2d%2d.%d", &(res->hour), &(res->min), &(res->sec), &(res->hsec)
                                   ));
        break;
    default:
        //nmea_error("Parse of time error (format error)!");
        success = 0;
        break;
    }
    return (success?0:-1);
}

void nmea::OutRmcParametres()
{
    QByteArray outData;
    QDataStream out(&outData,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    out<<RmcMsg.Status
    <<RmcMsg.UTC.year
    <<RmcMsg.UTC.mon
    <<RmcMsg.UTC.day
    <<RmcMsg.UTC.hour
    <<RmcMsg.UTC.min
    <<RmcMsg.UTC.sec
    <<RmcMsg.UTC.hsec
    <<RmcMsg.Lat
    <<RmcMsg.Lon
    <<RmcMsg.Speed
    <<RmcMsg.Course
    <<RmcMsg.MagneticVariation
    <<GgaMsg.H
    <<GgaMsg.SatellitUsed
    <<GgaMsg.HDOP;
    emit outRmcNavData(outData);
}

void nmea::outpPIRTR(int ELIPS)
{
    QByteArray data;
    QString str;
    str="$PIRTR,"+QString::number(ELIPS)+",,";
    data=str.toLocal8Bit();
    str=str+"*"+QString::number(calcCrc(data),16)+"\r\n";
    data=str.toLocal8Bit();
    emit OutputData(data);
}
