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
#include "../GyroDev/gyrodata.h"
#include "../ModelData/tablemodel.h"
#include "../loger/loger.h"
#include "../ledlamp/rgbled.h"

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
    void BeginRecord();
    void StopRecord();
public slots:
    void OpenSerialPort();
    void isConnectedComPort(const QString &msg);
    void isNotConnectedComPort(const QString &msg);
    void UpdateSettingsComPort();
    void UpdateCountPacketLineEdit(const QString &packet, const QString &error);
    void AdditionalParamsVisible();
    void ConsoleVisible();
private slots:
    void SaveData();
    void StatusUpdate();
private:
    void CreateTable();
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

    QPushButton *SettingsPortButton;
    QPushButton *ComPortButton;
    QPushButton *ClearConsoleButton;

    QPushButton *AdditionalParamButton;

    QCheckBox *ConsoleVisibleCheckBox;

    SettingsDialog *SettingsComPort;
    comPort *DeviceComPort;
    QThread *ComPortThread;

    Console *ConsoleWidget;

    TableModel *m_model;
    MyDelegate *m_delegate;
    QTableView *m_tableView;
    QWidget *tableWidget;

    QTimer *tmr;
    RgbLed *validDataLamp;
    RgbLed *searchZeroLamp;
    RgbLed *validZeroLamp;

    unsigned char updateSettingsPort;

    loger *log;
    QThread *logThread;
    QPushButton *SaveButton;

    bool planC; //напористая сдача

public:
    double getSummDa();
    double getMeanDvX();
    double getMeanDvY();
    GyroData *Measure;
    QThread *MeasureThread;
    bool getPlanC() const;
    void setPlanC(bool value);
};
#endif // GYRODEVICE_H
