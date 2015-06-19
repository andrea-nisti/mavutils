#include "AutoThread.h"
#include "QTime"
#include "MavState.h"
#include "global.h"
#include "utils.h"
#include <cmath>

AutoThread::AutoThread(QObject *parent) :
    QThread(parent)
{

}

void AutoThread::run(){

    QTime rate;
    int r = 20; //Hz

    qDebug() << "automatic from: " << QThread::currentThreadId();

    rate.start();

    while (true) {

        //takeoff
        if(executioner::take_off::take_off_sig){
                takeOff();
        }
        //land
        if(executioner::land::land_sig){
            float vel = nodeList[actualNode].a.params[0];
            land(vel,(float)1/r);
        }

        //move
        if(executioner::move::move_sig){
            move(0.5);
        }

        msleep(1000/r - (float)rate.elapsed());
        rate.restart();

    }

}

void AutoThread::land(float speed, float dt){

    //landing procedure

        MavState comm = g::setPoint;
        comm.setX(nodeList[actualNode].p.x);
        comm.setY(nodeList[actualNode].p.y);
        float z = g::state.z();
        if(z > -0.30){

            z = 1;
            executioner::land::landed = true;

        }
        else{

            z += speed * dt;
            executioner::land::landed = false;

        }

        comm.setZ(z);
        autoCommand.push_back(comm);
        publish();

}

void AutoThread::takeOff(){

    MavState comm = g::setPoint;

    comm.setX(nodeList[actualNode].p.x);
    comm.setY(nodeList[actualNode].p.y);

    double height = nodeList[actualNode].a.params[0];

    comm.setZ(height);
    autoCommand.push_back(comm);
    publish();

}


void AutoThread::move(double alpha){

    MavState comm = g::setPoint;
    //Loading parameters
    double x_node = nodeList[actualNode].p.x;
    double y_node = nodeList[actualNode].p.y;
    double z_node = nodeList[actualNode].p.z;
    double yaw = nodeList[actualNode].p.yaw;
    //Save actual state
    double x_robot = g::state.x();
    double y_robot = g::state.y();
    double z_robot = g::state.z();
    double yaw_robot = g::state.getYaw();
    //Calculate error vector
    double positionError[3] = {x_node - x_robot , y_node - y_robot , z_node - z_robot};
    double incrementVect[3];

    double dist = sqrt(pow(positionError[0],2) + pow(positionError[1],2) + pow(positionError[2],2));

    //Publish
    if(true){//fabs(dist) < alpha){
    comm.setX( x_node);//x_robot + incrementVect[0]);
    comm.setY( y_node);//y_robot + incrementVect[1]);
    comm.setZ( z_node);//z_robot + incrementVect[2]);
    }

    else if(fabs(dist) >= alpha){

        //Normalize
        positionError[0] /= dist;
        positionError[1] /= dist;
        positionError[2] /= dist;
        //Calculate relative motion to actual position
        incrementVect[0] = positionError[0] * alpha;
        incrementVect[1] = positionError[1] * alpha;
        incrementVect[2] = positionError[2] * alpha;

        comm.setX(x_robot + incrementVect[0]);
        comm.setY(y_robot + incrementVect[1]);
        comm.setZ(z_robot + incrementVect[2]);

    }
    autoCommand.push_back(comm);
    publish();

}




























