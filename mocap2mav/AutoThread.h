#ifndef AUTOTHREAD_H
#define AUTOTHREAD_H

#include<QThread>
#include "utils.h"
#include "MavState.h"

class AutoThread : public QThread
{
    Q_OBJECT
    void run();
public:
    explicit AutoThread(QObject *parent = 0);
    ~AutoThread();
    void land(float speed, float dt, double vz, position p, position robot_state);
    void takeOff();
    void move(double alpha, position target, position robot_state , MavState &comm);
    void rotate();
    void trajectory(double omega, double rad, double c[2], float t, int secs, float look);
    void land_plat(MavState platform, MavState robot_state,float  = 1);
signals:
    void publish();

public slots:
    void startMe();


};

#endif // AUTOTHREAD_H
