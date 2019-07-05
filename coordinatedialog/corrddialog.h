/*!
*   @file
*   @brief Класс создает виджет ручного ввода координат
*   @author Щербаков Александр
*   @version 1.0
*   @date 13.09.2018
*/
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
    ///Конструктор класса
    explicit corrdDialog(QWidget *parent = nullptr);
    ~corrdDialog();
    ///Широта
    double Lat;
    ///Долгота
    double Lon;
    ///Высота
    double H;
private:
    Ui::corrdDialog *ui;
    ///Прочитать сохраненные настройки
    void readSettings();
    ///Сохранить настройки
    void writeSettings();
signals:
    ///сигнал отправки введенных координат
    void outCoordinate(double *Lat,double *Lon,double *H);
public slots:
    ///принять введенные координаты
    void accept();
    ///Обновить координаты
    void updateCoordinate();

};

#endif // CORRDDIALOG_H
