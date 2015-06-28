#include "AutoThread.h"
#include "QTime"
#include "MavState.h"
#include "global.h"
#include "utils.h"

#include "params.h"

#include <cmath>
#define PI 3.141592653589

int land_count, rot_count;
bool contRot_valid;
void calculateYawIntem(double yawSP, double robotHeading, double &yawComm);

AutoThread::AutoThread(QObject *parent) :
    QThread(parent)
{

}

void AutoThread::run(){

    QTime rate;

    qDebug() << "automatic from: " << QThread::currentThreadId();
    MavState previous = g::state;
    MavState next;
    rate.start();
    land_count = 0;
    rot_count = 0;
    double vz;
    contRot_valid = false;
    while (true) {

        next = g::state;

        vz = r_auto * (next.z() - previous.z()) ;

        //takeoff
        if(executioner::take_off::take_off_sig){
                takeOff();
        }

        //land
        if(executioner::land::land_sig){

            position state;
            state.x = g::state.x();
            state.y = g::state.y();
            state.z = g::state.z();
            position p;

            p.x = nodeList[actualNode].p.x;
            p.y = nodeList[actualNode].p.y;


            float vel = nodeList[actualNode].a.params[0];
            land(vel,(float)1/r_auto,vz,p,state);
        }

        //move
        if(executioner::move::move_sig){

            position target;

            target.x = nodeList[actualNode].p.x;
            target.y = nodeList[actualNode].p.y;
            target.z = nodeList[actualNode].p.z;
            target.yaw = nodeList[actualNode].p.yaw;

            position state;
            state.x = g::state.x();
            state.y = g::state.y();
            state.z = g::state.z();
            state.yaw = g::state.getYaw();

            move2(move_alpha,target,state,(float)1/r_auto);
        }

        //rotate
        if(executioner::rotate::rotate_sig){

            rotate();
        }


        previous = next;

        msleep(1000/r_auto - (float)rate.elapsed());
        rate.restart();


    }

}

void AutoThread::land(float speed, float dt,double vz, position p , position robot_state){

    //landing procedure

    MavState comm = g::setPoint;
    position error, sP;

    float offset = nodeList[actualNode].a.params[1];
    float z = comm.z();

    bool descend_valid = false;
    if(fabs(vz) < 0.01 ){

        z = z + 1;
        if(++land_count == land_wait * r_auto) {


            executioner::land::landed = true;

        }

    }
    else{

        //Descending task

        error.x = p.x - robot_state.x;
        error.y = p.y - robot_state.y;

        if (robot_state.z - offset >= - 0.20 ){

            descend_valid = true;
            sP.x = error.x * land_gain * 0.6 + p.x;
            sP.y = error.y * land_gain * 0.6 + p.y;

            z += speed*dt;
        }
        else{

         //Centering task

                //Calculate error


            //qDebug()<<"descending: error" << error.x <<" " <<error.y <<"P: " << p.x <<" " <<p.y;

                //Calculate corrected setpoint

            sP.x = error.x * land_gain + p.x;
            sP.y = error.y * land_gain + p.y;

            //wait to recenter


            if(fabs(error.x) < 0.06 && fabs(error.y) < 0.06){ z = robot_state.z + 0.2; descend_valid = true;}

            else if(fabs(error.x) < 0.03 && fabs(error.y) < 0.03){ z = robot_state.z + 0.4; descend_valid = true;}


            comm.setX(sP.x);
            comm.setY(sP.y);

        }

        land_count = 0;
        executioner::land::landed = false;

    }

    if(descend_valid) comm.setZ(z);
    autoCommand.push_back(comm);
    publish();

}

void AutoThread::takeOff(){

    MavState comm = g::setPoint;

    comm.setX(nodeList[actualNode].p.x);
    comm.setY(nodeList[actualNode].p.y);
    comm.setYaw(nodeList[actualNode].p.yaw);

    double height = nodeList[actualNode].a.params[0];

    comm.setZ(height);
    autoCommand.push_back(comm);
    publish();

}


void AutoThread::move(double alpha, position target, position robot_state){

    MavState comm = g::setPoint;

    //Calculate error vector
    double positionError[3] = {target.x - robot_state.x ,target.y - robot_state.y , target.z - robot_state.z};
    double incrementVect[3];

    double dist = sqrt(pow(positionError[0],2) + pow(positionError[1],2)); //+ pow(positionError[2],2));

    //Publish
    if(true){//fabs(dist) <= alpha){
        comm.setX( target.x);
        comm.setY( target.y);
        comm.setZ( target.z);
    }

    else if(fabs(dist) > alpha){

        //Normalize
        positionError[0] = positionError[0] / dist;
        positionError[1] = positionError[1] / dist;
        positionError[2] = positionError[2] / dist;

        //Calculate relative motion to actual position
        incrementVect[0] = positionError[0] * alpha ;
        incrementVect[1] = positionError[1] * alpha;
        incrementVect[2] = positionError[2] * alpha;

        comm.setX(robot_state.x + incrementVect[0]);
        comm.setY(robot_state.y + incrementVect[1]);
        comm.setZ(robot_state.z + incrementVect[2]);

    }

    autoCommand.push_back(comm);
    publish();

}

void AutoThread::move2(double alpha, position target, position robot_state,float dt){

    MavState comm = g::setPoint;

    //Calculate error vector
    double positionError[3] = {target.x - comm.x() ,target.y - comm.y() , target.z - comm.z()};
    double incrementVect[3];

    double dist = sqrt(pow(positionError[0],2) + pow(positionError[1],2) + pow(positionError[2],2));

    //Publish
    if(fabs(dist) <= alpha){
        comm.setX( target.x);
        comm.setY( target.y);
        comm.setZ( target.z);
    }

    else if(fabs(dist) > alpha){

        //XXX Change alpha according to y/xs

        //Normalize
        positionError[0] = positionError[0] / dist;
        positionError[1] = positionError[1] / dist;
        positionError[2] = positionError[2] / dist;

        //Calculate relative motion to actual position
        incrementVect[0] = positionError[0] * alpha * dt;
        incrementVect[1] = positionError[1] * alpha * dt;
        incrementVect[2] = positionError[2] * alpha * dt;

        //comm.x += incrementVect[0] ?

        comm.setX(comm.x() + incrementVect[0]);
        comm.setY(comm.y() + incrementVect[1]);
        comm.setZ(comm.z() + incrementVect[2]);

    }

    autoCommand.push_back(comm);
    publish();

}


void AutoThread::rotate(){

    MavState commRot = g::setPoint;

    double robotHeading = g::state.getYaw();
    double angle_valid = nodeList[executioner::rotate::rotate_id].a.params[0];
    double yawSP = nodeList[executioner::rotate::rotate_id].p.yaw;
    double yawComm;

    if (angle_valid == 0){

        double x_target = nodeList[executioner::rotate::rotate_id].p.x;
        double y_target = nodeList[executioner::rotate::rotate_id].p.y;

        yawSP = atan2(y_target - g::state.y(),x_target - g::state.x());

    }

    calculateYawIntem(yawSP,robotHeading,yawComm);

    commRot.setYaw(yawComm);
    autoCommand.push_back(commRot);
    publish();

    if(++rot_count == rot_wait * r_auto && fabs(fabs(yawSP) - fabs(robotHeading)) < PI/10){
        executioner::rotate::rotate_done = true; rot_count = 0;
        rot_count = 0;
    }


}

void AutoThread::startMe(){
    this->start();
}





void calculateYawIntem(double yawSP,double robotHeading,double &yawComm){


    double yawSp_h = yawSP - robotHeading;

    if(yawSp_h > PI ) yawSp_h = yawSp_h - 2*PI;
    else if (yawSp_h < -PI) yawSp_h= yawSp_h + 2*PI;

    if (fabs(yawSp_h) <= PI/18) yawComm = yawSP;
    else if(fabs(yawSp_h) > PI - PI/18){
        //Increase yaw
        //rot_count = 0;
        yawComm = robotHeading + PI / 18 ;
        if (yawComm > PI){
            yawComm = yawComm - 2*PI;
        }
    }
    else{
        //rot_count = 0;

        if (yawSp_h > 0){
            //Increase yaw
            yawComm = robotHeading + PI / 18 ;
            if (yawComm > PI){
               yawComm = yawComm - 2*PI;
            }

        }
        else{
            //decrease yaw
            yawComm = robotHeading - PI / 18 ;
            if (yawComm < -PI){
              yawComm = -yawComm + 2*PI;
            }
        }

    }

}

















