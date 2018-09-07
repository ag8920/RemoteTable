#ifndef TABLEDEVICE_H
#define TABLEDEVICE_H

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

class TableDevice : public QMainWindow
{
    Q_OBJECT
public:
    explicit TableDevice(QWidget *parent = nullptr);
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
    QComboBox *TypeTableComboBox;
    QLabel *TypeTableLabel;

    QLineEdit *RateOfTurnLineEdit;
    QLabel *RateOfTurnLabel;

    QLineEdit *CurrPositionLineEdit;
    QLabel *CurrPositionLabel;

    QPushButton *OnMotionButton;
    QPushButton *OffMotionButton;
    QPushButton *startButton;
    QPushButton *stopButton;


    QCheckBox *PositveRotationCheckBox;
    QCheckBox *NegativeRotationCheckBox;

    QPushButton *SettingsPortButton;
    QPushButton *OnComPortButton;
    QPushButton *OffComPortButton;

    QPushButton *SendCommandButton;
    QLineEdit *SendCommandLineEdit;

    SettingsDialog *SettingsComPort;
    comPort *DeviceComPort;
};

#endif // TABLEDEVICE_H
