#ifndef SENSORCONFIGURATION_H
#define SENSORCONFIGURATION_H
#include <QString>
/*
 * Sensor configuration which defines:
 * power consumption
 * range
 * vendor name and additional information
 */
const QString SENSOR_IMG_FILE = ":/res/sensor.png";
const QString SENSOR_RX_IMG_FILE = ":/res/sensor_rx.png";
const QString SENSOR_TX_IMG_FILE = ":/res/sensor_tx.png";
const QString CLUSTER_IMG_FILE = ":/res/cluster.png";
const QString CLUSTER_RX_IMG_FILE = ":/res/cluster_rx.png";
const QString CLUSTER_TX_IMG_FILE = ":/res/cluster_tx.png";
const QString SINK_IMG_FILE = ":/res/sink.png";
const QString SINK_RX_IMG_FILE = ":/res/sink_rx.png";
const QString SINK_TX_IMG_FILE = ":/res/sink_tx.png";

const quint16 SINK_ID = UINT16_MAX;

const double IMG_SCALE = 0.5;

class NodeConfiguration
{
public:
    NodeConfiguration();
};

#endif // SENSORCONFIGURATION_H
