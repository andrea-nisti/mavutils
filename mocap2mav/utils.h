#ifndef UTILS_H
#define UTILS_H

#include<vector>

struct position{

    double x = 0;
    double y = 0;
    double z = 0;
    double yaw = 0;

    void operator=(position &p){
        x = p.x;
        y = p.y;
        z = p.z;
        yaw = p.yaw;
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




#endif // UTILS_H
