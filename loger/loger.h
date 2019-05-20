/*!
*   @class loger
*   @brief Класс для логирования данных в бинарный файл
*   @author Щербаков Александр
*   @date  04.10.2018
*/
#ifndef LOGER_H
#define LOGER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QFile>
class loger : public QObject
{
    Q_OBJECT
public:
    explicit loger(QObject *parent = nullptr);
    ~loger();
signals:
    void finished();
public slots:
    /*!
     * \brief создает новый файл для записи
     */
    void start();
    /*!
     * \brief запись данных в бинарный файл .dat
     * \param[In] данные для записи
     */
    void write(QByteArray data);
    /*!
     * \brief закрывает файл, вызывается для завершения записи
     */
    void CloseFile();
    /*!
     * \brief запись данных в текстовый файл .log
     * \param строка для записи
     */
    void PutLog(QString Data);
    void PutLog2(QString Data, QString FileName);
private:
    QFile *file;///>объект класса QFile

};

#endif // LOGER_H
