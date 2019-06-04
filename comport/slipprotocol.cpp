//------------------------------------------------------------------------------
//     Данный модуль реализует функции SLIP-протокола
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include <QByteArray>
#include "slipprotocol.h"

const char S_END = static_cast<char>(0xC0); // end of packet
const char S_ESC = static_cast<char>(0xdb); // byte stuffing
const char S_ESC_END = static_cast<char>(0xDC); // ESC ESC_END means END data
const char S_ESC_ESC = static_cast<char>(0xDD); // ESC ESC_ESC means ESC data

//-----------------------------------------------------------
// Назначение: конструктор класса
//-----------------------------------------------------------
SlipProtocol::SlipProtocol(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------
// Назначение: декодирование пакета
//-----------------------------------------------------------
void SlipProtocol::GetSlipByteArray(QByteArray slipInputData)
{
    QByteArray decodeByteArray;
    char r=SlipDecode(slipInputData,decodeByteArray);
    if(r==1)
        emit outDecodeArray(decodeByteArray);
}
//-----------------------------------------------------------
// Назначение: SLIP-декодирование пакета данных
//-----------------------------------------------------------
char SlipProtocol::SlipDecode(QByteArray b, QByteArray &b2)
{
    if(b.contains(S_END)){
        int j=0;
        while (b.at(j)!=S_END) {
            if(b.at(j)==S_ESC && b.at(j+1)!=S_ESC_END && b.at(j+1)!=S_ESC_ESC)
                return 2;
            if(b.at(j)==S_ESC && b.at(j+1)==S_ESC_END){
                b2.append(S_END);
                j++;
            }
            else if(b.at(j)==S_ESC && b.at(j+1)==S_ESC_ESC){
                b2.append(S_ESC);
                j++;
            }
            else
                b2.append(b.at(j));
            j++;
        } return 1;
    }
    else
        return  0;
}


char SlipProtocol::SlipDecode(QByteArray b, QByteArray &b2, int l, int cnt)
{
    if(cnt>=l*2-1)  return 2;   // overflow error protection
    //        if(b[cnt-1]==S_END && cnt)
    if(b.contains(S_END))
    {   int e=0, c=0; cnt=0;
        while(b[c]!=S_END)
        {   if(b[c]==S_ESC && (b[c+1]!=S_ESC_END && b[c+1]!=S_ESC_ESC)) return 2;
            if      (b[c]==S_ESC && b[c+1]==S_ESC_END)  {b2[e]=S_END; c++;}
            else if (b[c]==S_ESC && b[c+1]==S_ESC_ESC)  {b2[e]=S_ESC; c++;}
            else                                         b2[e]=b[c];
            e++; c++;
        }   return 1;
    }else   return 0;
}

uint16_t SlipProtocol::SlipEncode(QByteArray buffer, QByteArray &encodebuffer, uint16_t lenght)
{
    uint16_t c=0;
    for(int i=0;i<lenght;i++){
        switch (buffer[i]) {
        case S_END:
            encodebuffer[c]=S_ESC;
            c++;
            encodebuffer[c]=S_ESC_END;
            c++;
            break;
        case S_ESC:
            encodebuffer[c]=S_ESC;
            c++;
            encodebuffer[c]=S_ESC_ESC;
            c++;
            break;
        default:
            encodebuffer[c]=buffer[i];
            c++;
            break;
        }
    }
    encodebuffer[c]=S_END;
    c++;
    return c;
}



