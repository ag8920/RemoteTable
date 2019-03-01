#include "tablers232.h"

tableRS232::tableRS232(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------
// Назначение:  выполнить позиционирование
//-----------------------------------------------------------
void tableRS232::ExecutePos()
{
    QString str;
    str= str="mo=0;um=5;mo=1;SP="+speed+typePositioning
            +angle+";bg;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: возрат в нулевое положение
//-----------------------------------------------------------
void tableRS232::ZeroPosition()
{
    QString str="mo=0;um=5;mo=1;SP="+speed+";PA=0;bg;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: получение текущей позиции и
//             определение движения поворотного стола
//-----------------------------------------------------------
int tableRS232::GetPosition(const QByteArray &data)
{
    static QString str=nullptr;
    static bool start=false;
    static bool end=false;

    if(data.startsWith("px;")){start=true; end=false;}
    if(start && !end)
    {
        for(int pos=0;pos<data.length();pos++)
        {
            QChar symbol=data.at(pos);
            if((symbol>='0' && symbol<='9')|| symbol=='-')
            {
                str+=symbol;
            }
        }
    }
    if(data.endsWith(";")) {end=true;start=false;}
    return str.toInt();
}
//-----------------------------------------------------------
// Назначение: абсолютное позиционирование
//-----------------------------------------------------------
void tableRS232::GoToPosition(QVariant position)
{
    QString str;
    str="mo=0;um=5;mo=1;SP="+speed+";PA="
            +position.toString()+";bg;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: Запрос текущего положения
//-----------------------------------------------------------
void tableRS232::RequestPosition()
{
    QString str="px;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: Включить привод
//-----------------------------------------------------------
void tableRS232::OnMotion()
{
    QString str="mo=1;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: отключить привод
//-----------------------------------------------------------
void tableRS232::OffMotion()
{
    QString str="mo=0;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: начать вращение(команда старт)
//-----------------------------------------------------------
void tableRS232::BeginMotion()
{
    QString str="bg;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: остановить вращение(команда стоп)
//-----------------------------------------------------------
void tableRS232::StopMotion()
{
    QString str="st;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: остановить вращение(команда стоп)
// и отключить привод
//-----------------------------------------------------------
void tableRS232::FinishedMotion()
{
    QString str="st;mo=0;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: сброс абсолютной координаты
//-----------------------------------------------------------
void tableRS232::ResetAbsCoord()
{
    QString str="mo=0;px=0;mo=1;";
}
//-----------------------------------------------------------
// Назначение: отправка сообщения
//-----------------------------------------------------------
inline void tableRS232::SendCMD(const QString &cmd)
{
    emit OutputToComPort(cmd.toLocal8Bit());
}
//-----------------------------------------------------------
// Назначение: установить скорость вращения стола
//-----------------------------------------------------------
void tableRS232::SetSpeed(const QString &str)
{
    speed=str;
}
//-----------------------------------------------------------
// Назначение: установить угол поворота
//-----------------------------------------------------------
void tableRS232::SetAngle(const QString &str)
{
    angle=str;
}
//-----------------------------------------------------------
// Назначение: установить направление движения
// по часовой/ против часовой
//-----------------------------------------------------------
void tableRS232::SetTypePositioning(bool pos)
{
    if (pos==true)
        typePositioning=";PA";
    else
        typePositioning=";PR";
}

//void tableRS232::AddThreads()
//{
//    DeviceComPort->moveToThread(ComPortThread); //помещаем класс в поток
//    DeviceComPort->thisPort.moveToThread(ComPortThread);//помещаем порт в поток

//    connect(ComPortThread,&QThread::started,
//            DeviceComPort,&comPort::processPort);
//    connect(DeviceComPort,&comPort::finishedPort,
//            ComPortThread,&QThread::quit);
//    connect(ComPortThread,&QThread::finished,
//            DeviceComPort,&comPort::deleteLater);
//    connect(DeviceComPort,&comPort::finishedPort,
//            ComPortThread,&QThread::deleteLater);
//    //    connect(this,&TableDevice::StopAll,
//    //            DeviceComPort,&comPort::Stop);

//    ComPortThread->start(QThread::TimeCriticalPriority);

//}
