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
private:
    char SlipDecode(QByteArray b, QByteArray &b2);
};

#endif // SLIPPROTOCOL_H
