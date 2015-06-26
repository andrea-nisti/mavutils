#ifndef AUTOTHREAD_H
#define AUTOTHREAD_H

#include<QThread>
#include "utils.h"

class AutoThread : public QThread
{
    Q_OBJECT
    void run();
public:
    explicit AutoThread(QObject *parent = 0);
    void land(float speed, float dt, double vz, position p, position robot_state);
    void takeOff();
    void move(double alpha, position target, position robot_state);
    void move2(double alpha, position target, position robot_state, float dt);
    void rotate();
signals:
    void publish();

public slots:
    void startMe();


};

#endif // AUTOTHREAD_H
