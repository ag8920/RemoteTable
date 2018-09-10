#include "comport.h"

#include <qdebug.h>
#include <QMessageBox>

comPort::comPort(QObject *parent) :
    QObject(parent)
{

}

comPort::~comPort()
{
    qDebug("comPort::~comPort()");
    DisconnectPort();
    emit finishedPort();//сигнал о завершении работы
}


void comPort::processPort() //выполняется при старте класса
{
    qDebug("comPort::processPort()");
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&thisPort,SIGNAL(readyRead()),this,SLOT(ReadInPort())); //подключаем чтение с порта по сигналу readyread()
}

void comPort::ConnectPort(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl)
{
    qDebug("comPort::ConnectPort()");
    if(thisPort.isOpen()){
        isNotConnectedPort(tr("Порт уже открыт"));
        return;
    }

    SettingsPort.name = name;
    SettingsPort.baudRate = static_cast<QSerialPort::BaudRate> (baudrate);
    SettingsPort.dataBits = static_cast<QSerialPort::DataBits> (DataBits);
    SettingsPort.parity = static_cast<QSerialPort::Parity> (Parity);
    SettingsPort.stopBits = static_cast<QSerialPort::StopBits> (StopBits);
    SettingsPort.flowControl = static_cast<QSerialPort::FlowControl> (FlowControl);

    thisPort.setPortName(SettingsPort.name);

    if(thisPort.open(QIODevice::ReadWrite))
    {
        if(thisPort.setBaudRate(SettingsPort.baudRate)
           && thisPort.setDataBits(SettingsPort.dataBits)
           && thisPort.setParity(SettingsPort.parity)
           && thisPort.setStopBits(SettingsPort.stopBits)
           && thisPort.setFlowControl(SettingsPort.flowControl))
        {
            if(thisPort.isOpen())
            {
                isConnectedPort(tr("Открыт порт: %1, %2, %3, %4, %5, %6")
                                .arg(name).arg(baudrate)
                                .arg(DataBits).arg(Parity)
                                .arg(StopBits).arg(FlowControl));
            }
            else
            {
                thisPort.close();
                error_(thisPort.errorString().toLocal8Bit());
                isNotConnectedPort(tr("Невозможно открыть порт %1").arg(name));
            }
        }
    }
    else
    {
        thisPort.close();
        error_(thisPort.errorString().toLocal8Bit());
        isNotConnectedPort(tr("Ошибка открытия порта"));
    }
}

bool comPort::DisconnectPort() //отключаем порт
{
    qDebug("comPort::DisconnectedPort()");
    if(thisPort.isOpen())
    {
        thisPort.close();
        isNotConnectedPort(tr("Com-порт закрыт"));
        return true;
    }
    else return false;

}

void comPort::handleError(QSerialPort::SerialPortError error) //проверка ошибок в работе
{
    if((thisPort.isOpen())
            && (error == QSerialPort::ResourceError))
    {
        error_(thisPort.errorString().toLocal8Bit());
        DisconnectPort();
    }
}

void comPort::WriteToPort(const QByteArray &data) //запись данных в порт
{
    if(thisPort.isOpen())
        thisPort.write(data);
}

void comPort::ReadInPort() //чтение данных из порта
{
    QByteArray data;
    data.append(thisPort.readAll());
    dataOutput(data);
}

void comPort::Stop()
{
    qDebug("comPort::stop()");
//    this->DisconnectPort();
    emit finishedPort();
}

