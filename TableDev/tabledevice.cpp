#include "tabledevice.h"


TableDevice::TableDevice(QWidget *parent) : QWidget(parent)
{
    CreateWidgets();
}

void TableDevice::CreateWidgets()
{
     QRegExp regExp("[1-9][0-9]{0,4}");

    QGroupBox *TypeTableGroupBox=new QGroupBox(tr("Тип поворотного утройства"));
    TypeTableComboBox=new QComboBox;
    TypeTableComboBox->addItem(QStringLiteral("Поворотный стол Б"));
    TypeTableLabel = new QLabel(tr("Тип поворотного устройства:"));
    TypeTableLabel->setBuddy(TypeTableGroupBox);

    RateOfTurnLineEdit=new QLineEdit;
    RateOfTurnLineEdit->setValidator(new QRegExpValidator(regExp,this));
    RateOfTurnLabel = new QLabel(tr("Скорость вращения (меток/сек)"));

    CurrPositionLineEdit=new QLineEdit;
    CurrPositionLineEdit->setReadOnly(true);

    CurrPositionLabel=new QLabel("Текущая позиция(метки):");
    CurrPositionLabel->setBuddy(CurrPositionLineEdit);

    OnMotionButton=new QPushButton(tr("Включить привод"));
    OffMotionButton=new QPushButton(tr("Отключить привод"));
    startButton=new QPushButton(tr("Начать вращение"));
    stopButton=new QPushButton(tr("Остановить вращение"));
    SettingsPortButton=new QPushButton(tr("Настройка Com-порта"));
    OnComPortButton=new QPushButton(tr("Подключить"));
    OffComPortButton=new QPushButton(tr("Отключить"));


    PositveRotationCheckBox=new QCheckBox(tr("Положительное направление"));
    NegativeRotationCheckBox=new QCheckBox(tr("Отрицательное направление"));


    QGridLayout *MainLayout=new QGridLayout;
    QGridLayout *SettingsTableLayout=new QGridLayout;

    QGroupBox *TableSettingsBox=new QGroupBox(tr("Настройка поворотного устройства"));


    SettingsTableLayout->addWidget(TypeTableLabel,0,0);
    SettingsTableLayout->addWidget(TypeTableComboBox,0,1);
    SettingsTableLayout->addWidget(CurrPositionLabel,1,0);
    SettingsTableLayout->addWidget(CurrPositionLineEdit,1,1);

    SettingsTableLayout->addWidget(RateOfTurnLabel,2,0);
    SettingsTableLayout->addWidget(RateOfTurnLineEdit,2,1);
    SettingsTableLayout->addWidget(PositveRotationCheckBox,3,0);
    SettingsTableLayout->addWidget(NegativeRotationCheckBox,4,0);

    TableSettingsBox->setLayout(SettingsTableLayout);

    QVBoxLayout *SettingsButtonLayout=new QVBoxLayout;
    SettingsButtonLayout->addWidget(SettingsPortButton);
    SettingsButtonLayout->addWidget(OnComPortButton);
    SettingsButtonLayout->addWidget(OffComPortButton);
    SettingsButtonLayout->addStretch();
    SettingsButtonLayout->addWidget(OnMotionButton);
    SettingsButtonLayout->addWidget(startButton);
    SettingsButtonLayout->addWidget(stopButton);
    SettingsButtonLayout->addWidget(OffMotionButton);



    QGroupBox *SendCommandBox=new QGroupBox(tr("Отправка команд (ручной режим)"));
    QGridLayout *SendCommandLayout=new QGridLayout;
    SendCommandButton=new QPushButton(tr("Отправить"));
    SendCommandLineEdit=new QLineEdit;
    SendCommandLayout->addWidget(SendCommandLineEdit,0,0);
    SendCommandLayout->addWidget(SendCommandButton,0,1);
    SendCommandBox->setLayout(SendCommandLayout);

    QVBoxLayout *LeftLayout=new QVBoxLayout;
    LeftLayout->addWidget(TableSettingsBox);
    LeftLayout->addWidget(SendCommandBox);

    MainLayout->addLayout(LeftLayout,0,0);
    MainLayout->addLayout(SettingsButtonLayout,0,1);


    this->setLayout(MainLayout);

    this->setWindowTitle(tr("Настройка поворотного устройства"));


}
