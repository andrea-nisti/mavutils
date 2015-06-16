#include <QCoreApplication>
#include <QTimer>
#include <QObject>

#include <MavLinkReader.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MavLinkReader mavReader;
    QObject::connect(&mavReader, SIGNAL(finished()), &a, SLOT(quit()));
    QTimer::singleShot(0, &mavReader, SLOT(run()));

    return a.exec();
}
