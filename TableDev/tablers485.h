#ifndef TABLERS485_H
#define TABLERS485_H

#include <QObject>
#include "../comport/comport.h"

const quint8 STARTBYTE=0x5a; //начало посылки
const quint8 DEV=0;//адрес стола
//const quint8 NO_DATA=254;
const quint8 DATA=255;
//результат выполнения
enum class Reports : quint8
{
    RES_OUT=0x31,//значение вне рабочей зоны
    RES_HRD=0x35,//команду невозможно выполнить
    RES_OK=0x36,//команда выполнена успешно
    RES_CS=0xC8,//ошибка контрольной суммы
    RES_COM=0xCB,//недопустимый код команды или номер параметра
    RES_PAR=0xCC //неверные значения данных
};
//режимы работы
enum class modeRotation : quint8
{
    Angle=15,//режим отработки по заданному положению
    Speed=16 //режим отработки с заданной скоростью
};

enum class Command : quint8
{
    MODE=1, //установить режим
    ANGLE_MODE=121,//выполнить поворот по заданному углу
    SPEED_MODE=24,//выполнить поворот с заданной скоростью
    CURR_POS=29,//текущее положение ротора
    STATUS=6 //состояние
};
union FloatType
{
    float fVal;
    char buf[4];
};
union Uint16Type
{
    quint16 iVal;
    char buf[2];
};

unsigned short calcCRC16(unsigned char *pcBlock,unsigned short len);

//------------------------------------------------------------------------------
class MarkerEncoder: public QObject
{
    Q_OBJECT
public:
    explicit MarkerEncoder(QObject *parent = nullptr);
    void Encode(QByteArray &data);
    char EncodeByte(char byte,int delta);
    void Decode(QByteArray &data);

};
//------------------------------------------------------------------------------
class tableRS485 : public QObject
{
    Q_OBJECT
public:
    explicit tableRS485(QObject *parent = nullptr);
    comPort *ComPort;
    QThread *comPortThread;
    MarkerEncoder *encoder;
signals:
    void OutputToComPort(const QByteArray &data);
public slots:
    QByteArray GetMessage(Command CMD, QByteArray &DATA, const quint8 AddByte);
    bool StatusCom();//состояние стола
    bool ModeCom();//выбор режима управления
    bool setSpeed();
    bool setAngle();
};


#endif // TABLERS485_H
