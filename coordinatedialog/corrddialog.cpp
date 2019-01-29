#include "corrddialog.h"
#include "ui_corrddialog.h"

corrdDialog::corrdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::corrdDialog)
{
    ui->setupUi(this);
    QDoubleValidator *validator=new QDoubleValidator;
    this->Lat=55.566111;
    this->Lon=38.245833;
    this->H=131.2;
    validator->setLocale(QLocale::English);
    ui->LatlineEdit->setValidator(validator);
    ui->LatlineEdit->setText(QString::number(static_cast<double>(Lat)));
    ui->LonlineEdit->setValidator(validator);
    ui->LonlineEdit->setText(QString::number(static_cast<double>(Lon)));
    ui->HeightlineEdit->setValidator(validator);
    ui->HeightlineEdit->setText(QString::number(static_cast<double>(H)));
}

corrdDialog::~corrdDialog()
{
    delete ui;
}

void corrdDialog::accept()
{
    updateCoordinate();
    emit outCoordinate(&Lat,&Lon,&H);
    this->hide();
}

void corrdDialog::updateCoordinate()
{
    this->Lat=ui->LatlineEdit->text().toDouble();
    this->Lon=ui->LonlineEdit->text().toDouble();
    this->H=ui->HeightlineEdit->text().toDouble();
}
