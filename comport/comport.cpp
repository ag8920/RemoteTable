#include "comport.h"

#include <qdebug.h>
#include <QMessageBox>

comPort::comPort(QObject *parent) :
    QObject(parent)
{

}
//-----------------------------------------------------------
// Назначение:
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
// Назначение:
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
// Назначение:
//-----------------------------------------------------------
bool comPort::DisconnectPort() //отключаем порт
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
// Назначение:
//-----------------------------------------------------------
void comPort::handleError(QSerialPort::SerialPortError error) //проверка ошибок в работе
{
    if((thisPort.isOpen())
            && (error == QSerialPort::ResourceError))
    {
        error_(thisPort.errorString().toLocal8Bit());
        DisconnectPort();
    }
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
char comPort::SlipDecode(QByteArray b, QByteArray &b2)
{
    if(b.contains(S_END)){
        int j=0;
        while (b.at(j)!=S_END) {
           if(b.at(j)==S_ESC && b.at(j+1)!=S_ESC_END && b.at(j+1)!=S_ESC_ESC)
               return 2;
           if(b.at(j)==S_ESC && b.at(j+1)!=S_ESC_END){
              b2.append(S_END);
               j++;
           }
           else if(b.at(j)==S_ESC && b.at(j+1)==S_ESC_ESC){
               b2.append(S_ESC);
               j++;
           }
           else
               b2.append(b.at(j));
           j++;
        } return 1;
    }
    else
        return  0;
}

//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void comPort::WriteToPort(const QByteArray &data) //запись данных в порт
{
    if(thisPort.isOpen())
        thisPort.write(data);
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void comPort::ReadInPort() //чтение данных из порта
{
    QByteArray inputData;
    inputData.append(thisPort.readAll());
    dataOutput(inputData);

//    QByteArray decodeInputData;
//    char r=SlipDecode(inputData,decodeInputData);
//    dataOutput(decodeInputData);
//    if(r==1){
//        //разбор пакета
//        qDebug("SlipDecode is Ok");
//    }
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void comPort::Stop()
{
    qDebug("comPort::stop()");
//    this->DisconnectPort();
    emit finishedPort();
}

