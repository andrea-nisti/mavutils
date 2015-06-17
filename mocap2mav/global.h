#ifndef GLOBAL_H
#define GLOBAL_H

#include "MavState.h"
#include <vector>
#include "utils.h"
#include "ManualControl.h"
#include "PositionDispatcher.h"
#include "Window.h"
#include "AutoControl.h"
#include "CommanderThread.h"
#include "MavState.h"
#include "utils.h"

namespace g {
    extern MavState state;
    extern MavState setPoint;
}

extern std::vector <MavState> manualCommand;
extern std::vector <MavState> autoCommand;

namespace executioner{
    namespace land{



    }
    namespace take_off{

    }



}

extern std::vector<node> nodeList;



#endif // GLOBAL_H

