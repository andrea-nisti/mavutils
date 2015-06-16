#include <QCoreApplication>
#include <QTimer>
#include <QObject>

#include <MavRCOverride.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MavRCOverride mavRC;
    QObject::connect(&mavRC, SIGNAL(finished()), &a, SLOT(quit()));
    QTimer::singleShot(0, &mavRC, SLOT(run()));

    return a.exec();
}

