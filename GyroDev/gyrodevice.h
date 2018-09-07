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

#include "../comport/comport.h"
#include "../comport/settingsdialog/settingsdialog.h"

class GyroDevice : public QMainWindow
{
    Q_OBJECT
public:
    explicit GyroDevice(QWidget *parent = nullptr);

signals:
    void ConnectComPort(SettingsDialog::Settings *p);
    void DisconnectComPort();
public slots:
    void OpenSerialPort();
    void isConnectedComPort(const QString msg);
    void isNotConnectedComPort(const QString msg);
    void CloseSerialPort();
private:
    void CreateWidgets();
    void CreateConnections();

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

    QPushButton *AdditionalParamButton;


    SettingsDialog *SettingsComPort;
    comPort *DeviceComPort;
};
#endif // GYRODEVICE_H
