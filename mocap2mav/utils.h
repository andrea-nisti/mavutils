#ifndef UTILS_H
#define UTILS_H

#include<vector>

struct position{

    double x;
    double y;
    double z;
    double yaw;

};

struct command {

    int priority;
    position p;

};


#endif // UTILS_H
