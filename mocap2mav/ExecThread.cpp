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

    }
    namespace take_off{
       bool take_off_sig;
       bool take_off_done;
       bool was_executing;

    }
    namespace move{
       bool move_sig;
       bool move_done;
    }
    namespace rotate{
       bool rotate_sig;
       bool rotate_done;
       int rotate_id;
    }

}

std::vector<node> nodeList;
int actualNode = 0;


bool checkActions(char a);
void signalsReset();
bool can_run = false;
ExecThread::ExecThread(QObject *parent) :
    QThread(parent)
{
    //fill nodelist

    node node1;
    node1.a.type = 't';
    node1.a.params[0] = -1.5; //height
    nodeList.push_back(node1);

    node move;

    node rotate;
    rotate.a.type = 'r';
    rotate.a.params[0] = 1; //angle_valid
    rotate.p.x = 1.03;
    rotate.p.y = 1;
    rotate.p.yaw = 0;
    nodeList.push_back(rotate);

    move.a.type = 'm';
    move.p.x = 1.0;
    move.p.y = 1.0;
    move.p.z =-1.8;
    nodeList.push_back(move);

    node land;
    land.a.type = 'l';
    land.a.params[0] = 1.3; //height velocity
    land.a.params[1] = - 0.713; // offset
    nodeList.push_back(land);

    /*
    nodeList.push_back(node1);
    rotate.p.x = 0;
    rotate.p.y = 0;
    nodeList.push_back(rotate);

    move.p.x = 0.5;
    move.p.y = -0.5;
    move.p.z = -1;
    nodeList.push_back(move);

    rotate.a.params[0] = 1;
    rotate.p.yaw = 0;
    nodeList.push_back(rotate);
    move.p.x = 1.018;
    move.p.y = 1.072;
    move.p.z =-1.5;
    nodeList.push_back(move);
    nodeList.push_back(land);
*/





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
    int r_exec = 120; //Hz


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
            executioner::move::move_sig = true;
            break;

        //LAND
        case 'l':
            //Land on previous move position
            if(!executioner::land::was_executing){

                nodeList[actualNode].p.x = nodeList[actualNode - 1].p.x;
                nodeList[actualNode].p.y = nodeList[actualNode - 1].p.y;

                qDebug() << "Landing on: "<<nodeList[actualNode].p.x<<" "<<nodeList[actualNode].p.y;

                executioner::land::was_executing = true;
            }

            executioner::land::land_sig = true;
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

int move_count = 0;
bool checkActions(char a){

    switch (a) {
    //MOVE
    case 'm':

        if(fabs(g::state.x() - nodeList[actualNode].p.x) < 0.15 &&
           fabs(g::state.y() - nodeList[actualNode].p.y) < 0.15 &&
           fabs(g::state.z() - nodeList[actualNode].p.z) < 0.15 ){

           if(++move_count == 3 * 120) executioner::move::move_done = true;

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




    default:

        break;
    }

}

void signalsReset(){

    executioner::land::landed = false;
    executioner::land::land_sig = false;
    executioner::land::was_executing = false;
    executioner::take_off::take_off_sig = false;
    executioner::take_off::take_off_done = false;
    executioner::take_off::was_executing = false;
    executioner::move::move_done = false;
    executioner::move::move_sig = false;
    executioner::rotate::rotate_done = false;
    executioner::rotate::rotate_sig = false;

}

void ExecThread::startMe(){
    this->start();
}





