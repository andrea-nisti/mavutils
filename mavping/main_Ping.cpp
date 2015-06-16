#include <QCoreApplication>
#include <QTimer>
#include <QObject>

#include <MavPing.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MavPing mavPing;
    QObject::connect(&mavPing, SIGNAL(finished()), &a, SLOT(quit()));
    QTimer::singleShot(0, &mavPing, SLOT(run()));

    return a.exec();
}

