#include "ManualThread.h"
#include "unistd.h"
#include <QDebug>
#include "MavState.h"
#include <QTime>
#include <cmath>
#include "global.h"
#define PI 3.141592653589

ManualThread::ManualThread(QObject *parent) :
    QThread(parent)
{
    m_stop = false;
}



double omega = 0.3;
QTime t;
QTime rate;
int r = 5; //Hz
int milli;
float a = 0.8;
double x_sp,y_sp,yaw;
void ManualThread::run(){


    g::setPoint.setPosition(a + 0.4, 0, -1);
    sleep(3);



    t.start();
    rate.start();


    while (!m_stop && t.elapsed() < 50000) {

        rate.restart();


        // Lemniscate trajectory
        milli = t.elapsed();

        float sin_ = sin(omega * milli / 1000);
        float cos_ = cos(omega * milli / 1000);

        float factor = cos_ / (pow(sin_, 2) + 1);

        x_sp = 0.4 + a * factor;
        y_sp = a * sin_ * factor;


        //yaw = atan2(-g::state.y(),-g::state.x());

        //g::setPoint.setYaw(yaw);

        g::setPoint.setPosition(x_sp, y_sp , -1);

        qDebug() << "set point: " << g::setPoint.x() << " " << g::setPoint.y() << " t: " << sin_;


        //qDebug() << "set point: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z() <<"yaw: "<<g::setPoint.yaw();
        msleep(1000/r - (float)rate.elapsed());




    }

}

void ManualThread::startMe(){

    qDebug() << "starting thread";
    m_stop = false;
    this->start();
    begin();

}

void ManualThread::stopMe(){

    qDebug() << "killing thread";
    m_stop = true;
    this->quit();

}

