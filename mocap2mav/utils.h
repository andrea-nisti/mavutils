#ifndef UTILS_H
#define UTILS_H

#include<vector>

struct position{

    double x;
    double y;
    double z;
    double yaw;

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
    char action;

};




#endif // UTILS_H
