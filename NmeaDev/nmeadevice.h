#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QAction>
#include <QActionGroup>
#include "comport/comport.h"
#include "comport/settingsdialog/settingsdialog.h"
#include "nmea/nmea.h"


#define ORGANIZATION_NAME "SHCH inc"
#define ORGANIZATION_DOMAIN "www.demo.ru"
#define APPLICATION_NAME "NMEA_0183"


struct _RmcPacket
{
    QString Status;
    int     year;       /**< Years since 1900 */
    int     mon;        /**< Months since January - [0,11] */
    int     day;        /**< Day of the month - [1,31] */
    int     hour;       /**< Hours since midnight - [0,23] */
    int     min;        /**< Minutes after the hour - [0,59] */
    int     sec;        /**< Seconds after the minute - [0,59] */
    int     hsec;
    double Lat;
    double Lon;
    double Speed;
    double Course;
    double MagneticVariation;
    double H;
    int SatelliteUsed;
    double HDOP;

};

namespace Ui {
class NmeaDevice;
}

class NmeaDevice : public QMainWindow
{
    Q_OBJECT

public:
    explicit NmeaDevice(QWidget *parent = nullptr);
    ~NmeaDevice();
private:
    Ui::NmeaDevice *ui;
    SettingsDialog *SettingsComPort;
    comPort *ComPort;
    QThread *ComPortThread;
    bool isOpenPort;

    nmea *GPSdata;
    QThread *GPSdataThread;
    _RmcPacket RmcPacket;
    QTimer *tmr;
    QFile *file;

    void AddThread();


    QAction *actionPz9002;
    QAction *actionPz90;
    QAction *actionWgs84;
    QAction *actionSK42;
    QAction *actionSK95;
    QActionGroup *actionGroupSK;



private slots:
    void ReadFile();
signals:
    void DisconnectComPort();
    void ConnectComPort(QString name,int baudrate,
                        int DataBits,int Parity,
                        int StopBits,int FlowControl);
    void showPortSettings();
    void signalOutputCommand(int typeElips);
//    void sendBasicData(QByteArray data);
    void sendBasicData(double *Lat, double *Lon, double *H);
public slots:
    void slotConnectPort();
    void PortIsOpen();
    void OpenSerialPort();
    void showMsg(const QString &msg);
    void updateNavInf(QByteArray data);

    void slotOutputCommand();
};

#endif // MAINWINDOW_H
