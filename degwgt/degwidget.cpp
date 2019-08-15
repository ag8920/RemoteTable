#include "degwidget.h"
#include "ui_degwidget.h"
#include <QDoubleValidator>
#include <QDebug>
#include <math.h>

degWidget::degWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::degWidget),
    degrees(0.)
{
    ui->setupUi(this);
    QIntValidator *degvalidator=new QIntValidator(0,360,this);
    QIntValidator *minvalidator=new QIntValidator(0,59,this);
    QDoubleValidator *secvalidator=new QDoubleValidator(0,59,2,this);
    secvalidator->setLocale(QLocale::English);
    ui->lineEditDeg->setValidator(degvalidator);
    ui->lineEditMin->setValidator(minvalidator);
    ui->lineEditSec->setValidator(secvalidator);

    connect(ui->lineEditDeg, &QLineEdit::textChanged,
            this, &degWidget::changedDeg);
    connect(ui->lineEditMin, &QLineEdit::textChanged,
            this, &degWidget::changedDeg);
    connect(ui->lineEditSec, &QLineEdit::textChanged,
            this, &degWidget::changedDeg);
}

degWidget::~degWidget()
{
    delete ui;
}

void degWidget::setDeg(double degrees)
{
    double intpart;
    double fractpart;
    int deg=0,min=0;
    double sec=0.;
    fractpart=modf(degrees,&intpart);
    deg=static_cast<int>(intpart);
    fractpart*=60.;
    fractpart=modf(fractpart,&intpart);
    min=static_cast<int>(intpart);
    sec=fractpart*60;
    ui->lineEditDeg->setText(QString::number(deg));
    ui->lineEditMin->setText(QString::number(min));
    ui->lineEditSec->setText(QString::number(sec,'g',6));
}

void degWidget::changedDeg()
{
    bool ok=false;
    int value=0; double dvalue=0.;
    value=ui->lineEditDeg->text().toInt(&ok);
    if(ok) deg=value;else deg=0;
    value=ui->lineEditMin->text().toInt(&ok);
    if(ok) min=value; else min=0;
    dvalue=ui->lineEditSec->text().toDouble(&ok);
    if(ok) sec=dvalue; else sec=0;
    degrees=static_cast<double>(deg) +(static_cast<double>(min)+sec/60.)/60.;
}

double degWidget::getDegrees() const
{
    return degrees;
}
