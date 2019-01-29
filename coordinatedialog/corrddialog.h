#ifndef CORRDDIALOG_H
#define CORRDDIALOG_H

#include <QDialog>

namespace Ui {
class corrdDialog;
}

class corrdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit corrdDialog(QWidget *parent = nullptr);
    ~corrdDialog();
    ///широта
    double Lat;
    ///долгота
    double Lon;
    ///высота
    double H;
private:
    Ui::corrdDialog *ui;
signals:
    void outCoordinate(double *Lat,double *Lon,double *H);
public slots:
    void accept();
    void updateCoordinate();

};

#endif // CORRDDIALOG_H
