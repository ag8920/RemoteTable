#include "loger.h"
#include <QDateTime>
#include <QFile>
#include <QDataStream>
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
// Назначение: создает новый файл для записи
//----------------------------------------------------------
void loger::start()
{
    if(file!=nullptr) return;
    qDebug("start loger");
    qDebug()<< QThread::currentThread();
    QDateTime dateTime=QDateTime::currentDateTime();
    QString fileName = dateTime.toString("ddMMyy_HHmmss_")+"binout.dat";
    file= new QFile(fileName);
    file->open(QIODevice::WriteOnly);
}
//-----------------------------------------------------------
// Назначение: запись данных в файл
//----------------------------------------------------------
void loger::write(QByteArray data)
{
    if(file==nullptr) return;
    if(file->isOpen()){
        file->write(data);
    }

}
//-----------------------------------------------------------
// Назначение: остановка записи(закрывает файли записи)
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
