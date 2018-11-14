#include "tablers232.h"

tableRS232::tableRS232(QObject *parent) : QObject(parent)
{
    DeviceComPort = new comPort;
    ComPortThread = new QThread;
}

void tableRS232::ExecutePos()
{
    QString str;
    str= str="mo=0;um=5;mo=1;SP="+speed+typePositioning
            +angle+";bg;";
    SendCMD(str);
}

void tableRS232::ZeroPosition()
{
    QString str="mo=0;um=5;mo=1;SP="+speed+";PA=0;bg;";
    SendCMD(str);
}

void tableRS232::GetPosition()
{

}

void tableRS232::GoToPosition(QVariant position)
{
    QString str;
    nextPosition=position.toInt();
    str="mo=0;um=5;mo=1;SP="+speed+";PA="
                    +position.toString()+";bg;";
    SendCMD(str);
}

void tableRS232::RequestPosition()
{
    QString str="px;";
    SendCMD(str);
}

void tableRS232::OnMotion()
{
    QString str="mo=1;";
    SendCMD(str);
}

void tableRS232::OffMotion()
{
    QString str="mo=0;";
    SendCMD(str);
}

void tableRS232::BeginMotion()
{
    QString str="bg;";
    SendCMD(str);
}

void tableRS232::StopMotion()
{
    QString str="st;";
    SendCMD(str);
}

void tableRS232::FinishedMotion()
{
    QString str="st;mo=0;";
    SendCMD(str);
}

void tableRS232::ResetAbsCoord()
{
    QString str="mo=0;px=0;mo=1;";
}

void tableRS232::SendCMD(const QString &cmd)
{
    emit OutputToComPort(cmd.toLocal8Bit());
}

void tableRS232::SetSpeed(const QString &str)
{
    speed=str;
}

void tableRS232::SetAngle(const QString &str)
{
    angle=str;
}

void tableRS232::SetTypePositioning(bool pos)
{
    if (pos==true)
        typePositioning=";PA";
    else
        typePositioning=";PR";
}

void tableRS232::AddThreads()
{
    DeviceComPort->moveToThread(ComPortThread); //помещаем класс в поток
    DeviceComPort->thisPort.moveToThread(ComPortThread);//помещаем порт в поток

    connect(ComPortThread,&QThread::started,
            DeviceComPort,&comPort::processPort);
    connect(DeviceComPort,&comPort::finishedPort,
            ComPortThread,&QThread::quit);
    connect(ComPortThread,&QThread::finished,
            DeviceComPort,&comPort::deleteLater);
    connect(DeviceComPort,&comPort::finishedPort,
            ComPortThread,&QThread::deleteLater);
//    connect(this,&TableDevice::StopAll,
//            DeviceComPort,&comPort::Stop);

    ComPortThread->start(QThread::TimeCriticalPriority);

}
