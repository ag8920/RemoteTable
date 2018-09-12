#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QIntValidator>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QWidget
{
    Q_OBJECT

public:
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
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Settings settings()const;
signals:
    void isUpdateSettings();
private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void fillPortsInfo();
private:
    void fillPortsParameters();    
    void updateSettings();

private:
    Ui::SettingsDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
};

#endif // SETTINGSDIALOG_H
