#include "ManualThread.h"
#include "unistd.h"
#include <QDebug>
#include "MavState.h"
#include <QTime>
#include <cmath>
#include "global.h"
#include "utils.h"

#define PI 3.141592653589

ManualThread::ManualThread(QObject *parent) :
    QThread(parent)
{
    m_stop = false;
}



double omega = 0.4;
QTime t;
QTime rate;
int r = 10; //Hz
int milli;
float a = 0.8;
float b = 2;
float sp_thre = 0.2;
double x_sp,y_sp,yaw_sp;
double c[2] = {0.2,0};


void ManualThread::run(){


    g::setPoint.setPosition(c[0], c[1], -1);
    sleep(3);



    t.start();
    rate.start();


    while (!m_stop && t.elapsed() < 80000) {

        rate.restart();
        MavState comm = g::setPoint;
        double yaw_comm;
        // Lemniscate trajectory
        milli = t.elapsed();

        float sin_ = sin(PI/2 + omega * milli / 1000);
        float cos_ = cos(PI/2 + omega * milli / 1000);

        float factor = cos_ / (pow(sin_, 2) + 1);

        x_sp = c[0] + (a * factor);
        y_sp = c[1] + (b * sin_ * factor);
/*
        if(x_sp > 0) {
            yaw_sp = atan2( - g::state.y(),a/2 - g::state.x());
        }
        else if (x_sp <= 0 ){
            yaw_sp = atan2( - g::state.y(),-a/2 - g::state.x());
        }
*/
        yaw_sp = atan2( c[1] - g::state.y(),c[0] - g::state.x());
        calculateYawIntem(yaw_sp,g::state.getYaw(),yaw_comm);

        g::setPoint.setX(x_sp);
        g::setPoint.setY(y_sp);
        g::setPoint.setYaw(yaw_comm);

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


