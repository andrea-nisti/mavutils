#include "ExecThread.h"
#include "global.h"
#include <vector>
#include "utils.h"
#define PI 3.141592653589
//#include "params.h"

namespace executioner{
    namespace land{
       bool land_sig;
       bool landed;
       bool was_executing;
       bool land_plat_sig;
       bool reset_int;

    }
    namespace take_off{
       bool take_off_sig;
       bool take_off_done;
       bool was_executing;

    }
    namespace move{
       bool move_sig;
       bool move_done;
       bool was_executing;
       bool keep_sp_still;
    }
    namespace rotate{
       bool rotate_sig;
       bool rotate_done;
       int rotate_id;
    }
    namespace trajectory{
       bool traj_sig;
       bool traj_done;
       bool was_executing;
       double traj_time;
       bool ready;
    }

}

std::vector<node> nodeList;
int actualNode = 0;
int r_exec = 120; //Hz

int move_count = 0;
bool checkActions(char a);
void signalsReset();
bool can_run = false;
ExecThread::ExecThread(QObject *parent) :
    QThread(parent)
{


    // Fill Node list

    node node1;
    node1.a.type = 't';
    node1.a.params[0] = -1.5; //height
    nodeList.push_back(node1);

    node move;
    move.a.type = 'm';
    move.p.x = 1.22;
    move.p.y = 1.14;
    move.p.z = -1.4;
    move.a.params[0] = 0.6;
    move.a.params[1] = 3;
    nodeList.push_back(move);

    node rotate;
    rotate.a.type = 'r';
    rotate.a.params[0] = 1;
    rotate.p.yaw = 0;
    nodeList.push_back(rotate);

    node land;
    land.a.type = 'l';
    land.a.params[0] = 4; //height velocity
    land.a.params[1] = -0.73; // offset
    nodeList.push_back(land);

    nodeList.push_back(node1);

    move.p.x = -0.8;
    move.p.y = 1;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 3;
    nodeList.push_back(move);

    move.p.x = -0.5;
    move.p.y = 0;
    move.p.z = -0.8;
    move.a.params[0] = 0.6;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    rotate.a.params[0] = 1;
    rotate.p.yaw = -PI/4;
    nodeList.push_back(rotate);

    move.p.x = 0.264;
    move.p.y = -0.914;
    move.p.z = -1;
    move.a.params[0] = 0.5;
    move.a.params[1] = 5;
    nodeList.push_back(move);

    land.a.params[0] = 5; //height velocity
    land.a.params[1] = -0.31; // offset
    nodeList.push_back(land);
    if(nodeList.size()>0){

        can_run = true;
    }
    else{

        qDebug() << "WARNING, empty list";
        can_run = false;
    }

}

void ExecThread::run(){

    /*_____ACTIONS: _____________/*
     *
     * m : move (staring point)
     * l : land (landing altitude)
     * t : take off (altitude)
     * f : follow trajectory (traj type, staring point)
     * h : hover (seconds)
     * s : stare (staring point)
     *
     */

    QTime rate;


    qDebug() << "executioner from: " << QThread::currentThreadId();

    signalsReset();
    bool message = true;
    rate.start();
    while(actualNode < nodeList.size()){


        char act = nodeList[actualNode].a.type;

        // Spread the word
        if(message) {
                qDebug() << "Performing node: " << actualNode << "with action: " << act;
                message = false;
        }

        //Toggle action

        switch (act) {

        //MOVE
        case 'm':
            if(!executioner::move::was_executing){
                move_count = nodeList[actualNode].a.params[1];
                executioner::move::was_executing = true;

            }
            executioner::move::move_sig = true;
            break;

        //LAND
        case 'l':
            //Land on previous move position
            if(!executioner::land::was_executing){

                nodeList[actualNode].p.x = g::setPoint.x();
                nodeList[actualNode].p.y = g::setPoint.y();

                qDebug() << "Landing on: "<<nodeList[actualNode].p.x<<" "<<nodeList[actualNode].p.y;

                executioner::land::was_executing = true;
            }

            executioner::land::land_plat_sig = true;
            break;

        //TAKEOFF
        case 't':
            //Takeoff from actual position
            if(!executioner::take_off::was_executing){

                nodeList[actualNode].p.x = g::state.x();
                nodeList[actualNode].p.y = g::state.y();
                nodeList[actualNode].p.z = g::state.z();
                nodeList[actualNode].p.yaw = g::state.getYaw();

                executioner::take_off::was_executing = true;
            }

            executioner::take_off::take_off_sig = true;
            break;
        case 'r':
            executioner::rotate::rotate_id = actualNode;
            executioner::rotate::rotate_sig = true;
            break;

        case 'c':

            if(!executioner::trajectory::was_executing){

                nodeList[actualNode].p.x = g::setPoint.x();
                nodeList[actualNode].p.y = g::setPoint.y();
                executioner::trajectory::was_executing = true;

            }
            executioner::trajectory::traj_sig = true;

            break;
        default:
            break;
        }

        //Is the action completed?
        if(checkActions(act)) {
            actualNode++;
            signalsReset();
            message = true;
        }

        //Spin
        msleep(1000/r_exec - (float)rate.elapsed());
        rate.restart();
    }

    qDebug() << "no more nodes!";
    this->exit();


}


bool checkActions(char a){

    switch (a) {
    //MOVE
    case 'm':

        if(fabs(g::state.x() - nodeList[actualNode].p.x) < 0.15 &&
           fabs(g::state.y() - nodeList[actualNode].p.y) < 0.15 &&
           fabs(g::state.z() - nodeList[actualNode].p.z) < 0.15 ){
           float move_wait = nodeList[actualNode].a.params[1];
           if(++move_count >= move_wait * r_exec){move_count = 0; executioner::move::move_done = true;}

        }
        else{
            executioner::move::move_done = false;
            move_count = 0;

        }

        return executioner::move::move_done;
        break;
    //LAND
    case 'l':

        return executioner::land::landed;

        break;
    //TAKEOFF
    case 't':

        if(fabs(g::state.z() - nodeList[actualNode].a.params[0]) < 0.1 ){
            executioner::take_off::take_off_done = true;
        }
        else{
            executioner::take_off::take_off_done = false;
        }

        return executioner::take_off::take_off_done;
        break;

     //ROTATE

     case 'r':

        return executioner::rotate::rotate_done;

        break;
     case 'c':

        return executioner::trajectory::traj_done;

        break;



    default:

        break;
    }

}

void signalsReset(){

    executioner::land::landed = false;
    executioner::land::land_sig = false;
    executioner::land::was_executing = false;
    executioner::land::land_plat_sig = false;

    executioner::take_off::take_off_sig = false;
    executioner::take_off::take_off_done = false;
    executioner::take_off::was_executing = false;

    executioner::move::move_done = false;
    executioner::move::move_sig = false;
    executioner::move::was_executing = true;
    executioner::move::keep_sp_still = false;

    executioner::rotate::rotate_done = false;
    executioner::rotate::rotate_sig = false;

    executioner::trajectory::traj_done = false;
    executioner::trajectory::traj_sig = false;
    executioner::trajectory::was_executing = false;
    executioner::trajectory::traj_time = 0;
    executioner::trajectory::ready = false;

}

void ExecThread::startMe(){
    this->start();
}



/*  TABLE LANDING

    node node1;
    node1.a.type = 't';
    node1.a.params[0] = -1.5; //height
    nodeList.push_back(node1);node node1;
    node1.a.type = 't';
    node1.a.params[0] = -1.5; //height
    nodeList.push_back(node1);

    node move;
    move.a.type = 'm';
    move.p.x = 1.22;
    move.p.y = 1.14;
    move.p.z = -1.4;
    move.a.params[0] = 0.6;
    move.a.params[1] = 3;
    nodeList.push_back(move);

    node rotate;
    rotate.a.type = 'r';
    rotate.a.params[0] = 1;
    rotate.p.yaw = 0;
    nodeList.push_back(rotate);

    node land;
    land.a.type = 'l';
    land.a.params[0] = 4; //height velocity
    land.a.params[1] = -0.73; // offset
    nodeList.push_back(land);

    nodeList.push_back(node1);

    move.p.x = -0.8;
    move.p.y = 1;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 3;
    nodeList.push_back(move);

    move.p.x = -0.5;
    move.p.y = 0;
    move.p.z = -0.8;
    move.a.params[0] = 0.6;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    rotate.a.params[0] = 1;
    rotate.p.yaw = -PI/4;
    nodeList.push_back(rotate);

    move.p.x = 0.264;
    move.p.y = -0.914;
    move.p.z = -1;
    move.a.params[0] = 0.5;
    move.a.params[1] = 5;
    nodeList.push_back(move);

    land.a.params[0] = 5; //height velocity
    land.a.params[1] = -0.31; // offset
    nodeList.push_back(land);

    node move;
    move.a.type = 'm';
    move.p.x = 1.22;
    move.p.y = 1.14;
    move.p.z = -1.4;
    move.a.params[0] = 0.6;
    move.a.params[1] = 3;
    nodeList.push_back(move);

    node rotate;
    rotate.a.type = 'r';
    rotate.a.params[0] = 1;
    rotate.p.yaw = 0;
    nodeList.push_back(rotate);

    node land;
    land.a.type = 'l';
    land.a.params[0] = 4; //height velocity
    land.a.params[1] = -0.73; // offset
    nodeList.push_back(land);

    nodeList.push_back(node1);

    move.p.x = -0.8;
    move.p.y = 1;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 3;
    nodeList.push_back(move);

    move.p.x = -0.5;
    move.p.y = 0;
    move.p.z = -0.8;
    move.a.params[0] = 0.6;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    rotate.a.params[0] = 1;
    rotate.p.yaw = -PI/4;
    nodeList.push_back(rotate);

    move.p.x = 0.264;
    move.p.y = -0.914;
    move.p.z = -1;
    move.a.params[0] = 0.5;
    move.a.params[1] = 5;
    nodeList.push_back(move);

    land.a.params[0] = 5; //height velocity
    land.a.params[1] = -0.31; // offset
    nodeList.push_back(land);

*/




/* Circle

    // Fill Node list

    node node1;
    node1.a.type = 't';
    node1.a.params[0] = -1; //height
    nodeList.push_back(node1);

    node rotate;
    rotate.a.type = 'r';
    rotate.a.params[0] = 1;
    rotate.p.yaw = PI;
    nodeList.push_back(rotate);


    node move;
    move.a.type = 'm';
    move.p.x = 0.6;
    move.p.y = 0;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 1;
    nodeList.push_back(move);

    node circle;
    circle.p.x = 0;
    circle.p.y = 0;
    circle.a.type = 'c';
    circle.a.params[0] = 0.5; //Omega
    circle.a.params[1] = 0.6; //Rad
    circle.a.params[2] = 8;  //Secs
    circle.a.params[3] = 0;   //look
    nodeList.push_back(circle);

    //Circle done
    move.p.x = -0.8;
    move.p.y = -0.8;
    move.p.z = -1;
    move.a.params[0] = 0.5;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = 0;
    move.p.y = -0.8;
    move.p.z = -0.8;
    move.a.params[0] = 1;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = 0.8;
    move.p.y = 0;
    move.p.z = -1.1;
    move.a.params[0] = 1;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = 0.8;
    move.p.y = 0.8;
    move.p.z = -0.5;
    move.a.params[0] = 1;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = -0.8;
    move.p.y = 1;
    move.p.z = -0.8;
    move.a.params[0] = 0.5;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    rotate.p.x = -1.2;
    rotate.p.y = 1.2;
    rotate.a.params[0] = 0;
    rotate.p.yaw = 0;
    nodeList.push_back(rotate);

    move.p.x = -0.8;
    move.p.y = 1;
    move.p.z = -1;
    move.a.params[0] = 0.5;
    move.a.params[1] = 3;
    nodeList.push_back(move);

    move.p.x = 1.2;
    move.p.y = 1.1;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 1;
    nodeList.push_back(move);

    node land;
    land.a.type = 'l';
    land.a.params[0] = 4; //height velocity
    land.a.params[1] = 0; // offset
    nodeList.push_back(land);

*/


/* Square
 *
    node node1;
    node1.a.type = 't';
    node1.a.params[0] = -1; //height
    nodeList.push_back(node1);

    node move;
    move.a.type = 'm';
    move.p.x = 0.8;
    move.p.y = -0.8;
    move.p.z = -1;
    move.a.params[0] = 0.3;
    move.a.params[1] = 1;
    nodeList.push_back(move);

    move.p.x = 0.8;
    move.p.y = 0.8;
    move.p.z = -1;
    move.a.params[0] = 0.3;
    move.a.params[1] = 1;
    nodeList.push_back(move);

    move.p.x = -0.8;
    move.p.y =  0.8;
    move.p.z = -1;
    move.a.params[0] = 0.3;
    move.a.params[1] = 1;
    nodeList.push_back(move);

    move.p.x = -0.8;
    move.p.y = -0.8;
    move.p.z = -1;
    move.a.params[0] = 0.3;
    move.a.params[1] = 1;
    nodeList.push_back(move);

    move.p.x = 0;
    move.p.y = 0;
    move.p.z = -1;
    move.a.params[0] = 0.3;
    move.a.params[1] = 1;
    nodeList.push_back(move);

    node land;
    land.a.type = 'l';
    land.a.params[0] = 4; //height velocity
    land.a.params[1] = 0; // offset
    nodeList.push_back(land);


  */


/* LAND

    // Fill Node list

    node node1;
    node1.a.type = 't';
    node1.a.params[0] = -1; //height
    nodeList.push_back(node1);

    node move;
    move.a.type = 'm';
    move.p.x = 0.5;
    move.p.y = -0.5;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = 0;
    move.p.y = 0;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 4;
    nodeList.push_back(move);

    node land;
    land.a.type = 'l';
    land.a.params[0] = 4; //height velocity
    land.a.params[1] = 0; // offset
    nodeList.push_back(land);

    node1.a.params[0] = -1; //height
    nodeList.push_back(node1);

    move.p.x = 0.5;
    move.p.y = 0.5;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = 0;
    move.p.y = 0;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 4;
    nodeList.push_back(move);

    land.a.params[0] = 4; //height velocity
    land.a.params[1] = 0; // offset
    nodeList.push_back(land);

    node1.a.params[0] = -1; //height
    nodeList.push_back(node1);

    move.p.x = -0.5;
    move.p.y = 0.5;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = 0;
    move.p.y = 0;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 4;
    nodeList.push_back(move);

    land.a.params[0] = 4; //height velocity
    land.a.params[1] = 0; // offset
    nodeList.push_back(land);

    node1.a.params[0] = -1; //height
    nodeList.push_back(node1);

    move.p.x = -0.5;
    move.p.y = -0.5;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 0;
    nodeList.push_back(move);

    move.p.x = 0;
    move.p.y = 0;
    move.p.z = -1;
    move.a.params[0] = 0.6;
    move.a.params[1] = 4;
    nodeList.push_back(move);

    land.a.params[0] = 4; //height velocity
    land.a.params[1] = 0; // offset
    nodeList.push_back(land);



*/
