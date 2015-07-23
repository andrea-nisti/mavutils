#include "AutoThread.h"
#include "QTime"
#include "MavState.h"
#include "global.h"
#include "utils.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include "params.h"
#include <sstream>
#include <cmath>
#include <QString>
#include <QDate>
#include <sys/stat.h>
#include <string>
#define PI 3.141592653589

int land_count = 0;int rot_count = 0;int circle_count = 0; int descent_count = 0;
void calculateYawIntem(double yawSP, double robotHeading, double &yawComm);
float plat_error[2] = {0,0};
float error_int = 0;
float error_int_x = 0;
float error_int_y = 0;
float t_traj = 0;


std::ofstream output;
AutoThread::AutoThread(QObject *parent) :
    QThread(parent)
{

    // Log Values
    QString s_file,s_folder;

    std::string str;

    s_file = QTime::currentTime().toString("hh_mm");
    s_file.push_back(".txt");
    s_folder = QDate::currentDate().toString();
    str = s_folder.toStdString();
    struct stat sb;

    if (!(stat(str.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
    {
         mkdir(str.c_str(),0777);
    }

    str = str + "/" + s_file.toStdString();

    output.open(str);

}

AutoThread::~AutoThread()

{
    output.close();

}

void AutoThread::run(){

    QTime rate;


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
    rate.start();

    while (true) {

        next = g::state;
        next_platform = g::platform;

        e_x = g::setPoint.x() - g::state.x();
        e_y = g::setPoint.y() - g::state.y();
        e_z = g::setPoint.z() - g::state.z();

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
            //wait to recenter
            target.y = nodeList[actualNode].p.y;
            target.z = nodeList[actualNode].p.z;
            target.yaw = nodeList[actualNode].p.yaw;

            float alpha = nodeList[actualNode].a.params[0];
            float move_wait = nodeList[actualNode].a.params[1];

            position state;
            state.x = g::state.x();
            state.y = g::state.y();
            state.z = g::state.z();

            MavState comm;
            move(alpha,target,state,comm);
            g::setPoint = comm;
        }

        //Land on moving platform (experimental)
        if(executioner::land::land_plat_sig){

            land_plat(g::platform,g::state,2);

        }

        //rotate
        if(executioner::rotate::rotate_sig){

            rotate();
        }

        //Trajectory
        if(executioner::trajectory::traj_sig){

            double omega = nodeList[actualNode].a.params[0];
            double radius = nodeList[actualNode].a.params[1];
            double secs = nodeList[actualNode].a.params[2];
            double look = nodeList[actualNode].a.params[3];
            double c[2] = {nodeList[actualNode].p.x,nodeList[actualNode].p.y};

            MavState comm = g::setPoint;
            position state;
            state.x = g::state.x();
            state.y = g::state.y();
            state.z = g::state.z();
            state.yaw = g::state.getYaw();

            double init_x = c[0] + radius;
            double init_y = c[1];

            position target;
            target.x = init_x;
            target.y = init_y;
            target.z = comm.z();

            double init_yaw;
            if(look == 1) init_yaw = PI;
            double comm_yaw;
            t_traj += (float)1/r_auto;
            trajectory(omega,radius,c,t_traj,secs,look,state);


        }


        //Writing output file

        float roll,pitch,yaw;
        g::state.orientation2(&roll,&pitch,&yaw);

        output <<g::state.x()<<" "<<g::state.y()<<" "<<g::state.z()<< //Position
            " "<<g::setPoint.x()<<" "<<g::setPoint.y()<<" "<<g::setPoint.z()<< //Position SetPoint

            " "<<e_x<<" "<<e_y<<" "<<e_z<<" "<<                  //Position error
            vx <<" "<<vy<<" "<<vz<<" "<<                         //Velocity
            roll<<" "<<pitch << " " << yaw                       //Attitude
            <<" "<<plat_error[0]<<" "<<plat_error[1];            //platform allignement error

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
            sP.x = error.x * land_gain * 0.9 + p.x;
            sP.y = error.y * land_gain * 0.9 + p.y;

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

void AutoThread::land_plat(MavState platform,MavState robot_state,float alpha){

    MavState comm = g::setPoint;

    float position_error[2] = {platform.x() - robot_state.x(),platform.y() - robot_state.y() };

    error_int_x += position_error[0];
    error_int_y += position_error[1];

    float x_sp = platform.x() + kp * position_error[0] + ki * error_int_x;
    float y_sp = platform.y() + kp * position_error[1] + ki* error_int_y;

    double dist = sqrt(pow(x_sp - robot_state.x(),2) + pow(y_sp - robot_state.y(),2));

    if (true){//dist <= alpha){

        //comm.setX(x_sp);
        comm.setY(y_sp);
        comm.setX((float)0.5);

    }
    else{

        float sp_error[2];

        sp_error[0] = (x_sp - robot_state.x())/dist;
        sp_error[1] = (y_sp - robot_state.y())/dist;

        x_sp = robot_state.x() + sp_error[0] * alpha;
        y_sp = robot_state.y() + sp_error[1] * alpha;

        comm.setX((float)0.5);
        comm.setY(y_sp);

        //executioner::land::reset_int = true;

    }

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

void AutoThread::move(double alpha, position target, position robot_state , MavState &comm){

    comm = g::setPoint;

    //Calculate error vector

    calculatePositionInterm(alpha, target,robot_state ,comm);

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

void AutoThread::trajectory(double omega,double rad,double c[2],float t,int secs,float look,position robot_state){

    MavState comm = g::setPoint;

    // Circular trajectory

    double x_sp = 0.2 + rad*cos(omega * t);
    double y_sp = rad*sin(omega * t);

    //if (look == 1){
        double yawSP = atan2( -g::state.y(),0.2 - g::state.x());
        double yawComm;
        //calculateYawIntem(yawSP,robot_state.yaw,yawComm);
        comm.setYaw(yawSP);
    //}

    comm.setX(x_sp);
    comm.setY(y_sp);

    g::setPoint = comm;

    if(++circle_count >= secs * r_auto){ executioner::trajectory::traj_done = true; circle_count = 0;}

}

void AutoThread::startMe(){
    this->start();
}
















