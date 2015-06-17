#include "AutoThread.h"
#include "AutoControl.h"
#include "QTime"
#include "MavState.h"
#include "global.h"
#include"utils.h"

AutoThread::AutoThread(QObject *parent) :
    QThread(parent)
{

}

void AutoThread::run(){
    QTime t;
    QTime rate;
    int r = 10; //Hz
    int milli;
    qDebug() << "starting land in 5 seconds";
    sleep(5);
    t.start();
    rate.start();

    while (t.elapsed() < 50000) {

        //takeoff


        //land
/*
        if(){
            land(0.3,(float)1/r,true);
        }
*/

        msleep(1000/r - (float)rate.elapsed());
        rate.restart();

    }

}

int AutoThread::land(float speed, float dt, bool wasLanding){

    //landing procedure
    if(!wasLanding){

        MavState comm = g::state;
        autoCommand.push_back(comm);
        publish();

    }

    else{

        MavState comm = g::setPoint;
        float z = comm.z();
        if(z > -0.30){

            z = 1;

        }
        else{
            z += speed * dt;
        }

        comm.setZ(z);
        manualCommand.push_back(comm);
        publish();


    }

}

void AutoThread::takeOff(){

}
