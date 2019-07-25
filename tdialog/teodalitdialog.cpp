#include <QSettings>
#include <QDoubleValidator>
#include <QDebug>
#include "teodalitdialog.h"
#include "ui_teodalitdialog.h"

teodalitDialog::teodalitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::teodalitDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Ориентация визирной трубы теодолита");
    this->setWindowIcon(QIcon(":/icons/theodolite.png"));
    this->setWindowFlags(Qt::Dialog
                         | Qt::MSWindowsFixedSizeDialogHint
                         | Qt::CustomizeWindowHint
                         | Qt::WindowCloseButtonHint);
    QLocale locale(QLocale::English);
    QDoubleValidator *validator=new QDoubleValidator;
    validator->setLocale(locale);
    lineEdits={
        ui->lineEditPaKl,
        ui->lineEditPaKR,
        ui->lineEditPvKl,
        ui->lineEditPvKR
    };
    for(QLineEdit *lineEdit:lineEdits)
    {
        lineEdit->setValidator(validator);
        lineEdit->hide();
    }

    PaKLwgt=new degWidget();
    PaKRwgt=new degWidget();
    PvKLwgt=new degWidget();
    PvKRwgt=new degWidget();

    degEdits={
        PaKLwgt,
        PaKRwgt,
        PvKLwgt,
        PvKRwgt
    };

    ui->gridLayout->addWidget(PaKLwgt,1,1);
    ui->gridLayout->addWidget(PaKRwgt,2,1);
    ui->gridLayout->addWidget(PvKLwgt,1,2);
    ui->gridLayout->addWidget(PvKRwgt,2,2);

    readSettings();
}

teodalitDialog::~teodalitDialog()
{
    delete ui;
}

void teodalitDialog::accept()
{
    int i=0;
    for(const degWidget *lineEdit:degEdits)
    {

        double value=lineEdit->getDegrees();//lineEdit->text().toDouble(&ok);
        PaPv[i]=value;
        i++;
    }
    QByteArray outData={};
    QDataStream out(&outData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    out<<PaPv[0]<<PaPv[1]<<PaPv[2]<<PaPv[3];
    //qDebug()<<PaPv[0]<<PaPv[1]<<PaPv[2]<<PaPv[3];
    emit sendValue(outData);
    writeSettings();
    hide();
    //emit accepted();

}

void teodalitDialog::reject()
{
    PaKLwgt->setDeg(PaPv[0]);
    PaKRwgt->setDeg(PaPv[1]);
    PvKLwgt->setDeg(PaPv[2]);
    PvKRwgt->setDeg(PaPv[3]);
//    emit rejected();
    hide();
}

void teodalitDialog::readSettings()
{
    int i=0;
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("Justified_teodolit");
    PaPv[0]=settings.value("PaKl").toDouble();
    PaPv[1]=settings.value("PaKR").toDouble();
    PaPv[2]=settings.value("PvKl").toDouble();
    PaPv[3]=settings.value("PvKR").toDouble();
    settings.endGroup();

    PaKLwgt->setDeg(PaPv[0]);
    PaKRwgt->setDeg(PaPv[1]);
    PvKLwgt->setDeg(PaPv[2]);
    PvKRwgt->setDeg(PaPv[3]);

//    ui->lineEditPaKl->setText(QString::number(PaPv[0],'g',8));
//    ui->lineEditPaKR->setText(QString::number(PaPv[1],'g',8));
//    ui->lineEditPvKl->setText(QString::number(PaPv[2],'g',8));
//    ui->lineEditPvKR->setText(QString::number(PaPv[3],'g',8));
}

void teodalitDialog::writeSettings()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("Justified_teodolit");
    settings.setValue("PaKl",QString::number(PaPv[0]));
    settings.setValue("PaKR",QString::number(PaPv[1]));
    settings.setValue("PvKl",QString::number(PaPv[2]));
    settings.setValue("PvKR",QString::number(PaPv[3]));
    settings.endGroup();
}

inline double teodalitDialog::convertDegrees(const double &deg, const double &min, const double &sec)
{
    return deg + (min +sec/ 60) / 60;
}
