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
    fileName = dateTime.toString("ddMMyy_HHmmss_");//+"binout.dat";
    if(dir.mkpath(QDir::currentPath()+"/record/binout")){
        file= new QFile(dir.filePath(fileName)+"binout.dat");
    }else
        file= new QFile(fileName);
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
    QFile File(dateTime.toString("yyyy.MMM.dd")+".log");
    QTextStream out(&File);
    if(File.open(QIODevice::Append|QIODevice::Text)){
        writeData=QString("%1 %2\n").arg(dateTime.toString("hh::mm::ss\n"))
                .arg(Data);
        out<<writeData;
        File.close();
    }
}
