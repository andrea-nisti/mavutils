#include "ExecThread.h"
#include "global.h"
#include <vector>
#include "utils.h"
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

    node node2;
    node2.a.type = 'm';
    node2.p.x = 1;
    node2.p.y = 0;
    node2.p.z =-1.5;
    nodeList.push_back(node2);

    node node3;
    node3.a.type = 'm';
    node3.p.x = 1.018;
    node3.p.y = 1.072;
    node3.p.z =-1.7;
    nodeList.push_back(node3);

    node node4;
    node4.a.type = 'l';
    node4.a.params[0] = 0.4; //height velocity
    node4.a.params[1] = 0.0;
    nodeList.push_back(node4);
    nodeList.push_back(node1);

    node node5;
    node5.a.type = 'm';
    node5.p.x = 0;
    node5.p.y = 0;
    node5.p.z =-0.7;
    nodeList.push_back(node5);

    nodeList.push_back(node3);
    nodeList.push_back(node4);





    //nodeList.push_back(node1);



    //Initialization for take off
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
                nodeList[actualNode].p.z = nodeList[actualNode - 1].p.z;

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

}

void ExecThread::startMe(){
    this->start();
}





