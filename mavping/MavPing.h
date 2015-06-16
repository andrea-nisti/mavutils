#ifndef MAVPING_H
#define MAVPING_H

#include <QObject>
#include <QtSerialPort>
#include <QTime>
#include <stdint.h>

class MavPing : public QObject
{
    Q_OBJECT
public:
    explicit MavPing(QObject *parent = 0);

signals:
    void finished();

public slots:
    void run();

private:
    uint64_t m_startTime;
    uint64_t m_finishTime;

};

#endif // MAVPING_H
