/*!
*    @brief Класс реализует функции SLIP-протокола
*    @author Щербаков Александр
*    @date 13.09.2018
*/
#ifndef SLIPPROTOCOL_H
#define SLIPPROTOCOL_H

#include <QObject>

class SlipProtocol : public QObject
{
    Q_OBJECT
public:
    ///<конструктор класса
    explicit SlipProtocol(QObject *parent = nullptr);

signals:
    /*!
     * \brief сигнал отправки декодированного пакета
     * \param[Out] data декодированный пакет(байтовый массив данных)
     */
    void outDecodeArray(QByteArray data);
public slots:
    /*!
     * \brief  получение и декодирование SLIP пакета
     * \param [In] slipInputData  входящий SLIP-кодированный пакет данных
     * (байтовый массив)
     */
    void GetSlipByteArray(QByteArray slipInputData);
public:
    /*!
     * \brief декодирование SLIP-протокола
     * \param[In] b кодированный пакет данных
     * \param[Out] b2 декодированный пакет данных
     * \return при успешном выполнении возвращает 1
     */
    char SlipDecode(QByteArray b, QByteArray &b2);
    /*!
     * \brief перегруженная функция
     * \param[In] b кодированный пакет
     * \param[Out] b2 декодированный пакет
     * \param[In] l длина декодированного пакета
     * \param[In] cnt количество байт в кодированном пакете
     * \return при успешном выполнении возвращает 1
     */
    char SlipDecode(QByteArray b, QByteArray &b2, int l, int cnt);

    /*!
     * \brief SlipEncode
     * \param buffer
     * \param encodebuffer
     * \param lenght
     * \return
     */
    uint16_t SlipEncode(QByteArray buffer, QByteArray &encodebuffer, uint16_t lenght);
};

#endif // SLIPPROTOCOL_H
