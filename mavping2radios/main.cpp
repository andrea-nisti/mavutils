#include <QCoreApplication>
#include <QTimer>
#include <QObject>

#include <MavPing2.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MavPing2 mavPing;
    QObject::connect(&mavPing, SIGNAL(finished()), &a, SLOT(quit()));
    QTimer::singleShot(0, &mavPing, SLOT(run()));

    return a.exec();
}

