#include "rgbled.h"
#include <QDebug>

RgbLed::RgbLed(QWidget *parent)
    : QWidget(parent),
      ledColor(Qt::green),
      lightColor(QColor(0xE0, 0xE0,0xE0)),
      shadowColor(QColor(0x70,0x70,0x70)),
      ringShadowDarkColor(QColor(0x50, 0x50, 0x50, 0xFF)),
      ringShadowMedColor(QColor(0x50, 0x50, 0x50, 0x20)),
      ringShadowLightColor(QColor(0xEE, 0xEE, 0xEE, 0x00)),
      topReflexUpColor(QColor(0xFF, 0xFF, 0xFF, 0xA0)),
      topReflexDownColor(QColor(0xFF, 0xFF, 0xFF, 0x00)),
      bottomReflexCenterColor(QColor(0xFF, 0xFF, 0xFF, 0x00)),
      bottomReflexSideColor(QColor(0xFF, 0xFF, 0xFF, 0x70))
{

}

RgbLed::RgbLed(QWidget *parent, QColor color)
    : QWidget(parent),
      lightColor(QColor(0xE0, 0xE0,0xE0)),
      shadowColor(QColor(0x70,0x70,0x70)),
      ringShadowDarkColor(QColor(0x50, 0x50, 0x50, 0xFF)),
      ringShadowMedColor(QColor(0x50, 0x50, 0x50, 0x20)),
      ringShadowLightColor(QColor(0xEE, 0xEE, 0xEE, 0x00)),
      topReflexUpColor(QColor(0xFF, 0xFF, 0xFF, 0xA0)),
      topReflexDownColor(QColor(0xFF, 0xFF, 0xFF, 0x00)),
      bottomReflexCenterColor(QColor(0xFF, 0xFF, 0xFF, 0x00)),
      bottomReflexSideColor(QColor(0xFF, 0xFF, 0xFF, 0x70))
{
    ledColor=color;
}

RgbLed::~RgbLed()
{

}

void RgbLed::setColor(QColor color)
{
    ledColor=color;
    this->repaint();

}

void RgbLed::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    this->height=this->size().height();
    this->width=this->size().width();
    this->minDim=(height>width) ? width : height;
    this->half=minDim/2;
    this->centerX=width/2;
    this->centerY=height/2;

    this->outerBorderWidth=minDim/10;
    this->innerBorderWidth=minDim/14;
    this->outerBorderRadius=half-outerBorderWidth;
    this->innerBorderRadius=half-(outerBorderWidth+innerBorderWidth);

    this->topReflexY=centerY-(half-outerBorderWidth-innerBorderWidth)/2;
    this->bottomReflexY=centerY + (half-outerBorderWidth-innerBorderWidth)/2;
    this->topReflexHeight=half/5;
    this->topReflexWidth=half/3;
    this->bottomReflexHeight=half/5;
    this->bottomReflexWidth=half/3;

    drawingRect.setTop((height-minDim)/2);
    drawingRect.setLeft((width-minDim)/2);
    drawingRect.setHeight(minDim);
    drawingRect.setWidth(minDim);

}

void RgbLed::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    this->drawLed(ledColor);
}

void RgbLed::drawLed(const QColor &color)
{
    QPainter p(this);
    QPen pen;
    pen.setStyle(Qt::NoPen);
    p.setPen(pen);

    //внешнее кольцо
    QRadialGradient outerRingGradient(QPoint(centerX,
                                             centerY - outerBorderRadius - (outerBorderWidth / 2)),
                                      minDim - (outerBorderWidth / 2));
    outerRingGradient.setColorAt(0, lightColor);
    outerRingGradient.setColorAt(1, shadowColor);
    QBrush outerRingBrush(outerRingGradient);
    p.setBrush(outerRingBrush);
    p.drawEllipse(this->drawingRect);


    //внутреннее кольцо
    QRadialGradient innerRingGradient(QPoint(centerX,
                                             centerY + innerBorderRadius + (innerBorderWidth / 2)),
                                      minDim - (innerBorderWidth / 2));
    innerRingGradient.setColorAt(0, lightColor);
    innerRingGradient.setColorAt(1, shadowColor);
    QBrush innerRingBrush(innerRingGradient);
    p.setBrush(innerRingBrush);
    p.drawEllipse(QPoint(centerX, centerY),
                  outerBorderRadius, outerBorderRadius);

    //заполнение "стекла"
    QColor dark(color.darker(120));
    QRadialGradient glassGradient(QPoint(centerX, centerY),
                                  innerBorderRadius);
    glassGradient.setColorAt(0, color);
    glassGradient.setColorAt(1, dark);
    QBrush glassBrush(glassGradient);
    p.setBrush(glassBrush);
    p.drawEllipse(QPoint(centerX, centerY),
                  innerBorderRadius,
                  innerBorderRadius);
    //кольцевая тень вокруг стекла(для реалистичности)
    QRadialGradient shadowGradient(QPoint(centerX, centerY),
                                   innerBorderRadius);
    shadowGradient.setColorAt(0, ringShadowLightColor);
    shadowGradient.setColorAt(0.85, ringShadowMedColor);
    shadowGradient.setColorAt(1, ringShadowDarkColor);
    QBrush shadowBrush(shadowGradient);
    p.setBrush(shadowBrush);
    p.drawEllipse(QPoint(centerX, centerY),
                  innerBorderRadius,
                  innerBorderRadius);

    //добавление бликов
    QLinearGradient topTeflexGradient(QPoint(centerX,
                                             (innerBorderWidth + outerBorderWidth)),
                                      QPoint(centerX, centerY));
    topTeflexGradient.setColorAt(0, topReflexUpColor);
    topTeflexGradient.setColorAt(1, topReflexDownColor);
    QBrush topReflexbrush(topTeflexGradient);
    p.setBrush(topReflexbrush);
    p.drawEllipse(QPoint(centerX, topReflexY), topReflexWidth, topReflexHeight);

    QRadialGradient bottomReflexGradient(QPoint(centerX,
                                                bottomReflexY + (bottomReflexHeight / 2)),
                                         bottomReflexWidth);
    bottomReflexGradient.setColorAt(0, bottomReflexSideColor);
    bottomReflexGradient.setColorAt(1, bottomReflexCenterColor);
    QBrush bottomReflexBrush(bottomReflexGradient);
    p.setBrush(bottomReflexBrush);
    p.drawEllipse(QPoint(centerX, bottomReflexY),
                  bottomReflexWidth,
                  bottomReflexHeight);
}
