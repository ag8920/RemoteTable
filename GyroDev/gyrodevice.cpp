#include "gyrodevice.h"

GyroDevice::GyroDevice(QWidget *parent) : QWidget(parent)
{
    SettingsComPort = new SettingsDialog;
    CreateWidgets();
    Connections();
}

void GyroDevice::CreateWidgets()
{
    TypeProtocolComboBox=new QComboBox;
    TypeProtocolComboBox->addItem(QStringLiteral("Delta_PS"));
    TypeProtocolComboBox->addItem(QStringLiteral("Rate_2"));
    TypeProtocolComboBox->addItem(QStringLiteral("Dadvtt"));
    TypeProtocolLabel=new QLabel(tr("Тип протокола:"));
    TypeProtocolLabel->setBuddy(TypeProtocolComboBox);

    CountPacketLineEdit=new QLineEdit;
    CountPacketLineEdit->setReadOnly(true);
    CountPacketLabel=new QLabel(tr("число пакетов:"));
    CountPacketLabel->setBuddy(CountPacketLineEdit);

    CountErrorLineEdit=new QLineEdit;
    CountErrorLineEdit->setReadOnly(true);
    CountErrorLabel=new QLabel(tr("Число ошибок:"));
    CountErrorLabel->setBuddy(CountErrorLineEdit);

    ValueLineEdit=new QLineEdit;
    ValueLineEdit->setReadOnly(true);
    ValueLabel=new QLabel(tr("Показания:"));
    ValueLabel->setBuddy(ValueLineEdit);

    SettingsPortButton=new QPushButton(tr("Настройка Com-порта"));
    OnComPortButton=new QPushButton(tr("Подключить"));
    OffComPortButton=new QPushButton(tr("Отключить"));

    AdditionalParamButton=new QPushButton(tr("Дополнительно ..."));

    QGroupBox *GyroSettingsBox=new QGroupBox(tr("Настройка гироскопического устройства"));

    QGridLayout *LeftLayout=new QGridLayout;
    LeftLayout->addWidget(TypeProtocolLabel,0,0);
    LeftLayout->addWidget(TypeProtocolComboBox,0,1);
    LeftLayout->addWidget(CountPacketLabel,1,0);
    LeftLayout->addWidget(CountPacketLineEdit,1,1);
    LeftLayout->addWidget(CountErrorLabel,2,0);
    LeftLayout->addWidget(CountErrorLineEdit,2,1);
    LeftLayout->addWidget(ValueLabel,3,0);
    LeftLayout->addWidget(ValueLineEdit,3,1);
    LeftLayout->addWidget(AdditionalParamButton,4,0);

    GyroSettingsBox->setLayout(LeftLayout);

    QVBoxLayout *RightLayout=new QVBoxLayout;
    RightLayout->addWidget(SettingsPortButton);
    RightLayout->addWidget(OnComPortButton);
    RightLayout->addWidget(OffComPortButton);
    RightLayout->addStretch();

    QGridLayout *MainLayout=new QGridLayout;
    MainLayout->addWidget(GyroSettingsBox,0,0);
    MainLayout->addLayout(RightLayout,0,1);


    this->setLayout(MainLayout);
    this->setWindowTitle(tr("Параметры гироскопического устройства"));


}

void GyroDevice::Connections()
{
    connect(SettingsPortButton,SIGNAL(pressed()),SettingsComPort,SLOT(show()));
}
