#include <QByteArray>
#include "slipprotocol.h"

const char S_END = static_cast<char>(0xC0); // end of packet
const char S_ESC = static_cast<char>(0xdb); // byte stuffing
const char S_ESC_END = static_cast<char>(0xDC); // ESC ESC_END means END data
const char S_ESC_ESC = static_cast<char>(0xDD); // ESC ESC_ESC means ESC data

//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
SlipProtocol::SlipProtocol(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void SlipProtocol::GetSlipByteArray(QByteArray slipInputData)
{
    QByteArray decodeByteArray;
    char r=SlipDecode(slipInputData,decodeByteArray);
    if(r==1)
        emit outDecodeArray(decodeByteArray);
}
//-----------------------------------------------------------
// Назначение:
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
