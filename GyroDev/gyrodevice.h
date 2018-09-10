#ifndef GYRODEVICE_H
#define GYRODEVICE_H

#include <QMainWindow>
#include <QWidget>
#include <QComboBox>
#include <QtGui>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QLayout>
#include <QStatusBar>
#include <QThread>

#include "../comport/comport.h"
#include "../comport/settingsdialog/settingsdialog.h"
#include "../Console/console.h"

class GyroDevice : public QMainWindow
{
    Q_OBJECT
public:
    explicit GyroDevice(QWidget *parent = nullptr);
    ~GyroDevice();
signals:
    void ConnectComPort(QString name,int baudrate,
                        int DataBits,int Parity,
                        int StopBits,int FlowControl);
    //(SettingsDialog::Settings p);
    void DisconnectComPort();
    void StopAll();
public slots:
    void OpenSerialPort();
    void isConnectedComPort(const QString msg);
    void isNotConnectedComPort(const QString msg);
    void CloseSerialPort();
    void UpdateSettingsComPort();
private:
    void CreateWidgets();
    void CreateConnections();
    void AddThread();
    void StopThreads();

    QWidget *MainWidget;
    QLabel *TypeProtocolLabel;
    QComboBox *TypeProtocolComboBox;
    QLabel *CountErrorLabel;
    QLineEdit *CountErrorLineEdit;

    QLabel *CountPacketLabel;
    QLineEdit *CountPacketLineEdit;

    QLabel *ValueLabel;
    QLineEdit *ValueLineEdit;

    QPushButton *SettingsPortButton;
    QPushButton *OnComPortButton;
    QPushButton *OffComPortButton;
    QPushButton *ClearConsoleButton;

    QPushButton *AdditionalParamButton;


    SettingsDialog *SettingsComPort;
    comPort *DeviceComPort;
    QThread *ComPortThread;

    Console *ConsoleWidget;

    unsigned char updateSettingsPort;
};
#endif // GYRODEVICE_H
