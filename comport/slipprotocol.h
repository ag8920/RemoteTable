#ifndef SLIPPROTOCOL_H
#define SLIPPROTOCOL_H

#include <QObject>

class SlipProtocol : public QObject
{
    Q_OBJECT
public:
    explicit SlipProtocol(QObject *parent = nullptr);

signals:
    void outDecodeArray(QByteArray);
public slots:
    void GetSlipByteArray(QByteArray slipInputData);
public:
    char SlipDecode(QByteArray b, QByteArray &b2);
    char SlipDecode(QByteArray b, QByteArray &b2, int l, int cnt);
};

#endif // SLIPPROTOCOL_H
