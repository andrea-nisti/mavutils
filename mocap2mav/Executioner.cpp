#include "Executioner.h"

Executioner::Executioner(QObject *parent) :
    QObject(parent)
{
    thread.start();
}
