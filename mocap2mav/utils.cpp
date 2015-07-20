#include "utils.h"
#include <cmath>
#include "QDebug"

void calculateYawIntem(double yawSP,double robotHeading,double &yawComm){


    double yawSp_h = yawSP - robotHeading;

    if(yawSp_h > PI ) yawSp_h = yawSp_h - 2*PI;
    else if (yawSp_h < -PI) yawSp_h= yawSp_h + 2*PI;

    if (fabs(yawSp_h) <= PI/18) yawComm = yawSP;
    else if(fabs(yawSp_h) > PI - PI/18){
        //Increase yaw

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
             qDebug() << yawComm;
            if (yawComm < -PI){
              yawComm = yawComm + 2*PI;

            }
        }


    }
    qDebug() << "heading: " << robotHeading <<"Yaw SP: " << yawSP << "Yaw SP_h: " <<yawSp_h << "yawcomm " << yawComm;

}

void calculatePositionInterm(double alpha, position target, position robot_state , MavState &comm){

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
        incrementVect[0] = positionError[0] * alpha;
        incrementVect[1] = positionError[1] * alpha;
        incrementVect[2] = positionError[2] * alpha;

        comm.setX(robot_state.x + incrementVect[0]);
        comm.setY(robot_state.y + incrementVect[1]);
        comm.setZ(target.z); //robot_state.z + incrementVect[2]);

    }


}


