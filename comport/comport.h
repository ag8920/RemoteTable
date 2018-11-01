#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <comport/settingsdialog/settingsdialog.h>

///Структура основных параметров COM-порта
struct SettingsComPort{
    QString name;///< имя порта
    qint32 baudRate;///<скорость порта
    QSerialPort::DataBits dataBits;///<число бит данных
    QSerialPort::Parity parity;///< бит четности
    QSerialPort::StopBits stopBits;///<число стоп битов
    QSerialPort::FlowControl flowControl;///<управление поток передачи данных
};

/*!
*   @brief Класс для управления COM-портом
*   @author Щербаков Александр
*   @version 1.0
*   @date 13.09.2018
*/
class comPort: public QObject
{
    Q_OBJECT
public:
    explicit comPort(QObject *parent=nullptr);///<конструктор класса
    ~comPort();///<деструктор класса

    QSerialPort thisPort;///<объект класса последовательного COM-порта
    SettingsComPort SettingsPort;///<объект настроект COM-порта

signals:
    void isConnectedPort(const QString &msg); ///<сигнал успешного подключения порта
    void isNotConnectedPort(const QString &msg); ///<сигнал неудачного открытия порта

    void finishedPort(); ///<сигнал закрытия класса
    void error_(QString err); ///<сигнал ошибок порта
    /*!
     * \brief сигнал вывода полученных данных
     * \param[Out] data байтовый массив данных, полученный из COM-порта
     */
    void dataOutput(const QByteArray &data);///<

public slots:
    bool DisconnectPort(); ///<отключения COM-порта
    /*!
     * \brief подключение COM-порта
     * \param name имя порта
     * \param baudrate скорость передачи данных (БОД/сек)
     * \param DataBits число бит данных
     * \param Parity четность
     * \param StopBits число стоповых битов
     * \param FlowControl управление потоком передачи данных
     */
    void ConnectPort(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);
    /*!
     * \brief выполняется при старте класса, подключает чтение из порта по
     * приходу данных в порт
     */
    void processPort();
    /*!
     * \brief отправка данных в COM-порт
     * \param[In] data байтовый массив
     */
    void WriteToPort(const QByteArray &data);

    void ReadInPort();///<чтение данных из COM-порта

    void Stop();///<завершение работы с COM-портом,остановка потока
private slots:
    void handleError(QSerialPort::SerialPortError error);///<обработка ошибок
};

#endif // COMPORT_H
