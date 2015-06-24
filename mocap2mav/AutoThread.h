#ifndef AUTOTHREAD_H
#define AUTOTHREAD_H

#include<QThread>

class AutoThread : public QThread
{
    Q_OBJECT
    void run();
public:
    explicit AutoThread(QObject *parent = 0);
    void land(float speed, float dt, double vz);
    void takeOff();
    void move(double alpha);
    void rotate();
signals:
    void publish();

public slots:
    void startMe();


};

#endif // AUTOTHREAD_H
