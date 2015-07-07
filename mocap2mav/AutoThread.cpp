#include "AutoThread.h"
#include "QTime"
#include "MavState.h"
#include "global.h"
#include "utils.h"
#include <iostream>
#include<ostream>
#include <fstream>
#include "params.h"

#include <cmath>
#define PI 3.141592653589

int land_count = 0;int rot_count = 0;int circle_count = 0; int descent_count = 0;
void calculateYawIntem(double yawSP, double robotHeading, double &yawComm);
float plat_error = 0;
float error_int = 0;

std::ofstream output;
AutoThread::AutoThread(QObject *parent) :
    QThread(parent)
{
    output.open("output.txt");


}

AutoThread::~AutoThread()

{
    output.close();

}

void AutoThread::run(){

    QTime rate;
    QTime timer;

    qDebug() << "automatic from: " << QThread::currentThreadId();
    MavState previous = g::state;
    MavState previous_platform = g::platform;
    MavState next, next_platform;

    land_count = 0;
    rot_count = 0;
    double vz = 0;
    double vx = 0;
    double vy = 0;
    double e_x = 0;
    double e_y = 0;
    double e_z = 0;
    float vy_platform = 0;
    timer.start();
    rate.start();
    while (true) {

        next = g::state;
        next_platform = g::platform;


        vz = r_auto * (next.z() - previous.z()) ;
        vx = r_auto * (next.x() - previous.x()) ;
        vy = r_auto * (next.y() - previous.y()) ;
        vy_platform = r_auto * (next_platform.y() - previous_platform.y()) ;
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
            target.y = g::platform.y();
            target.z = nodeList[actualNode].p.z;
            target.yaw = nodeList[actualNode].p.yaw;

            position state;
            state.x = g::state.x();
            state.y = g::state.y();
            state.z = g::state.z();


            e_x = g::setPoint.x() - g::state.x();
            e_y = g::setPoint.y() - g::state.y();
            e_z = g::setPoint.z() - g::state.z();

            state.yaw = g::state.getYaw();

            plat_error = target.y - state.y;
            error_int += plat_error;
            target.y = g::platform.y() + 0.01 * error_int + 0.13 * plat_error;


            move(move_alpha,target,state);
        }

        //rotate
        if(executioner::rotate::rotate_sig){

            rotate();
        }
        //Circle
        if(executioner::circle::circle_sig){

            double omega = nodeList[actualNode].a.params[0];
            double radius = nodeList[actualNode].a.params[1];
            double secs = nodeList[actualNode].a.params[2];
            double c[2] = {nodeList[actualNode].p.x,nodeList[actualNode].p.y};
            circle(omega,radius,c,(float)1/r_auto,secs);
        }

        //Logging

        //Writing output file

        float roll,pitch,yaw;
        g::state.orientation2(&roll,&pitch,&yaw);


        output <<g::state.x()<<" "<<g::state.y()<<" "<<g::state.z()<< //Position
            " "<<g::setPoint.x()<<" "<<g::setPoint.y()<<" "<<g::setPoint.z()<< //Position SetPoint

                 " "<<e_x<<" "<<e_y<<" "<<e_z<<" "<<                  //Position error
                 vx <<" "<<vy<<" "<<vz<<" "<<                         //Velocity
                roll<<" "<<pitch << " " << yaw ;                     //Attitude


        output << ";\n";

        previous = next;
        previous_platform = next_platform;
        msleep(1000/r_auto - (float)rate.elapsed());
        rate.restart();

    }

}

void AutoThread::land(float speed, float dt,double vz, position p , position robot_state){

    //landing procedure

    MavState comm = g::setPoint;
    position error, sP;
    float descending_rate = 0;

    float offset = nodeList[actualNode].a.params[1];
    float z = comm.z();

    bool descend_valid = false;
    if(fabs(vz) < 0.01 && robot_state.z - offset >= - 0.10){


        if(++land_count == land_wait * r_auto) {

            land_count = 0;
            executioner::land::landed = true;

        }

    }
    else{

        //Descending task

        error.x = p.x - robot_state.x;
        error.y = p.y - robot_state.y;

        if (robot_state.z - offset >= - 0.15 ){

            if(fabs(error.x) < 0.03 && fabs(error.y) < 0.03) {

              descend_valid = true;

            }
            else speed = 0;
            sP.x = error.x * land_gain * 0.7 + p.x;
            sP.y = error.y * land_gain * 0.7 + p.y;

            comm.setX(sP.x);
            comm.setY(sP.y);

            z += speed*dt;
        }
        else{

         //Centering task


            sP.x = error.x * land_gain + p.x;
            sP.y = error.y * land_gain + p.y;

            //wait to recenter

            if(fabs(error.x) < 0.08 && fabs(error.y) < 0.08){ z = g::state.z() + 0.3; descend_valid = true;}

            else if(fabs(error.x) < 0.05 && fabs(error.y) < 0.05){ z = g::state.z() + 0.5; descend_valid = true;}

            //z += descending_rate * dt;
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

    double dist = sqrt(pow(positionError[0],2) + pow(positionError[1],2) + pow(positionError[2],2));

    //Publish
    if(fabs(dist) <= alpha){
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
        comm.setZ(target.z); //robot_state.z + incrementVect[2]);

    }


    g::setPoint = comm;

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

    if( fabs(fabs(yawSP) - fabs(robotHeading)) < PI/10){

        if(++rot_count == rot_wait * r_auto){

            qDebug() << "done rotation" ;
            rot_count = 0;
            executioner::rotate::rotate_done = true;

        }
    }


}

void AutoThread::circle(double omega,double rad,double c[2],float dt,int secs){

    MavState comm = g::setPoint;

      // Circular trajectory

    double x_sp = c[0] + rad*cos(omega * dt);
    double y_sp = c[1] + rad*sin(omega * dt);
    double yawSP = atan2(c[0] - g::state.y(),c[1] - g::state.x());
    double yawComm;
    calculateYawIntem(yawSP,g::state.getYaw(),yawComm);

    comm.setX(x_sp);
    comm.setY(y_sp);
    comm.setYaw(yawComm);


    g::setPoint = comm;

    if(++circle_count >= secs * r_auto){ executioner::circle::circle_done = true; circle_count = 0;}

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
        rot_count = 0;
        yawComm = robotHeading + PI / 18 ;
        if (yawComm > PI){
            yawComm = yawComm - 2*PI;
        }
    }
    else{


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


void log(float a,float b){

}














