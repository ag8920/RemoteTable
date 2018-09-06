#ifndef TABLEDEVICE_H
#define TABLEDEVICE_H

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

class TableDevice : public QWidget
{
    Q_OBJECT
public:
    explicit TableDevice(QWidget *parent = nullptr);

private:

    void CreateWidgets();

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


};

#endif // TABLEDEVICE_H
