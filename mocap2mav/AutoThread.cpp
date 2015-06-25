#include "AutoThread.h"
#include "QTime"
#include "MavState.h"
#include "global.h"
#include "utils.h"

#include "params.h"

#include <cmath>
#define PI 3.141592653589
int count, rot_count;
bool contRot_valid;
void calculateYawIntem(double yawSP, double robotHeading, double &yawComm);
void calculateYawIntem2(double yawSP, double robotHeading, double &yawComm);
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
    count = 0;
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

            move(move_alpha);
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

    if(fabs(vz) < 0.01){
        qDebug()<<"error" << error.x <<" " <<error.y <<"P: " << p.x <<" " <<p.y;
        if(++count == land_wait * r_auto) executioner::land::landed = true;

    }
    else{



        //Descending task


        if (robot_state.z >= - 0.2 - offset){
            qDebug()<<"error" << error.x <<" " <<error.y <<"P: " << p.x <<" " <<p.y;
            z += speed * dt;
        }
        else{
         //Centering task

                //Calculate error

            error.x = p.x - robot_state.x;
            error.y = p.y - robot_state.y;
            qDebug()<<"error" << error.x <<" " <<error.y <<"P: " << p.x <<" " <<p.y;

                //Calculate corrected setpoint

            sP.x = error.x * land_gain + p.x;
            sP.y = error.y * land_gain + p.y;

            //wait to recenter


            if(fabs(error.x) < 0.06 && fabs(error.y) < 0.06){

                z = robot_state.z + 0.2;

                if(fabs(error.x) < 0.03 && fabs(error.y) < 0.03){

                    z = robot_state.z + 0.5;

                }

            }





            comm.setX(sP.x);
            comm.setY(sP.y);

        }

        count = 0;
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
    comm.setYaw(nodeList[actualNode].p.yaw);

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
    if(fabs(dist) < alpha){
        comm.setX( x_node);
        comm.setY( y_node);
        comm.setZ( z_node);
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

    calculateYawIntem2(yawSP,robotHeading,yawComm);

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

    double diff = yawSP - robotHeading;
    // Setpoint and actual state with same sign
    if (yawSP*robotHeading > 0){

        if (fabs(diff) < PI/10) yawComm = yawSP;
        else{
            //Increase or decrease yaw sp
            rot_count = 0;
            if (diff >= 0){
                yawComm = robotHeading + PI/18;
            }
            else{
                yawComm = robotHeading - PI/18;
            }

            if (yawComm > PI){
                yawComm = -yawComm + 2*PI/10;
            }
            if (yawComm < -PI){
                yawComm = -yawComm - 2*PI/10;
            }

        }
    }
    else{
        //SP and state with different signs
        int sign;
        if (fabs(fabs(yawSP) - fabs(robotHeading)) < PI/10) yawComm = yawSP;
        else{
            //Increase or decrease yaw sp
            rot_count = 0;

            if (diff >= 0)  sign = 1;
            else    sign = -1;


            if (PI - fabs(diff) < fabs(diff) && fabs(diff) > PI + PI/18) sign = sign * -1;

            if (yawComm > PI){
                yawComm = -yawComm + 2*PI/10;
            }
            if (yawComm < -PI){
                yawComm = -yawComm - 2*PI/10;
            }

            yawComm = robotHeading + sign * PI/18;

        }

    }

}

void calculateYawIntem2(double yawSP,double robotHeading,double &yawComm){

    double yawSp_h = yawSP - robotHeading;

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
        rot_count = 0;
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
















