#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <comport/settingsdialog/settingsdialog.h>

const char S_END = static_cast<char>(0xC0); // end of packet
const char S_ESC = static_cast<char>(0xdb); // byte stuffing
const char S_ESC_END = static_cast<char>(0xDC); // ESC ESC_END means END data
const char S_ESC_ESC = static_cast<char>(0xDD); // ESC ESC_ESC means ESC data


struct SettingsComPort{
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

class comPort: public QObject
{
    Q_OBJECT
public:
    explicit comPort(QObject *parent=nullptr);
    ~comPort();

    QSerialPort thisPort;
    SettingsComPort SettingsPort;

signals:
    void isConnectedPort(const QString msg); //сигнал успешного подключения порта
    void isNotConnectedPort(const QString msg); //сигнал неудачного открытия порта

    void finishedPort(); //сигнал закрытия класса
    void error_(QString err); //сигнал ошибок порта
    void dataOutput(QByteArray data);//сигнал вывода полученных данных

public slots:
    bool DisconnectPort(); //слот отключения порта
    void ConnectPort(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);
    void processPort();//тело
    void WriteToPort(const QByteArray &data);//слот отправки данных в порт
    void ReadInPort();//слот чтения данных из порта
    void Stop();
private slots:
    void handleError(QSerialPort::SerialPortError error);//слот обработки ошибок
private:
    char SlipDecode(QByteArray b, QByteArray &b2);
};

#endif // COMPORT_H
