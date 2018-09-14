//------------------------------------------------------------------------------
//     Данный модуль создает класс для управления
//     COM портом
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "comport.h"

#include <qdebug.h>
#include <QMessageBox>

comPort::comPort(QObject *parent) :
    QObject(parent)
{

}
//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
comPort::~comPort()
{
    qDebug("comPort::~comPort()");
    DisconnectPort();
    emit finishedPort();//сигнал о завершении работы
}


void comPort::processPort() //выполняется при старте класса
{
    qDebug("comPort::processPort()");
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)),
            this,SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&thisPort,SIGNAL(readyRead()),
            this,SLOT(ReadInPort())); //подключаем чтение с порта по сигналу readyread()
}
//-----------------------------------------------------------
// Назначение: деструктор класса
//-----------------------------------------------------------
void comPort::ConnectPort(QString name, int baudrate, int DataBits,
                          int Parity, int StopBits, int FlowControl)
{
    qDebug("comPort::ConnectPort()");
    if(thisPort.isOpen()){
        isNotConnectedPort(tr("%1 уже открыт").arg(thisPort.portName()));
        return;
    }

    SettingsPort.name = name;
    SettingsPort.baudRate = baudrate;
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
//-----------------------------------------------------------
// Назначение: отключить порт
//-----------------------------------------------------------
bool comPort::DisconnectPort()
{
    qDebug("comPort::DisconnectedPort()");
    if(thisPort.isOpen())
    {
        thisPort.close();
        isNotConnectedPort(tr("%1 закрыт").arg(thisPort.portName()));
        return true;
    }
    else return false;

}
//-----------------------------------------------------------
// Назначение: проверка ошибок
//-----------------------------------------------------------
void comPort::handleError(QSerialPort::SerialPortError error)
{
    if((thisPort.isOpen())
            && (error == QSerialPort::ResourceError))
    {
        error_(thisPort.errorString().toLocal8Bit());
        DisconnectPort();
    }
}

//-----------------------------------------------------------
// Назначение: отправка данных в COM порт
//-----------------------------------------------------------
void comPort::WriteToPort(const QByteArray &data)
{
    if(thisPort.isOpen())
        thisPort.write(data);
}
//-----------------------------------------------------------
// Назначение: чтение данных из COM порта
//-----------------------------------------------------------
void comPort::ReadInPort()
{
    QByteArray inputData;
    inputData.append(thisPort.readAll());
    dataOutput(inputData);
}
//-----------------------------------------------------------
// Назначение: вызов сигнала для остановки потока
//-----------------------------------------------------------
void comPort::Stop()
{
    qDebug("comPort::stop()");
//    this->DisconnectPort();
    emit finishedPort();
}

