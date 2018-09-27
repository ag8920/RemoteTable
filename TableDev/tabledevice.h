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
#include <QThread>

#include "../comport/comport.h"
#include "../comport/settingsdialog/settingsdialog.h"
#include "../Console/console.h"

class TableDevice : public QMainWindow
{
    Q_OBJECT
public:
    explicit TableDevice(QWidget *parent = nullptr);
    ~TableDevice();
signals:
    void ConnectComPort(QString name,int baudrate,
                        int DataBits,int Parity,
                        int StopBits,int FlowControl);
    void DisconnectComPort();
    void StopAll();
    void OutputToComPort(const QByteArray &data);
    void ConsoleSetFormat(const int Format);

    void StopRotation();
    void StartRotation();
    
    void SendNumPosition(int pos);
    void SendNumMeasure(int num);
public slots:
    void OpenSerialPort();
    void isConnectedComPort(const QString msg);
    void isNotConnectedComPort(const QString msg);
    void CloseSerialPort();
    void UpdateSettingsComPort();

    void PositioningParamVisible();
    void ExecutePosition();
    void ZeroPostion();
    void RequestPosition();


    void GoToPosition(QVariant position);

    void OnMotion();
    void BeginMotion();
    void StopMotion();
    void OffMotion();
    void FinishedMotion();
    void ResetAbsCoord();

    void ManualMode();

    void SettingsRotation();
    void SetFormatConsole();

    void SetTimer();

    void GetPosition(QByteArray data);

    void StartMeasure();
    void StopMeasure();

    void ConsoleVisible();
private:

    void CreateWidgets();
    void CreateConnections();
    void AddThreads();
    void StopThread();


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
    QPushButton *ResetAbsolutCoordButton;

    QPushButton *ClearConsoleButton;

    QCheckBox *PositveRotationCheckBox;
    QCheckBox *NegativeRotationCheckBox;
    QCheckBox *AsciiFormatCheckBox;
    QCheckBox *ConsoleVisibleCheckBox;
    QCheckBox *RequestPostionCheckBox;

    QPushButton *PositioningButton;
    QPushButton *ExecutePositioningButton;
    QPushButton *ZeroPositionButton;
    QLineEdit *PositioningLineEdit;
    QLabel *PositioningLabel;
    QCheckBox *AbsolutPositioningCheckBox;
    QCheckBox *RelativePositioningCheckBox;
    QGroupBox *PositioningBox;

    QPushButton *SettingsPortButton;
    QPushButton *OnComPortButton;
    QPushButton *OffComPortButton;

    QPushButton *SendCommandButton;
    QLineEdit *SendCommandLineEdit;

    SettingsDialog *SettingsComPort;
    comPort *DeviceComPort;
    QThread *ComPortThread;

    Console *ConsoleWidget;

    QMenu *PositionCommand;

    QTimer *tmr;

public:
    unsigned char updateSettingsPort;
    bool isMeasuring;//признак измерения
    bool isPosition;
    bool isRotation; //признак движения
    int currPosition;
    int prevPosition;
    int nextPosition;
    int velocity;
    int numPosition;
    int numMeasure;


};

#endif // TABLEDEVICE_H
