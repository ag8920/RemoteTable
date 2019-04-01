#ifndef NMEA_H
#define NMEA_H
#include <QObject>
#include <QVariant>
#include <QTimer>

namespace NMEA0183 {
struct _Date{
    int Day;
    int Month;
    int Year;
};
struct nmeaTIME
{
    int year;       /**< Years since 1900 */
    int mon;        /**< Months since January - [0,11] */
    int day;        /**< Day of the month - [1,31] */
    int hour;       /**< Hours since midnight - [0,23] */
    int min;        /**< Minutes after the hour - [0,59] */
    int sec;        /**< Seconds after the minute - [0,59] */
    int hsec;       /**< Hundredth part of second - [0,99] */

};
//рекоменданный минимум данных
struct RMC_msg{
    QString ID;
    nmeaTIME UTC;
    QString Status;
    double Lat;
    QString NS_id;
    double Lon;
    QString EW_id;
    double Speed;
    double Course;
    nmeaTIME date;
    double MagneticVariation;
    QString EW_id2;
    uint8_t crc;
};
//наземный курс и скорость
struct VTG_msg{
    QString ID;
    double THdg;
    QString Reference1;
    double MagnThdg;
    QString Reference2;
    double Speed;
    QString Units;
    double Speed_KMH;
    QString Units_KMH;
    uint8_t crc;
};
//время, местоположение и годность навигационного решения
struct GGA_msg
{
    QString ID;
    nmeaTIME UTC_time; //время обсервации
    double Lat; //широта
    QString NS_ind;// север/юг
    double Lon;//долгота
    QString EW_ind;// восток/запад
    QString PFI;// показатель качества обсервации 0-определение места не получено
                    // 1-обсервация получена в автономном режиме
                    // 2-обсервация в диф.режиме
    int SatellitUsed;//число НКА в решении
    double HDOP;//величина горизонтального геометрического фактора
    double H;//высота над средним уровнем моря
    QString Units1;
    double GS;//превышение геоида над элипсоидом  WGS84
    QString Units2;
    double AgeOfDiff;//возраст дифференциальной поправки
    int IDstation;
    uint8_t crc;//контрольная сумма

};
//спутники в решении
struct GSA_msg{
    QString ID;
    QString Mode1;
    QString Mode2;
    int satellite1;
    int satellite2;
    int satellite3;
    int satellite4;
    int satellite5;
    int satellite6;
    int satellite7;
    int satellite8;
    int satellite9;
    int satellite10;
    int satellite11;
    int satellite12;
    double PDOP;
    double HDOP;
    double VDOP;
    uint8_t crc;
};
//видимые спутники
struct GSV_msg{
    QString ID;
    int NumberOfMsg;
    int MsgNum;
    int SatellitesView;
    int SatelliteID;
    double Elevation;
    double Azimuth;
    int SNR;
    uint8_t crc;
};

//местоположение, время и годность навигационного решения
struct GLL_msg{
    QString ID;
    double Lat;
    QString SN_ind;
    double Lon;
    QString EW_ind;
    nmeaTIME UTC;
    QString Status;
    QString ModeOrient;//режим местоопределения
    uint8_t crc;
};

}

class nmea : public QObject
{
    Q_OBJECT
public:
    explicit nmea(QObject *parent=nullptr);
    ~nmea();
signals:
    void outRmcNavData(QByteArray data);
    void OutputData(QByteArray data);
    void finished();
public slots:
    void process();
    uint8_t calcCrc(QByteArray msg);
    void GetData(const QByteArray &inputData);
    void unpackData();
    void readByte(const char &byte);
    void SortData(const QByteArray &inputData);


    bool parse_GPGSV(QByteArray msg);
    bool parse_GPGGA(QByteArray msg);
    bool parse_GPGLL(QByteArray msg);
    bool parse_GPGSA(QByteArray msg);
    bool parse_GPRMC(QByteArray msg);
    bool  parse_GPVTG(QByteArray msg);
    double Ndeg2degree(double val);
    double degree2Ndeg(double val);
    int _nmea_parse_time(const char *buff, int buff_sz, NMEA0183::nmeaTIME *res);
    void OutRmcParametres();

    void outpPIRTR(int ELIPS);
private:

    NMEA0183::GSV_msg GsvMsg;
    NMEA0183::GGA_msg GgaMsg;
    NMEA0183::GLL_msg GllMsg;
    NMEA0183::GSA_msg GsaMsg;
    NMEA0183::RMC_msg RmcMsg;
    NMEA0183::VTG_msg VtgMsg;
    //NMEA0183::nmeaTIME UTCtime;

    QByteArray inputBuffer;
    QByteArray unpackBuffer;

    QTimer *tmr;
};

#endif // NMEA_H
