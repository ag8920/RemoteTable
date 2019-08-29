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
QString tableRS232::GetPosition(const QByteArray &data)
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
    if(data.endsWith(";")) {end=true;start=false;
    return str;
    }
    return nullptr;
}
//-----------------------------------------------------------
// Назначение: абсолютное позиционирование
//-----------------------------------------------------------
void tableRS232::GoToPosition(QVariant position)
{
    QString str;
    str=/*"mo=0;um=5;mo=1;*/ "SP="+speed+";PA="
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
    SendCMD(str);
}

void tableRS232::DirectionTurn()
{
    QString str=dirrection+speed+";";
    SendCMD(str);
}

QString tableRS232::getDirrection() const
{
    return dirrection;
}

void tableRS232::setDirrection(const QString &value)
{
    dirrection = value;
}

void tableRS232::initMotion()
{
    QString str="mo=0;um=5;mo=1;";
    SendCMD(str);
}
//-----------------------------------------------------------
// Назначение: отправка сообщения
//-----------------------------------------------------------
inline void tableRS232::SendCMD(const QString &cmd)
{
    emit OutputToComPort(cmd.toLocal8Bit());
}

////-----------------------------------------------------------
//// Назначение: установить направление движения
//// по часовой/ против часовой
////-----------------------------------------------------------
//void tableRS232::SetTypePositioning(bool pos)
//{
//    if (pos==true)
//        typePositioning=";PA";
//    else
//        typePositioning=";PR";
//}

QString tableRS232::getTypePositioning() const
{
    return typePositioning;
}

void tableRS232::setTypePositioning(const QString &value)
{
    typePositioning = value;
}

QString tableRS232::getAngle() const
{
    return angle;
}

void tableRS232::setAngle(const QString &value)
{
    angle = value;
}

QString tableRS232::getSpeed() const
{
    return speed;
}

void tableRS232::setSpeed(const QString &value)
{
    speed = value;
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
