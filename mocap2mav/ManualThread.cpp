#include "ManualThread.h"


#include<QDebug>
#include "MavState.h"
#include<QTime>
#include <cmath>
#include "global.h"
#define PI 3.141592653589

ManualThread::ManualThread(QObject *parent) :
    QThread(parent)
{
    m_stop = false;
}



double omega = 0.4;
QTime t;
QTime rate;
int r = 1; //Hz
int milli;
double x_sp,y_sp,yaw;
void ManualThread::run(){

    g::setPoint.setPosition(0.6, 0 , -1);
    sleep(4);
    g::setPoint.setYaw(PI/2);
    sleep(2);
    g::setPoint.setYaw(PI);
    sleep(4);
    t.start();
    rate.start();
    while (!m_stop && t.elapsed() < 50000) {


        // Circular trajectory
        milli = t.elapsed();
        x_sp = 0.6*cos(omega * milli / 1000);
        y_sp = 0.6*sin(omega * milli / 1000);


        yaw = atan2(-g::state.y(),-g::state.x());

        g::setPoint.setYaw(yaw);

        g::setPoint.setPosition(x_sp, y_sp , -1);

        qDebug() << "set point: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();
        qDebug() << "yaw: " << yaw;


        sleep(1/r - rate.elapsed());
        rate.restart();

    }

}

void ManualThread::startMe(){

    qDebug() << "starting thread";
    m_stop = false;
    this->start();

}

void ManualThread::stopMe(){

    qDebug() << "killing thread";
    m_stop = true;
    this->quit();

}

