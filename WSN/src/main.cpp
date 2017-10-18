#include "sensorwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SensorWindow w;
    w.show();

    return a.exec();
}
