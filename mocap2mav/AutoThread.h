#ifndef AUTOTHREAD_H
#define AUTOTHREAD_H

#include<QThread>

class AutoThread : public QThread
{
    Q_OBJECT
    void run();
public:
    explicit AutoThread(QObject *parent = 0);
    int land(float speed, float dt, bool wasLanding);
    void takeOff();
    void move(double alpha);
signals:
    void publish();

public slots:


};

#endif // AUTOTHREAD_H
