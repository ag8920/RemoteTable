#include "comport.h"

#include <qdebug.h>
#include <QMessageBox>

comPort::comPort(QObject *parent) :
    QObject(parent)
{

}

comPort::~comPort()
{
    qDebug("by in Thread");
    emit finishedPort();//сигнал о завершении работы
}


void comPort::processPort() //выполняется при старте класса
{
    qDebug("hello world in Thread");
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&thisPort,SIGNAL(readyRead()),this,SLOT(ReadInPort())); //подключаем чтение с порта по сигналу readyread()
}

void comPort::ConnectPort(SettingsDialog::Settings *p)
{

    if(thisPort.isOpen()){
        isNotConnectedPort(tr("Порт уже открыт"));
        return;
    }

    SettingsPort.name = p->name;
    SettingsPort.baudRate = static_cast<QSerialPort::BaudRate> (p->baudRate);
    SettingsPort.dataBits = static_cast<QSerialPort::DataBits> (p->dataBits);
    SettingsPort.parity = static_cast<QSerialPort::Parity> (p->parity);
    SettingsPort.stopBits = static_cast<QSerialPort::StopBits> (p->stopBits);
    SettingsPort.flowControl = static_cast<QSerialPort::FlowControl> (p->flowControl);

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
                isConnectedPort(p->name,p->baudRate,
                                p->dataBits,p->parity,
                                p->stopBits,p->flowControl);
            }
            else
            {
                thisPort.close();
                error_(thisPort.errorString().toLocal8Bit());
                isNotConnectedPort(tr("Невозможно открыть порт %1").arg(p->name));
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
    qDebug("DisconnectedPort");
    if(thisPort.isOpen())
    {
        thisPort.close();
        error_(SettingsPort.name.toLocal8Bit() + " >> Закрыт!\r");
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

void comPort::WriteToPort(QByteArray data) //запись данных в порт
{
    if(thisPort.isOpen())
        thisPort.write(data);
}

void comPort::ReadInPort() //чтение данных из порта
{
    QByteArray data;
    data.append(thisPort.readAll());
    outPort(data);
}
