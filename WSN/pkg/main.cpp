#include "sensorwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SensorWindow w(NULL, QSize(1350, 825));
    w.show();
    return a.exec();
}
