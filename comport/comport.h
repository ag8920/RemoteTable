#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <comport/settingsdialog/settingsdialog.h>

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
    void outPort(QByteArray data);//сигнал вывода полученных данных

public slots:
    bool DisconnectPort(); //слот отключения порта
    void ConnectPort(SettingsDialog::Settings *p);
    void processPort();//тело
    void WriteToPort(const QByteArray &data);//слот отправки данных в порт
    void ReadInPort();//слот чтения данных из порта
private slots:
    void handleError(QSerialPort::SerialPortError error);//слот обработки ошибок
};

#endif // COMPORT_H
