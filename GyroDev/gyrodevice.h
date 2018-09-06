#ifndef GYRODEVICE_H
#define GYRODEVICE_H

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

#include "../comport/comport.h"
#include "../comport/settingsdialog/settingsdialog.h"

class GyroDevice : public QWidget
{
    Q_OBJECT
public:
    explicit GyroDevice(QWidget *parent = nullptr);

signals:

public slots:

private:
    void CreateWidgets();
    void Connections();

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
    comPort *GyroComPort;
};
#endif // GYRODEVICE_H
