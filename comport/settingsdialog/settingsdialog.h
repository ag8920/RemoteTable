
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QIntValidator>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class SettingsDialog;
}

/*!
    @class SettingsDialog
    @brief класс создает виджет окна
    с параметрами и информацией COM-порта.
    Класс позваляет задавать настройки порта.
    @author Щербаков Александр
    @date 13.09.2018
*/
class SettingsDialog : public QWidget
{
    Q_OBJECT

public:
    /*!
     * \brief настройки COM-порта
     * \param name имя порта
     * \param baudrate скорость передачи данных (БОД/сек)
     * \param DataBits число бит данных
     * \param Parity четность
     * \param StopBits число стоповых битов
     * \param FlowControl управление потоком передачи данных
     */
    struct Settings {
            QString name;
            int baudRate;
            QString stringBaudRate;
            QSerialPort::DataBits dataBits;
            QString stringDataBits;
            QSerialPort::Parity parity;
            QString stringParity;
            QSerialPort::StopBits stopBits;
            QString stringStopBits;
            QSerialPort::FlowControl flowControl;
            QString stringFlowControl;
            bool localEchoEnabled;
        };

    explicit SettingsDialog(QWidget *parent = nullptr,int id=0);///<конструктор класса
    ~SettingsDialog();///<деструктор класса

    Settings settings()const;///<объект настроек порта
protected:
    void closeEvent(QCloseEvent *event);
signals:
    void isUpdateSettings();///<сигнал для обновления данных
private slots:
    /*!
     * \brief отображение информации о COM-порте
     * \param idx
     */
    void showPortInfo(int idx);
    /*!
     * \brief действия по кнопке "Применить"
     */
    void apply();
    /*!
     * \brief установка не стандартных(Custom)скоростей
     * \param idx
     */
    void checkCustomBaudRatePolicy(int idx);

    void fillPortsInfo();///<считывание информации о COM-порте
public slots:
    void showWidget();///<отображение виджета настроек
private:
    void fillPortsParameters();///<отображение основных настраиваемых параметров
    void updateSettings();///<обновление(запись) текущих настроек COM-порта


    void readSettings();
    void writeSettings();
private:
    Ui::SettingsDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
    int idNum;

};

#endif // SETTINGSDIALOG_H
