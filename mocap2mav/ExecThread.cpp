#include "ExecThread.h"
#include "global.h"
#include <vector>


std::vector<node> nodeList;
ExecThread::ExecThread(QObject *parent) :
    QThread(parent)
{

}

