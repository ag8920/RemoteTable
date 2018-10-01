/*!
     @brief Класс прецезионного таймера на основе тиков процессора
     @author Щербаков Александр
     @date 13.09.2018
     @todo работоспособен исключительно в windows
*/
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
        /*!
         * \brief timeout сигнал запуска таймера
         */
        void timeout();
public slots:
        /*!
         * \brief setInterval устанавливает интервал срабатывания таймера
         * \param[in] sec время срабатывания в секундах
         */
        void setInterval(double sec);
        /*!
         * \brief stop останавливает таймер
         */
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
