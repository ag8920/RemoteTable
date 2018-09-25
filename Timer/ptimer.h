#ifndef PTIMER_H
#define PTIMER_H

#include <QObject>
#include <QThread>
#include <windows.h>

class PTimer : public QThread
{
    Q_OBJECT
public:
    explicit PTimer();

protected:
    void run();
signals:
        void timeout();
public slots:
        void setInterval(double sec);
        void stop();

private:
        double interval;
        LARGE_INTEGER currTick;
        LARGE_INTEGER prevTick;
        LARGE_INTEGER freqTick;

        double dcurrTick;
        double dprevTick;
        double freq;

        volatile bool stopped;

};

#endif // PTIMER_H
