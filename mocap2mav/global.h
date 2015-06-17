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

namespace g {
    extern MavState state;
    extern MavState setPoint;
}

extern std::vector <MavState> manualCommand;
extern std::vector <MavState> autoCommand;
//extern CommanderThread commanderT;



#endif // GLOBAL_H

