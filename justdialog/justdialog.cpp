#include <QDoubleValidator>
#include <QSettings>
#include "justdialog.h"
#include "ui_justdialog.h"

justDialog::justDialog(QWidget *parent) :
    QDialog(parent),
    value(0),
    ui(new Ui::justDialog)
{

    ui->setupUi(this);
    this->setWindowTitle("Ввод поправок");
    this->setWindowIcon(QIcon(":icons/input2.png"));
    this->setWindowFlags(Qt::Dialog
                         | Qt::MSWindowsFixedSizeDialogHint
                         | Qt::CustomizeWindowHint
                         | Qt::WindowCloseButtonHint);
    QLocale locale(QLocale::English);
    QDoubleValidator *validator=new QDoubleValidator;
    validator->setLocale(locale);
    ui->lineEdit_00->setValidator(validator);
    ui->lineEdit_01->setValidator(validator);
    ui->lineEdit_10->setValidator(validator);
    ui->lineEdit_11->setValidator(validator);

    readSettings();
}

justDialog::~justDialog()
{
    delete ui;
}

void justDialog::accept()
{
    int i=0;
    std::vector<QLineEdit*> lineEdits={ ui->lineEdit_00,
                                        ui->lineEdit_01,
                                        ui->lineEdit_10,
                                        ui->lineEdit_11
                                       };
    std::vector<double> values;
    for(const QLineEdit *lineEdit:lineEdits)
    {
        bool ok=false;
        double value=lineEdit->text().toDouble(&ok);
        if(ok)
        {
            dvalue[i]=value;
        }
        else dvalue[i]=0.;
        i++;
    }
    QByteArray outData={};
    QDataStream out(&outData,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    out<<dvalue[0]<<dvalue[1]<<dvalue[2]<<dvalue[3];
    emit sendValue(outData);
    hide();
}

void justDialog::input(const QString &text)
{
    if(text.isEmpty())value=0;
    else value=text.toDouble();
}

void justDialog::readSettings()
{
   QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("Justified_amendments");
    dvalue[0]=settings.value("u00").toDouble();
    dvalue[1]=settings.value("u01").toDouble();
    dvalue[2]=settings.value("u10").toDouble();
    dvalue[3]=settings.value("u11").toDouble();
    settings.endGroup();
    ui->lineEdit_00->setText(QString::number(dvalue[0],'g',8));
    ui->lineEdit_01->setText(QString::number(dvalue[1],'g',8));
    ui->lineEdit_10->setText(QString::number(dvalue[2],'g',8));
    ui->lineEdit_11->setText(QString::number(dvalue[3],'g',8));
}
