#include "tablers485.h"
//------------------------------------------------------------------------------
// Релизация класса управления поворотным столом
//------------------------------------------------------------------------------
#include "tablers485.h"
#include <QByteArray>
#include "a_math.h"
//------------------------------------------------------------------------------
tableRS485::tableRS485(QObject *parent) : QObject(parent)
{
    encoder=new MarkerEncoder;
    ComPort=new comPort;
    currentPos=0.;
    currentSpeed=0.;
    qDebug()<<"tableRS485";
}
//------------------------------------------------------------------------------
tableRS485::~tableRS485()
{
    delete ComPort;
    qDebug()<<"~tableRS485";
}
//------------------------------------------------------------------------------
QByteArray tableRS485::GetMessage(Command cmd, QByteArray &data,
                                  const quint8 addByte, const char numDev)
{
    QByteArray sendData;//=new QByteArray;
    QByteArray bodyData;//=new QByteArray;
    quint16 crc;
    sendData.append(STARTBYTE);

    bodyData.append(numDev);
    bodyData.append(static_cast<char>(addByte));
    bodyData.append(static_cast<char>(cmd));
    if(!data.isNull()){
        bodyData.append(data);
    }
    crc=a_math::calcCRC16((unsigned char*)bodyData.data(),bodyData.size());

    bodyData.append(static_cast<char>(crc&0xFF));
    bodyData.append(static_cast<char>(crc>>8));

    encoder->Encode(bodyData);
    return sendData.append(bodyData);
}
//------------------------------------------------------------------------------
bool tableRS485::StatusCom()
{
    AnswerRequestSost Msg;
    QByteArray data;
    QByteArray sendData=GetMessage(Command::STATUS,data,254,0);
    QByteArray answerData=ComPort->writeAndRead(sendData);
    if(answerData.endsWith(ENDBYTE))
    {
        if(answerData.size()==2)
        {
            ErrorAnalyze(static_cast<unsigned char>(answerData[0]),
                         "Команда режим вращения:");
            return false;
        }
        else{
            encoder->Decode(answerData);
            QDataStream stream(&answerData,QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream>>Msg.Point>>Msg.Data.words>>Msg.CRC>>Msg.STOP;
            if(Msg.CRC==a_math::calcCRC16((unsigned char*)answerData.data(),3))
            {
                QString stateMsg="Состояние поворотного стола:\n";
                if(Msg.Data.bits.state==0)stateMsg+="-Двигатель выключен;\n";
                else if (Msg.Data.bits.state==1) {
                    stateMsg+="-Двигатель включен;\n";
                }else if (Msg.Data.bits.state==2) {
                    stateMsg+="-Двигатель поддерживает заданную скорость;\n";
                }else if (Msg.Data.bits.state==3) {
                    stateMsg+="-Двигатель поддерживает заданное положение;\n";
                }else if (Msg.Data.bits.state==4) {
                    stateMsg+="-Выполняется инициализация;\n";
                }else if (Msg.Data.bits.state==5) {
                    stateMsg+="-Выполняется циклограмма выключения;\n";
                }else if (Msg.Data.bits.state==6) {
                    stateMsg+="-Выполняется циклограмма включения;\n";
                }
                if(Msg.Data.bits.U)stateMsg+="-Силовое напряжение в норме;\n";
                else stateMsg+="-Силовое напряжение не в норме;\n";
                if(Msg.Data.bits.emergencyU)stateMsg+="-Выполнено аварийное отключение:\n";
                if(Msg.Data.bits.Uminimum)stateMsg+="  низкое силовое напряжение\n";
                if(Msg.Data.bits.Umaximum)stateMsg+="  высокое силовое напряжение\n";
                if(Msg.Data.bits.Overheat)stateMsg+="  перегрев двигателя\n";
                if(Msg.Data.bits.emergencyI)stateMsg+="  перегрузка по току\n";
                if(Msg.Data.bits.shutdown)stateMsg+="  сбой в электронике\n";
                if(Msg.Data.bits.programmEmphasis)stateMsg+="Программные упоры не установлены(работа в режиме поддержания угла невозможна)\n";
                emit sendStateMsg(stateMsg);
            }
        }
    }
    return true;
}
//------------------------------------------------------------------------------
bool tableRS485::SpinMode()
{
    QByteArray data;
    Int16Type val;
    val.iVal=16;
    data.append(val.buf,2);//todo
    QByteArray sendData=GetMessage(Command::MODE,data,DATA,0);
    QByteArray answerData=ComPort->writeAndRead(sendData);
    if(answerData.endsWith(ENDBYTE))
    {
        if(answerData.size()==2)
        {
            ErrorAnalyze(static_cast<unsigned char>(answerData[0]),
                         "Команда режим ""вращения"":");
            return false;
        }
        else{
            encoder->Decode(answerData);
        }
    }
    return true;
}
//------------------------------------------------------------------------------
bool tableRS485::TurnMode()
{
    QByteArray data;
    Int16Type val;
    val.iVal=15;
    data.append(val.buf,2);//todo
    QByteArray sendData=GetMessage(Command::MODE,data,DATA,0);
    QByteArray answerData=ComPort->writeAndRead(sendData);
    if(answerData.endsWith(ENDBYTE))
    {
        if(answerData.size()==2)
        {
            ErrorAnalyze(static_cast<unsigned char>(answerData[0]),
                         "Команда режим ""Поворот"":");
            return false;
        }
        else{
            encoder->Decode(answerData);
        }
    }
    return true;
}
//------------------------------------------------------------------------------
bool tableRS485::setSpeed(float speed)
{
    AnswerAngleDrive ansMsg;
    unsigned char bytespacket[9];
    double fixdata=0;
    fixdata=a_math::valfixd(0x55d50f00,54613.33);
    QByteArray data;
    Int32Type val;
    val.iVal=static_cast<int32_t>((speed*54613.33));//град/сек  54613.33==1deg/sec
    data.append(val.buf,4); //todo
    QByteArray sendData=GetMessage(Command::SPEED_MODE,data,255,0);
    QByteArray answerData=ComPort->writeAndRead(sendData);
    if(answerData.endsWith(ENDBYTE))
    {
        if(answerData.size()==2){
            ErrorAnalyze(static_cast<unsigned char>(answerData[0]),"Вращение: ");
            return false;
        }
        else{
            encoder->Decode(answerData);
            QDataStream stream(&answerData, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream>>ansMsg.ResCod>>ansMsg.TagPoint>>ansMsg.Data>>ansMsg.CRC>>ansMsg.STOP;
            memcpy(&bytespacket,answerData,9);
            if(ansMsg.CRC==a_math::calcCRC16(&bytespacket[0],6))
            {
                qDebug("setAngle: CRC is true");
                return true;
            }
            else qDebug("setAngle: CRC is false");
        }
    }
    return false;
}
//------------------------------------------------------------------------------
bool tableRS485::setAngle(float angle)
{
    AnswerAngleDrive ansMsg;
    QByteArray data;
    Int32Type val;
    val.iVal=static_cast<int32_t>((angle*2500));
    data.append(val.buf,3);


    QByteArray sendData=GetMessage(Command::ANGLE_MODE,data,255,0);
    QByteArray answerData=ComPort->writeAndRead(sendData);
    if(answerData.endsWith(ENDBYTE))
    {
        if(answerData.size()==2){
            ErrorAnalyze(static_cast<unsigned char>(answerData[0]),"Поворот :");
            return false;
        }
        else{
            encoder->Decode(answerData);
            QDataStream stream(&answerData, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream>>ansMsg.ResCod>>ansMsg.TagPoint>>ansMsg.Data
                >>ansMsg.CRC>>ansMsg.STOP;
            if(ansMsg.CRC==a_math::calcCRC16((unsigned char*)answerData.data(),6))
            {
                qDebug("setAngle: CRC is true");
                return true;
            }
            else qDebug("setAngle: CRC is false");
        }
    }
    return false;
}
//------------------------------------------------------------------------------
void tableRS485::RequestPos()
{
    AnswerRequestPos PosMsg;
    //    unsigned char bytepacket[8];
    QByteArray data;
    QByteArray sendData=GetMessage(Command::CURR_POS,data,254,0);
    QByteArray answerData=ComPort->writeAndRead(sendData);
    if(answerData.endsWith(ENDBYTE)){
        if(answerData.size()==2){
            ErrorAnalyze(static_cast<unsigned char>(answerData[0]),"Запрос позиции: ");
        }
        else{
            encoder->Decode(answerData);
            QDataStream stream(&answerData,QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream>>PosMsg.Point>>PosMsg.Data>>PosMsg.CRC>>PosMsg.STOP;
            //            memcpy(&bytepacket,answerData,8);
            //            if(PosMsg.CRC==a_math::calcCRC16(&bytepacket[0],5)){
            if(PosMsg.CRC==a_math::calcCRC16((unsigned char*)answerData.data(),5)){
                currentPos=static_cast<double>(PosMsg.Data)/2500.;
            }
        }
    }
}

void tableRS485::RequestCurrentSpeed()
{
    AnswerRequestSpeed SpeedMsg;
    QByteArray data;
    QByteArray sendData=GetMessage(Command::CURR_VEL,data,254,DEV);
    QByteArray answerData=ComPort->writeAndRead((sendData));
    if(answerData.endsWith(ENDBYTE))
    {
        if(answerData.size()==2){
            ErrorAnalyze(static_cast<unsigned char>(answerData[0]),"Текущая скорость:");
        }
        else{
            encoder->Decode(answerData);
            QDataStream stream(&answerData, QIODevice::ReadOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream>>SpeedMsg.Point>>SpeedMsg.Data>>SpeedMsg.CRC>>SpeedMsg.STOP;
            if(SpeedMsg.CRC=a_math::calcCRC16((unsigned char*)answerData.data(),3))
            {
                currentSpeed=static_cast<double>(SpeedMsg.Data)/50;
            }
        }
    }
}
//------------------------------------------------------------------------------
void tableRS485::ErrorAnalyze( unsigned char errbyte,QString func)
{
    if(errbyte == RES_OUT )
        emit signalErrorMsg(func+ tr("Заданное значение вне рабочей зоны"));
    else if(errbyte == RES_HRD)
        emit signalErrorMsg(func+ tr("Команду невозможно выполнить"));
    else if(errbyte == RES_OK)
        emit signalErrorMsg(func +tr("Команда выполнена успешно"));
    else if(errbyte == RES_CS)
        emit signalErrorMsg(func +tr("Ошибка контрольной суммы"));
    else if(errbyte == RES_COM)
        emit signalErrorMsg(func +tr("Недопустимый код команды или номер параметра"));
    else if(errbyte == RES_PAR)
        emit signalErrorMsg(func +tr("Неверные значения данных"));
}

double tableRS485::getCurrentSpeed() const
{
    return currentSpeed;
}
//------------------------------------------------------------------------------
double tableRS485::getCurrentPos() const
{
    return currentPos;
}
//------------------------------------------------------------------------------
MarkerEncoder::MarkerEncoder(QObject *parent)
{
    Q_UNUSED(parent);
}
//------------------------------------------------------------------------------
void MarkerEncoder::Encode(QByteArray &data)
{
    int prevIndex=-1;

    for(int i=0;i<data.size();++i)
    {
        char b=data.at(i);
        if(b==0x65 || b==0x5a){
            if(prevIndex == -1){
                char resbyte=static_cast<char>(i);
                data[0]=data[0] | (resbyte&0xF);
                prevIndex=i;
                continue;
            }
            int delta=i - prevIndex;
            data[prevIndex]=EncodeByte(data[prevIndex],delta);
            prevIndex=i;
        }
    }
    if(prevIndex>0){
        data[prevIndex]=EncodeByte(data[prevIndex],0);
    }
}
//------------------------------------------------------------------------------
char MarkerEncoder::EncodeByte(char byte, int delta)
{
    if(byte==0x65)
        return (0x80|static_cast<char>(delta));
    return static_cast<char>(delta);
}
//------------------------------------------------------------------------------
void MarkerEncoder::Decode(QByteArray &data)
{
    char addressByte = data[0];
    char address= addressByte&0x3F;
    while(address>0){
        char codeByte=data[address];
        if(codeByte&0x80) data[address]=0x65;
        else data[address]=0x5A;
        address=codeByte&0x7F;
    }
    data[0]=addressByte & 0xF0;
}


//------------------------------------------------------------------------------
// Расчет контрольной суммы
//------------------------------------------------------------------------------
const unsigned short CRC16Table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

//---------------------------------------------------------------------------
unsigned short a_math::calcCRC16(unsigned char *pcBlock, unsigned short len)
{
    unsigned short crc=0xFFFF;
    while(len--){
        crc=(crc>>8)^CRC16Table[(crc & 0xFF) ^ *pcBlock++];
    }
    return crc;
}
//---------------------------------------------------------------------------
double a_math::valfixd(const long data, const double scale)
{
    return scale*(data/2147483648.);
}
//---------------------------------------------------------------------------
long a_math::fixd(const double value, const double scale)
{
    double r=value/scale; if(r>.999999999534339) r=.999999999534339;
    if(r<-1.) r=-1.;
    return long(r*2147483648.);
}
//---------------------------------------------------------------------------
float a_math::valfixw(const int data, const float scale)
{
    return scale*(data/32768.);
}
//---------------------------------------------------------------------------
int a_math::fixw(const float value, const float scale)
{
    float r=value/scale; if(r>.9999695) r=.9999695;
    if(r<-1.) r=-1.;
    return int(r*32768.);
}
