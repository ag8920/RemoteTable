#include "loger.h"
#include <QDateTime>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDebug>


//-----------------------------------------------------------
// Назначение: конструктор класса
//----------------------------------------------------------
loger::loger(QObject *parent) : QObject(parent)
{
    file=nullptr;
}
//-----------------------------------------------------------
// Назначение: деструктор класса
//----------------------------------------------------------
loger::~loger()
{
    qDebug("~loger");
    if(file!=nullptr){
        file->close();
        delete file;
    }
    emit finished();
}
//-----------------------------------------------------------
// Назначение: создает новый .dat файл для записи
//----------------------------------------------------------
void loger::start()
{
    if(file!=nullptr) return;
    QDateTime dateTime=QDateTime::currentDateTime();
    QDir dir;
    QString fileName;

    if(dir.mkpath(QDir::currentPath()+"/record/binout")){
        dir.cd(QDir::currentPath()+"/record/binout");
        fileName=dir.filePath(dateTime.toString("ddMMyy_HHmmss_"));
        file= new QFile(fileName+"binout.dat");

    }else{
        file= new QFile(fileName);
    }
    file->open(QIODevice::WriteOnly);
}
//-----------------------------------------------------------
// Назначение: запись данных в файл .dat
//----------------------------------------------------------
void loger::write(QByteArray data)
{
    if(file==nullptr) return;
    if(file->isOpen()){
        file->write(data);
    }

}
//-----------------------------------------------------------
// Назначение: остановка записи(закрывает файли записи) .dat
//----------------------------------------------------------
void loger::CloseFile()
{
    if(file==nullptr) return;
    if(file->isOpen())
    {
        file->close();
        delete file;
        file=nullptr;
    }
}
//-----------------------------------------------------------
// Назначение: запись данных в .log файл
//----------------------------------------------------------
void loger::PutLog(QString Data)
{
    QDateTime dateTime=QDateTime::currentDateTime();
    QString writeData;
    QDir dir;
    QString fileName;

    if(dir.mkpath(QDir::currentPath()+"/record/log")){
        dir.cd(QDir::currentPath()+"/record/log");
        fileName=dir.filePath(dateTime.toString("yyyy_MM_dd")+".log");
    }
    else
        fileName=dateTime.toString("yyyy_MM_dd")+".log";

    QFile File(fileName);
    QTextStream out(&File);
    if(File.open(QIODevice::Append|QIODevice::Text)){
        writeData=QString("%1 %2\n").arg(dateTime.toString("hh:mm:ss\t"))
                .arg(Data);
        out<<writeData;
        File.close();
    }
}

void loger::PutLog2(QString Data, QString FileName)
{
    QDateTime dateTime=QDateTime::currentDateTime();
    QString writeData;
    QDir dir;
    QString fileName;

    if(dir.mkpath(QDir::currentPath()+"/record/log")){
        dir.cd(QDir::currentPath()+"/record/log");
        fileName=dir.filePath(FileName+".log");
    }
    else
        fileName=FileName+".log";

    QFile File(fileName);
    QTextStream out(&File);
    if(File.open(QIODevice::Append|QIODevice::Text)){
        writeData=QString("%1\n").arg(Data);
        out<<writeData;
        File.close();
    }
}


