//------------------------------------------------------------------------------
//     Данный модуль создает
//     прецезионный таймер на основе тиков процессора
//     модуль работоспособен исключительно в windows
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "ptimer.h"


PTimer::PTimer()
{
    interval=0.;
    dprevTick=0.;
    dcurrTick=0.;
    freq=1.;
    stopped=true;
}

void PTimer::run()
{
    bool init=false;
    while (!stopped) {

        if(!init){
            if(QueryPerformanceCounter(&prevTick))
                dprevTick=prevTick.QuadPart;
            if(QueryPerformanceFrequency(&freqTick))
                freq=freqTick.QuadPart;
            init=true;
        }

        if(QueryPerformanceCounter(&currTick))
        dcurrTick=currTick.QuadPart;
        if(((dcurrTick-dprevTick)/freq)>=interval)
        {
            emit timeout();
            stopped=true;
        }
    }
}

void PTimer::setInterval(double sec)
{

    interval=sec;
    stopped=false;
}

void PTimer::stop()
{
    interval=0.;
    dprevTick=0.;
    dcurrTick=0.;
    freq=1.;
    stopped=true;
}
