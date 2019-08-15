#include "nmeadevice.h"
#include "ui_nmeadevice.h"
#include <QDataStream>
#include <QActionGroup>


enum typeSK{WGS84,PZ90,SK42,SK95,PZ9002};

NmeaDevice::NmeaDevice(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NmeaDevice)
{
    ui->setupUi(this);
    this->setWindowTitle(APPLICATION_NAME);
    this->setWindowIcon(QIcon(":/icons/satellite.png"));
    this->setWindowFlags(Qt::Tool);

    ui->actionConnect->setIcon(QIcon(":/icons/disconnect.png"));

    actionPz90=new QAction("ПЗ-90",this);
    actionPz90->setCheckable(true);
    actionPz9002=new QAction("ПЗ-90.02",this);
    actionPz9002->setCheckable(true);
    actionSK42=new QAction("СК-42",this);
    actionSK42->setCheckable(true);
    actionSK95=new QAction("СК-95",this);
    actionSK95->setCheckable(true);
    actionWgs84=new QAction("WGS-84",this);
    actionWgs84->setCheckable(true);
    actionGroupSK=new QActionGroup(this);
    actionGroupSK->addAction(actionWgs84);
    actionGroupSK->addAction(actionPz9002);
    actionGroupSK->addAction(actionPz90);
    actionGroupSK->addAction(actionSK42);
    actionGroupSK->addAction(actionSK95);
    actionWgs84->setChecked(true);
    actionGroupSK->setEnabled(false);
    ui->menu->addAction(actionWgs84);
    ui->menu->addAction(actionPz9002);
    ui->menu->addAction(actionPz90);
    ui->menu->addAction(actionSK42);
    ui->menu->addAction(actionSK95);
    ui->mainToolBar->hide();

    connect(actionGroupSK, &QActionGroup::triggered,
            this, &NmeaDevice::slotOutputCommand);

    isOpenPort=false;
    SettingsComPort=new SettingsDialog(nullptr,1);
    SettingsComPort->setWindowIcon(QIcon(":/icons/serialport.png"));

    ComPort=new comPort;
    ComPortThread=new QThread;

    GPSdata=new nmea;
    GPSdataThread=new QThread;

    tmr=new QTimer;
    RmcPacket={};

    this->AddThread();
    connect(ui->actionConnect, &QAction::triggered, this,
            &NmeaDevice::slotConnectPort);
    connect(this, &NmeaDevice::showPortSettings,
            SettingsComPort, &SettingsDialog::showWidget);
    connect(SettingsComPort, &SettingsDialog::isUpdateSettings,
            this, &NmeaDevice::OpenSerialPort);
    connect(this, &NmeaDevice::ConnectComPort,
            ComPort, &comPort::ConnectPort);
    connect(this, &NmeaDevice::DisconnectComPort,
            ComPort, &comPort::DisconnectPort);
    connect(ComPort, &comPort::isConnectedPort,
            this, &NmeaDevice::PortIsOpen);
    connect(ComPort,&comPort::isConnectedPort,
            this,&NmeaDevice::showMsg);
    connect(ComPort,&comPort::isNotConnectedPort,
            this,&NmeaDevice::showMsg);

    connect(GPSdata,&nmea::outRmcNavData,this, &NmeaDevice::updateNavInf);
    connect(ComPort, &comPort::dataOutput,GPSdata, &nmea::GetData);
    connect(tmr, &QTimer::timeout,this, &NmeaDevice::ReadFile);
    connect(GPSdata,&nmea::OutputData,ComPort, &comPort::WriteToPort);

    connect(this, &NmeaDevice::signalOutputCommand,
            GPSdata, &nmea::outpPIRTR);
//    tmr->start(10);
//    ComPort->processPort();
//    this->GPSdata->process();
    file=new QFile(this);
    file->setFileName("s:/work/project/main/Qt/nmeaGPS/nmeaGps/simple.log");
    if(!file->open(QIODevice::ReadOnly))
        qWarning()<<"Error:cannot open source file "<<file->fileName();


    updateNavInf(nullptr);
//    emit ConnectComPort("COM2",115200,8,0,1,0);
}

NmeaDevice::~NmeaDevice()
{
    qDebug("NmeaDevice::~NmeaDevice()");
    ComPortThread->quit();
    delete ui;
}
void NmeaDevice::AddThread()
{
    ComPort->moveToThread(ComPortThread);
    ComPort->thisPort.moveToThread(ComPortThread);
    connect(ComPortThread,&QThread::started,
            ComPort, &comPort::processPort);
    connect(ComPort, &comPort::finishedPort,
            ComPortThread, &QThread::quit);
    connect(ComPortThread, &QThread::finished,
            ComPort, &comPort::deleteLater);
    connect(ComPort, &comPort::finishedPort,
            ComPortThread, &QThread::deleteLater);
    ComPortThread->start(QThread::TimeCriticalPriority);

    GPSdata->moveToThread(GPSdataThread);
    connect(GPSdataThread, &QThread::started,
            GPSdata, &nmea::process);
    connect(GPSdata, &nmea::finished,
            GPSdataThread, &QThread::quit);
    GPSdataThread->start();

}

void NmeaDevice::ReadFile()
{
    if (!file->atEnd())
    {GPSdata->GetData(file->readLine().trimmed());}
     else file->seek(0);
}

void NmeaDevice::PortIsOpen()
{
    isOpenPort=true;
    ui->actionConnect->setText(tr("Отключить"));
    ui->actionConnect->setIcon(QIcon(":/icons/connect.png"));
    actionGroupSK->setEnabled(true);

}
void NmeaDevice::slotConnectPort()
{
    if(!isOpenPort)
    {
        emit showPortSettings();
    }
    else if(isOpenPort)
    {
        emit DisconnectComPort();
        ui->actionConnect->setText(tr("Подключить"));
        ui->actionConnect->setIcon(QIcon(":/icons/disconnect.png"));
        isOpenPort=false;
        actionGroupSK->setEnabled(false);

    }

}
void NmeaDevice::OpenSerialPort()
{

    SettingsDialog::Settings p=SettingsComPort->settings();
    QString name=static_cast<QString>(p.name);
    int baudRate=static_cast<int>(p.baudRate);
    int dataBits=static_cast<int>(p.dataBits);
    int parity=static_cast<int>(p.parity);
    int stopBits=static_cast<int>(p.stopBits);
    int flowControl=static_cast<int>(p.flowControl);
    //emit ConnectComPort(p);
    emit ConnectComPort(name,baudRate,dataBits,parity,stopBits,flowControl);

}

void NmeaDevice::showMsg(const QString &msg)
{
    ui->statusBar->showMessage(msg,5000);
}

void NmeaDevice::updateNavInf(QByteArray data)
{
    QString Status={};
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_3);
    in>>RmcPacket.Status
            >>RmcPacket.year>>RmcPacket.mon>>RmcPacket.day>>RmcPacket.hour
            >>RmcPacket.min>>RmcPacket.sec>>RmcPacket.hsec
            >>RmcPacket.Lat>>RmcPacket.Lon>>RmcPacket.Speed
            >>RmcPacket.Course>>RmcPacket.MagneticVariation>>RmcPacket.H
            >>RmcPacket.SatelliteUsed>>RmcPacket.HDOP;
    RmcPacket.Speed*=0.5144;//узлы->м/сек
    if(RmcPacket.Status=="V")Status=tr("Решение не годно");
    else if(RmcPacket.Status=="A")Status=tr("Автономный режим");
    else if(RmcPacket.Status=="D")Status=tr("Дифференциальный режим");
    else Status=tr("Решение отсутствует");
    ui->label->setText("год: "+QString::number(RmcPacket.year)+"\n"
                       +"месяц: "+QString::number(RmcPacket.mon)+"\n"
                       +"день: "+QString::number(RmcPacket.day)+"\n"
                       +"время: "+QString::number(RmcPacket.hour)+":"
                       +QString::number(RmcPacket.min)+":"
                       +QString::number(RmcPacket.sec)+"."
                       +QString::number(RmcPacket.hsec)+"\n"
                       +"Широта: "+QString::number(RmcPacket.Lat,'g',8)+"\n"
                       +"Долгота: "+QString::number(RmcPacket.Lon,'g',8)+"\n"
                       +"Высота: "+QString::number(RmcPacket.H,'g',8)+"\n"
                       +"Скорость: "+QString::number(RmcPacket.Speed,'g',8)+"\n"
                       +"Курс: "+QString::number(RmcPacket.Course,'g',8)+"\n"
                       +"Магнит.склонение: "+QString::number(RmcPacket.MagneticVariation,'g',8)+"\n"
                       +"Статус: "+Status+"\n"
                       +"Спутники: "+QString::number(RmcPacket.SatelliteUsed)+"\n"
                       +"HDOP: "+QString::number(RmcPacket.HDOP));



    QByteArray outData={};
    QDataStream out(&outData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    out<<RmcPacket.Lat<<RmcPacket.Lon<<RmcPacket.H<<RmcPacket.Speed<<RmcPacket.Status;
//    emit sendBasicData(outData);
    if((RmcPacket.Status=="A") || (RmcPacket.Status=="D"))
        emit sendBasicData(&RmcPacket.Lat,&RmcPacket.Lon,&RmcPacket.H);

}

void NmeaDevice::slotOutputCommand()
{
    if(actionPz90->isChecked())
        emit signalOutputCommand(PZ90);
    else if(actionPz9002->isChecked())
       emit signalOutputCommand(PZ9002);
    else if(actionWgs84->isChecked())
        emit signalOutputCommand(WGS84);
    else if(actionSK42->isChecked())
        emit signalOutputCommand(SK42);
    else if(actionSK95->isChecked())
        emit signalOutputCommand(SK95);
}
