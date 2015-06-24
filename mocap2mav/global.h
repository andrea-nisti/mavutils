#ifndef GLOBAL_H
#define GLOBAL_H

#include "MavState.h"
#include <vector>
#include "utils.h"
#include "ManualControl.h"
#include "PositionDispatcher.h"
#include "Window.h"

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
       extern bool land_sig;
       extern bool landed;
       extern bool was_executing;


    }
    namespace take_off{
       extern bool take_off_sig;
       extern bool take_off_done;
       extern bool was_executing;

    }
    namespace move {
       extern bool move_sig;
       extern bool move_done;

    }

    namespace rotate{
      extern bool rotate_sig;
      extern bool rotate_done;
      extern int rotate_id;
    }



}

extern std::vector<node> nodeList;
extern int actualNode;



#endif // GLOBAL_H

