#include "plotwidget.h"
#include <QGridLayout>
PlotWidget::PlotWidget(QWidget *parent) : QCustomPlot(parent)
{
    indexPlot=0;
    this->axisRect()->setupFullAxesBox();
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
//    this->axisRect()->setRangeDrag(Qt::Horizontal);
    this->axisRect()->setupFullAxesBox();
    this->rescaleAxes();
    resize(365,220);
    QSharedPointer<QCPAxisTickerTime>timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    this->xAxis->setTicker(timeTicker);
    this->yAxis->setLabel(tr("Значение"));
    this->xAxis->setLabel(tr("Время"));
    this->legend->setVisible(true);
    this->yAxis->setRange(-180,180);


}

void PlotWidget::closeEvent(QCloseEvent *event)
{

}

void PlotWidget::graphAdd(const QString Name, const QColor color, const int width)
{
    QPen Pen;
    Pen.setWidth(width);
    Pen.setColor(color);
    this->addGraph();
    this->graph(indexPlot)->setName(Name);
    this->graph(indexPlot)->setPen(Pen);
    this->graph(indexPlot)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));

    indexPlot++;
}

void PlotWidget::realtimeDataSlot(const int index, const double data)
{

    static QTime time(QTime::currentTime());
    // calculate two new data points:
    if(index<=indexPlot){
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
        static double lastPointKey = 0;
        if(1)// (key-lastPointKey > 0.002) // at most add point every 2 ms
        {
          // add data to lines:
         this->graph(index)->addData(key,data);

          // rescale value (vertical) axis to fit the current data:
          //this->graph(index)->rescaleValueAxis();
          lastPointKey = key;
        }
        // make key axis range scroll with the data (at a constant range size of 8):

        //this->xAxis->setRange(key, 8, Qt::AlignRight);
        //this->yAxis->setRange(key, 8, Qt::AlignRight);
        this->replot();
    }
}
