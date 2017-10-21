#ifndef SENSORCONFIGURATION_H
#define SENSORCONFIGURATION_H
#include <QString>
/*
 * Sensor configuration which defines:
 * power consumption
 * range
 * vendor name and additional information
 */
const QString SENSOR_IMG_FILE = ":/res/sensor.jpg";
const QString CLUSTER_IMG_FILE = ":/res/cluster.jpg";

class NodeConfiguration
{
public:
    NodeConfiguration();
};

#endif // SENSORCONFIGURATION_H