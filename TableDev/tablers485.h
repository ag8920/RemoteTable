#ifndef TABLERS485_H
#define TABLERS485_H

#include <QObject>
#include "../comport/comport.h"

const quint8 STARTBYTE=0x5a; //начало посылки
const quint8 ENDBYTE=0x65;
const quint8 DEV=0;//адрес стола
//const quint8 NO_DATA=254;
const quint8 DATA=255;
//результат выполнения
enum //uint8 Reports : uint8_t
{
    RES_OUT=0x31,//значение вне рабочей зоны
    RES_HRD=0x35,//команду невозможно выполнить
    RES_OK=0x36,//команда выполнена успешно
    RES_CS=0x48,//ошибка контрольной суммы
    RES_COM=0x4B,//недопустимый код команды или номер параметра
    RES_PAR=0x4C //неверные значения данных
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
    STATUS=6, //состояние
    CURR_VEL=5 //текущая скорость
};
union FloatType
{
    long lVal;
    float fVal;
    char buf[4];
};
union Int32Type
{
    int32_t iVal;
    char buf[4];
};

union Int16Type
{
    int16_t iVal;
    char buf[2];
};



typedef struct
{
    //    quint8 Point:6,Tag:2;
    uint8_t  Point;
    int32_t Data;
    uint16_t CRC;
    uint8_t  STOP;
}AnswerRequestPos;

//----------------------------------
typedef struct
{
    uint16_t
        state:2,
        U:1,
        reserve:4,
        emergencyU:1,
        Uminimum:1,
        Umaximum:1,
        Overheat:1,
        emergencyI:1,
        res:1,
        shutdown:1,
        programmEmphasis:1

        ;
} _state;
typedef union {
    _state bits;
    uint16_t words;
} state;
typedef struct
{
    //    quint8 Point:6,Tag:2;
    uint8_t  Point;
    /*uint16_t*/state Data;
    uint16_t CRC;
    uint8_t  STOP;
}AnswerRequestSost;
//----------------------------------
typedef struct
{
    uint8_t  ResCod;
    uint8_t  TagPoint;
    int32_t Data;
    uint16_t CRC;
    uint8_t  STOP;
}AnswerAngleDrive;
//--------------------------------------
typedef struct
{
    uint8_t  Point;
    int16_t Data;
    uint16_t CRC;
    uint8_t  STOP;
}AnswerRequestSpeed;
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
    ~tableRS485();
    comPort *ComPort;
    MarkerEncoder *encoder;
    double getCurrentPos() const;


    double getCurrentSpeed() const;

signals:
    void OutputToComPort(const QByteArray &data);
    void signalErrorMsg(const QString &msg);
    void sendMsg(const QString &msg);
    void sendStateMsg(const QString &msg);
public slots:
    QByteArray GetMessage(Command cmd, QByteArray &data,
                          const quint8 addByte, const char numDev);
    bool StatusCom();//состояние стола
    bool SpinMode();//выбор режима управления
    bool TurnMode();
    bool setSpeed(float speed);
    bool setAngle(float angle);
    void RequestPos();
    void RequestCurrentSpeed();


private:
    void ErrorAnalyze(unsigned char errbyte, QString func);
    double currentPos;
    double currentSpeed;
};



namespace a_math {
unsigned short calcCRC16(unsigned char *pcBlock,unsigned short len);
double valfixd(const long data, const double scale);
long fixd(const double value, const double scale);
float valfixw(const int data, const float scale);
int fixw(const float value, const float scale);
}

#endif // TABLERS485_H
