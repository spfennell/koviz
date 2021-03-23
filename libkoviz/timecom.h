#ifndef BVISCOM_H
#define BVISCOM_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QThread>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>
#include <QApplication>
#include <stdlib.h>

class TimeCom : public QObject
{
    Q_OBJECT
public:
    explicit TimeCom(QObject *parent=0);
    ~TimeCom();

    void sendRun2Bvis(const QString &iRunDir);
    void sendTime2Bvis(double liveTime);

signals:
    void timechangedByBvis(double time);

private slots:
     void _timeComRead();

private:
    QTcpSocket *socket;
    const int BvisPort;

    int _connect2Bvis();
    void _sendMsg2Bvis(const QString& msg);
};

#endif // TIMECOM_H
