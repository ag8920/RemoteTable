#include <QSettings>
#include <QDoubleValidator>
#include "teodalitdialog.h"
#include "ui_teodalitdialog.h"

teodalitDialog::teodalitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::teodalitDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Ориентация визирной трубы теодолита");
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
    }
    readSettings();
}

teodalitDialog::~teodalitDialog()
{
    delete ui;
}

void teodalitDialog::accept()
{
    int i=0;
//    std::vector<QLineEdit*> lineEdits={
//        ui->lineEditPaKl,
//        ui->lineEditPaKR,
//        ui->lineEditPvKl,
//        ui->lineEditPvKR
//    };
    for(const QLineEdit *lineEdit:lineEdits)
    {
        bool ok=false;
        double value=lineEdit->text().toDouble(&ok);
        if(ok)
        {
            PaPv[i]=value;
        }
        else PaPv[i]=0.;
        i++;
    }
    QByteArray outData={};
    QDataStream out(&outData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    out<<PaPv[0]<<PaPv[1]<<PaPv[2]<<PaPv[3];
    emit sendValue(outData);
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

    ui->lineEditPaKl->setText(QString::number(PaPv[0],'g',8));
    ui->lineEditPaKR->setText(QString::number(PaPv[1],'g',8));
    ui->lineEditPvKl->setText(QString::number(PaPv[2],'g',8));
    ui->lineEditPvKR->setText(QString::number(PaPv[3],'g',8));
}
