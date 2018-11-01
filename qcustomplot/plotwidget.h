#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QObject>
#include <QWidget>
#include "qcustomplot/qcustomplot.h"
class PlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

signals:

public slots:
    void graphAdd(const QString Name="",const QColor color=Qt::black,const int width=1);
   // void replot(const int index,double data );
    void realtimeDataSlot(const int index, const double data);
private:
    int indexPlot;
    QVector<QVector<QVariant>> vdata;
};

#endif // PLOTWIDGET_H
