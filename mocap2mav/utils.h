#ifndef UTILS_H
#define UTILS_H

#include<vector>
#define PI 3.141592653589


struct position{

    double x = 0;
    double y = 0;
    double z = 0;
    double yaw = 0;


    position operator-(position p){

        position temp;
        temp.x = x - p.x;
        temp.y = y - p.y;
        temp.z = z - p.z;
        return temp;

    }

    position operator+(position p){

        position temp;
        temp.x = x + p.x;
        temp.y = y + p.y;
        temp.z = z + p.z;
        return temp;

    }

    void operator=(position p){


        this->x = p.x;
        this->y = p.y;
        this->z = p.z;


    }

    position operator*(float f){

        position temp;
        temp.x = f*x;
        temp.y = f*y;
        temp.z = f*z;

        return temp;

    }



};



struct action{
    char type;
    double params[4] = {0,0,0,0};
};

struct node {

    int id;
    position p;
    action a;

};

inline void calculateYawIntem(double yawSP,double robotHeading,double &yawComm){


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
            if (yawComm < -PI){
              yawComm = -yawComm + 2*PI;
            }
        }

    }

}

#endif // UTILS_H
