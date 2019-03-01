#include "corrddialog.h"
#include "ui_corrddialog.h"
#include <QSettings>
corrdDialog::corrdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::corrdDialog)
{
    ui->setupUi(this);
    QDoubleValidator *validator=new QDoubleValidator;
//    this->Lat=55.566111;
//    this->Lon=38.245833;
//    this->H=131.2;
    this->readSettings();
    validator->setLocale(QLocale::English);
    ui->LatlineEdit->setValidator(validator);
    ui->LatlineEdit->setText(QString::number(static_cast<double>(Lat)));
    ui->LonlineEdit->setValidator(validator);
    ui->LonlineEdit->setText(QString::number(static_cast<double>(Lon)));
    ui->HeightlineEdit->setValidator(validator);
    ui->HeightlineEdit->setText(QString::number(static_cast<double>(H)));

    ui->buttonBox->addButton(tr("Ок"), QDialogButtonBox :: AcceptRole);
    ui->buttonBox->addButton(tr("Отмена"),QDialogButtonBox::RejectRole);

}

corrdDialog::~corrdDialog()
{
    this->writeSettings();
    delete ui;
}

void corrdDialog::accept()
{
    updateCoordinate();
    emit outCoordinate(&Lat,&Lon,&H);
    this->writeSettings();
    this->hide();
}

void corrdDialog::updateCoordinate()
{
    this->Lat=ui->LatlineEdit->text().toDouble();
    this->Lon=ui->LonlineEdit->text().toDouble();
    this->H=ui->HeightlineEdit->text().toDouble();
}


void corrdDialog::readSettings()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("Coordinate");
    Lat=settings.value("Lat","55.566111").toDouble();
    QString str=QString::number(this->Lat,'g',12);
//    ui->LatlineEdit->setText(str);
    ui->LatlineEdit->insert(str);
    ui->LonlineEdit->setText(settings.value("Lon","38.245833").toString());
    ui->HeightlineEdit->setText(settings.value("H","132.2").toString());
    updateCoordinate();
    emit outCoordinate(&Lat,&Lon,&H);

}

void corrdDialog::writeSettings()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("Coordinate");
    settings.setValue("Lat",QString("%1").arg(this->Lat));
    settings.setValue("Lon",QString("%1").arg(this->Lon));
    settings.setValue("H",QString("%1").arg(this->H));

}
