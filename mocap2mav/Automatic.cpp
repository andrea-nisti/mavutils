#include "Automatic.h"

Automatic::Automatic(QObject *parent) :
    QObject(parent)
{
    connect(&thread,SIGNAL(publish()),this,SLOT(publishWrapper()));
    //thread.start();
}

void Automatic::publishWrapper(){
    publish();
}
