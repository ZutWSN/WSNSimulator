//i/o
#include <iostream>
//widgets
#include <QPushButton>
#include <QPainter>
#include <QColor>
#include <QPlainTextEdit>
//events
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
//drag&drop
#include <QDrag>
#include <QMimeData>

//foreign dependencies
#include "SensorWindow.h"
#include "widgetfactory.h"
#include "NodeConfiguration.h"
#include "SinkWidget.h"
#include "ClusterNode.h"
#include "SensorNode.h"

const QString MIME_DATA_FORMAT = "text/plain";
//Define widgets position - later create separate file containing wigets position configuration
//
const QPoint SENSOR_WIDGET_LABEL_POS = QPoint(60, 30);
const QSize SENSOR_WIDGET_LABEL_SIZE = QSize(50, 20);
const QPoint SENSOR_WIDGET_POS = QPoint(60, 50);
const QSize SENSOR_WIDGET_SIZE = QSize(80, 50);
//
const QPoint CLUSTER_WIDGET_LABEL_POS = QPoint(60, 130);
const QSize CLUSTER_WIDGET_LABEL_SIZE = QSize(50, 20);
const QPoint CLUSTER_WIDGET_POS = QPoint(60, 150);
const QSize CLUSTER_WIDGET_SIZE = QSize(80, 50);
//
const QPoint SINK_WIDGET_LABEL_POS = QPoint(60, 230);
const QSize SINK_WIDGET_LABEL_SIZE = QSize(50, 20);
const QPoint SINK_WIDGET_POS = QPoint(60, 250);
const QSize SINK_WIDGET_SIZE = QSize(80, 50);
//
const QPoint BTN_NETWORK_STATE_POS = QPoint(1110, 590);
const QSize BTN_NETWORK_STATE_SIZE = QSize(140, 30);
//
const QPoint BTN_SEND_MSG_POS = QPoint(300, 690);
const QSize BTN_SEND_MSG_SIZE = QSize(200, 30);
//
const QPoint BTN_SENSOR_BROADCAST_POS = QPoint(300, 730);
const QSize BTN_SENSOR_BROADCAST_SIZE = QSize(200, 30);
//
const QPoint BTN_REMOVE_NODE_POS = QPoint(670, 690);
const QSize BTN_REMOVE_NODE_SIZE = QSize(230, 30);
//
const QPoint LOG_WINDOW_LABEL_POS = QPoint(1030, 10);
const QSize LOG_WINDOW_LABEL_SIZE = QSize(80, 20);
const QPoint LOG_WINDOW_POS = QPoint(1030, 30);
const QSize LOG_WINDOW_SIZE = QSize(300, 500);
//
const QPoint NETWORK_AREA_LABEL_POS = QPoint(200, 10);
const QSize NETWORK_AREA_LABEL_SIZE = QSize(80, 20);
const QPoint NETWORK_AREA_WINDOW_POS = QPoint(200, 30);
const QSize NETWORK_AREA_WINDOW_SIZE = QSize(300, 500);
//
const QPoint INPUT_LABEL_POS = QPoint(60, 330);
const QSize INPUT_LABEL_SIZE = QSize(50, 20);
//
const QPoint INPUT_NODE_ID_LABEL_POS = QPoint(60, 380);
const QSize INPUT_NODE_ID_LABEL_SIZE = QSize(50, 20);
const QPoint INPUT_NODE_ID_POS = QPoint(60, 400);
const QSize INPUT_NODE_ID_SIZE = QSize(90, 30);
//
const QPoint INPUT_NODE_RANGE_LABEL_POS = QPoint(60, 430);
const QSize INPUT_NODE_RANGE_LABEL_SIZE = QSize(70, 20);
const QPoint INPUT_NODE_RANGE_POS = QPoint(60, 450);
const QSize INPUT_NODE_RANGE_SIZE = QSize(90, 30);
//
const QPoint DATA_TRANSFER_LABEL_POS = QPoint(200, 540);
const QSize DATA_TRANSFER_LABEL_SIZE = QSize(80, 20);
//
const QPoint CB_SENSOR_LIST_LABEL_POS = QPoint(200, 570);
const QSize CB_SENSOR_LIST_LABEL_SIZE = QSize(70, 20);
const QPoint CB_SENSOR_LIST_POS = QPoint(200, 590);
const QSize CB_SENSOR_LIST_SIZE = QSize(80, 30);
//
const QPoint SENSOR_MESSAGE_WINDOW_LABEL_POS = QPoint(300, 570);
const QSize SENSOR_MESSAGE_WINDOW_LABEL_SIZE = QSize(90, 20);
const QPoint SENSOR_MESSAGE_WINDOW_POS = QPoint(300, 590);
const QSize SENSOR_MESSAGE_WINDOW_SIZE = QSize(200, 80);
//
const QPoint NODE_INFO_LABEL_POS = QPoint(540, 540);
const QSize NODE_INFO_LABEL_SIZE = QSize(80, 20);
//
const QPoint INFO_NODE_ID_LABEL_POS = QPoint(540, 570);
const QSize INFO_NODE_ID_LABEL_SIZE = QSize(50, 20);
const QPoint INFO_NODE_ID_POS = QPoint(540, 590);
const QSize INFO_NODE_ID_SIZE = QSize(90, 30);
//
const QPoint INFO_NODE_RANGE_LABEL_POS = QPoint(540, 620);
const QSize INFO_NODE_RANGE_LABEL_SIZE = QSize(70, 20);
const QPoint INFO_NODE_RANGE_POS = QPoint(540, 640);
const QSize INFO_NODE_RANGE_SIZE = QSize(90, 30);
//
const QPoint INFO_NODE_STATE_LABEL_POS = QPoint(540, 670);
const QSize INFO_NODE_STATE_LABEL_SIZE = QSize(70, 20);
const QPoint INFO_NODE_STATE_POS = QPoint(540, 690);
const QSize INFO_NODE_STATE_SIZE = QSize(90, 30);
//
const QPoint LAST_MSG_LABEL_POS = QPoint(670, 570);
const QSize LAST_MSG_LABEL_SIZE = QSize(100, 20);
const QPoint LAST_MSG_WINDOW_POS = QPoint(670, 590);
const QSize LAST_MSG_WINDOW_SIZE = QSize(230, 80);
//
const QPoint BTN_SHOW_SINK_PATH_POS = QPoint(670, 690);
const QSize BTN_SHOW_SINK_PATH_SIZE = QSize(230, 30);

SensorWindow::SensorWindow(QWidget *parent, const QSize &windowSize) :
    QMainWindow(parent),
    m_dragWidgets(QVector<DragWidget*>()),
    m_sinkCreated(false),
    m_draw(false),
    m_id(0)
{
    setMinimumSize(windowSize);
    setAcceptDrops(true);
    setWindowTitle("WSN Simulator");

    initializeUiWidgets();
    m_sensorNetwork.reset(new SensorNetwork);
    //register for sensor network signals and slots
    connect(this, SIGNAL(addNewSink(QPoint,quint16,QWidget*)), m_sensorNetwork.data(), SLOT(onSinkAdded(QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(addNewCluster(quint16,quint16,QPoint,quint16,QWidget*)), m_sensorNetwork.data(), SLOT(onNewClusterAdded(quint16,quint16,QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(addNewSensor(quint16,quint16,QPoint,quint16,QWidget*)), m_sensorNetwork.data(), SLOT(onNewSensorAdded(quint16,quint16,QPoint,quint16,QWidget*)));
    connect(this, SIGNAL(removeSink()), m_sensorNetwork.data(), SLOT(onSinkRemoved()));
    connect(this, SIGNAL(removeNode(quint16,quint16)), m_sensorNetwork.data(), SLOT(onNodeRemoved(quint16,quint16)));
}

SensorWindow::~SensorWindow()
{
    for(DragWidget *widget : m_dragWidgets)
    {
        if(widget)
        {
            widget->close();
        }
    }
}

//cyclic event
void SensorWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(MIME_DATA_FORMAT))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void SensorWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(MIME_DATA_FORMAT))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void SensorWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(MIME_DATA_FORMAT))
    {
        QByteArray itemData = event->mimeData()->data(MIME_DATA_FORMAT);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPoint offset;
        QString imgName;
        bool isOldWidgetRootWidget = false;
        DragWidget::DragWidgetType widgetType;
        quint8 wType;
        quint16 node_id, layer_id;
        double range;
        dataStream  >> offset
                    >> imgName
                    >> isOldWidgetRootWidget
                    >> wType
                    >> node_id
                    >> layer_id
                    >> range;

        widgetType = static_cast<DragWidget::DragWidgetType>(wType);
        if(widgetType != DragWidget::DragWidgetType::NoType)
        {
            WidgetFactory wFactory;
            DragWidget *newWidget = wFactory.getNewDragWidget(widgetType, this, false);
            newWidget->setWidgetImage(imgName);
            newWidget->move(event->pos() - offset);
            newWidget->show();
            newWidget->setAttribute(Qt::WA_DeleteOnClose);
            //widget has been just moved, update the connected
            //widget to coresponding layer and node id
            //also alter the position of it
            bool isSink = false;
            switch(widgetType)
            {
                case DragWidget::DragWidgetType::Sink:
                    if(!isOldWidgetRootWidget)
                    {
                        emit removeSink();
                    }
                    emit addNewSink(newWidget->getPosition(), range, newWidget);
                    newWidget->setRange(range);
                    isSink = true;
                    break;
                case DragWidget::DragWidgetType::Cluster:
                    if(!isOldWidgetRootWidget)
                    {
                        emit removeNode(node_id, layer_id);
                    }
                    emit addNewCluster(node_id, layer_id, newWidget->getPosition(), range, newWidget);
                    newWidget->connectToNode(node_id, layer_id, range);
                    break;
                case DragWidget::DragWidgetType::Sensor:
                    if(!isOldWidgetRootWidget)
                    {
                        emit removeNode(node_id, layer_id);
                    }
                    emit addNewSensor(node_id, layer_id, newWidget->getPosition(), range, newWidget);
                    newWidget->connectToNode(node_id, layer_id, range);
                    break;
            }
            //update displayed paths - check all connections and edit linepaths
            redrawConnections();
            m_dragWidgets.push_back(newWidget);
            setNodeInfo(node_id, layer_id, range, isSink);
            connect(newWidget, SIGNAL(sendWidgetReceivedData(DataFrame,quint16,quint16)), this, SLOT(onWidgetReceivedData(DataFrame,quint16,quint16)));
            if (event->source() == this)
            {
                event->setDropAction(Qt::MoveAction);
                event->accept();
            }
            else
            {
                event->acceptProposedAction();
            }
        }
    }
    else
    {
        event->ignore();
    }
}

void SensorWindow::mousePressEvent(QMouseEvent *event)
{
    DragWidget *child = static_cast<DragWidget*>(childAt(event->pos()));
    if (child)
    {
        bool isSink = (child->getWidgetType() == DragWidget::DragWidgetType::Sink);
        bool drag = true;
        quint16 node_id = 0;
        quint16 layer_id = 0;
        double range = 0;
        if(child->isRootWidget())
        {
            if(isSink && m_sinkCreated)
            {
                drag = false;
            }
            if(!isSink || (isSink && !m_sinkCreated))
            {
                bool createNode = true;
                if(isSink)
                {
                    m_sinkCreated = true;
                    if(!getNodeRange(range))
                    {
                        createNode = false;
                        drag = false;
                    }
                }
                else
                {
                    if(!getNodeIDAndRange(node_id, range))
                    {
                        createNode = false;
                        drag = false;
                    }
                }
                if(createNode)
                {
                    WidgetFactory wFactory;
                    DragWidget *oldWidget = wFactory.getNewDragWidget(child->getWidgetType(), this, true);
                    oldWidget->setWidgetImage(child->getImageName());
                    oldWidget->move(child->pos());
                    oldWidget->show();
                    oldWidget->setAttribute(Qt::WA_DeleteOnClose);
                }
            }
        }
        else
        {
            node_id = child->getNodeID();
            layer_id = child->getLayerID();
            range = child->getNodeRange();
            int index = m_dragWidgets.indexOf(child);
            if(index >= 0)
            {
                m_dragWidgets.remove(index); //node is moved, this address will be no longer valid
            }
            setNodeInfo(node_id, layer_id, range, isSink);
        }
        if(drag)
        {
            quint8 type = static_cast<quint8>(child->getWidgetType());
            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);

            dataStream << QPoint(event->pos() - child->pos())
                       << child->getImageName()
                       << child->isRootWidget() //is old widget a root widget or not
                       << type
                       << node_id
                       << layer_id
                       << range;
            //change definition of mime data to be a drag widget getter
            QMimeData *mimeData = new QMimeData;
            mimeData->setData(MIME_DATA_FORMAT, itemData);

            QDrag *drag = new QDrag(this);
            drag->setPixmap(*child->pixmap()); //shows this during dragging of the object
            drag->setMimeData(mimeData);
            drag->setHotSpot(event->pos() - child->pos());

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
               child->close();
            }
            else
            {
               child->show();
            }
        }
    }
}

void SensorWindow::paintEvent(QPaintEvent *e)
{
    if(m_draw)
    {
        QPainter painter(this);
        QPen pen;
        pen.setWidth(3);
        for(auto && connection : m_networkConnectionLines)
        {
            pen.setColor(connection.lineColor);
            painter.setPen(pen);
            painter.drawLine(connection.line);
        }
        m_draw = false;
    }
}

void SensorWindow::pressedButton()
{

}

void SensorWindow::onPressedShowNetworkState()
{
    QString log;
    QPushButton *button = static_cast<QPushButton*>(m_btnShowNetworkState);
    if(button)
    {
        quint16 i = 0;
        for(QVector<NetworkLayer*>::const_iterator layer = m_sensorNetwork->getIteratorToFirstLayer(); i < m_sensorNetwork->getNumberOfLayers(); layer++)
        {
            ++i;
            log.append("\nLayer ID: " + QString::number((*layer)->getLayerId()) +
                                         "\nNumber of Nodes: " + QString::number((*layer)->getNumOfNodes()));
            quint16 j = 0;
            for(QVector<NetworkNode*>::const_iterator node = (*layer)->getIteratorToFirstNode(); j < (*layer)->getNumOfNodes(); node++)
            {
                //list all information about every layer and its nodes
                log.append("\n------------------------\n");
                ++j;
                log.append("\nNode Type:" + (*node)->getNodeTypeName() +
                                             "\nNode ID: " + QString::number((*node)->getNodeID()) +
                                             "\nConnected To Nodes: \n");
                for(auto && neighbour : (*node)->getNeighbours())
                {
                    NetworkNode *neighbourNode = (*layer)->getNode(neighbour.first);
                    if(neighbourNode)
                    {
                        log.append("\nNode Type: " + neighbourNode->getNodeTypeName() +
                                                     "\nNode ID: " + QString::number(neighbourNode->getNodeID()));
                    }
                }
                //node sink path
                if((*node)->getNodeType() == NetworkNode::NodeType::Cluster)
                {
                    ClusterNode *cluster = static_cast<ClusterNode*>((*node));
                    if(cluster)
                    {
                        if(cluster->getCurrentState() == ClusterNode::ClusterStates::CONNECTED ||
                           cluster->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK)
                        {
                            auto sinkPath = cluster->getSinkPath();
                            if(!sinkPath.isEmpty())
                            {
                                log.append("\nSink Path: ");
                                for(auto && pathNodeID : sinkPath)
                                {
                                    log.append(" " + QString::number(pathNodeID));
                                }
                            }
                            if(cluster->getPathLength() != INIT_PATH_LENGTH)
                            {
                                log.append("\nPath Length: " + QString::number(cluster->getPathLength()));
                            }
                        }
                        log.append("\nCluster State: " + cluster->getCurrentStateName());
                    }
                }
                else if((*node)->getNodeType() == NetworkNode::NodeType::Sensor)
                {
                    SensorNode *sensor = static_cast<SensorNode*>(*node);
                    if(sensor)
                    {
                        QString connected = (sensor->isConnectedToCluster()) ?  "Connected to cluster: " + QString::number(sensor->getClusterID()) : "Disconnected";
                        log.append(connected);
                    }
                }
            }
        }
        m_etxLogWindow->setPlainText(log);
    }
}

void SensorWindow::onPressedSendMsg()
{

}

void SensorWindow::onPressedRemoveNode()
{

}

void SensorWindow::onPressedSensorBroadcast()
{

}

void SensorWindow::onPressedShowSinkPath()
{

}

void SensorWindow::onWidgetReceivedData(const DataFrame &data, quint16 node_id, quint16 layer_id)
{
    //handle displaying newly received data
}

void SensorWindow::initializeUiWidgets()
{
    //user input
    QLabel *lblInput = new QLabel("Input:", this);
    lblInput->resize(INPUT_LABEL_SIZE);
    lblInput->move(INPUT_LABEL_POS);
    lblInput->show();
    lblInput->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblInputNodeID = new QLabel("Node ID:", this);
    lblInputNodeID->resize(INPUT_NODE_ID_LABEL_SIZE);
    lblInputNodeID->move(INPUT_NODE_ID_LABEL_POS);
    lblInputNodeID->show();
    lblInputNodeID->setAttribute(Qt::WA_DeleteOnClose);

    m_etxInputNodeID = new QPlainTextEdit(this);
    m_etxInputNodeID->resize(INPUT_NODE_ID_SIZE);
    m_etxInputNodeID->move(INPUT_NODE_ID_POS);
    m_etxInputNodeID->show();
    m_etxInputNodeID->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblInputNodeRange = new QLabel("Node Range:", this);
    lblInputNodeRange->resize(INPUT_NODE_RANGE_LABEL_SIZE);
    lblInputNodeRange->move(INPUT_NODE_RANGE_LABEL_POS);
    lblInputNodeRange->show();
    lblInputNodeRange->setAttribute(Qt::WA_DeleteOnClose);

    m_etxInputNodeRange = new QPlainTextEdit(this);
    m_etxInputNodeRange->resize(INPUT_NODE_RANGE_SIZE);
    m_etxInputNodeRange->move(INPUT_NODE_RANGE_POS);
    m_etxInputNodeRange->show();
    m_etxInputNodeRange->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblSensor = new QLabel("Sensor:", this);
    lblSensor->resize(SENSOR_WIDGET_LABEL_SIZE);
    lblSensor->move(SENSOR_WIDGET_LABEL_POS);
    lblSensor->show();
    lblSensor->setAttribute(Qt::WA_DeleteOnClose);

    WidgetFactory wFactory;
    DragWidget *sensor = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor, this, true);
    sensor->setWidgetImage(SENSOR_IMG_FILE);
    sensor->move(SENSOR_WIDGET_POS);
    sensor->show();
    sensor->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblCluster = new QLabel("Cluster:", this);
    lblCluster->resize(CLUSTER_WIDGET_LABEL_SIZE);
    lblCluster->move(CLUSTER_WIDGET_LABEL_POS);
    lblCluster->show();
    lblCluster->setAttribute(Qt::WA_DeleteOnClose);

    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster, this, true);
    cluster->setWidgetImage(CLUSTER_IMG_FILE);
    cluster->move(CLUSTER_WIDGET_POS);
    cluster->show();
    cluster->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblSink= new QLabel("Sink:", this);
    lblSink->resize(SINK_WIDGET_LABEL_SIZE);
    lblSink->move(SINK_WIDGET_LABEL_POS);
    lblSink->show();
    lblSink->setAttribute(Qt::WA_DeleteOnClose);

    DragWidget *sink = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sink, this, true);
    sink->setWidgetImage(SINK_IMG_FILE);
    sink->move(SINK_WIDGET_POS);
    sink->show();
    sink->setAttribute(Qt::WA_DeleteOnClose);

    //log window
    m_btnShowNetworkState = new QPushButton("Network State", this);
    m_btnShowNetworkState->resize(BTN_NETWORK_STATE_SIZE);
    m_btnShowNetworkState->move(BTN_NETWORK_STATE_POS);
    m_btnShowNetworkState->show();
    m_btnShowNetworkState->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_btnShowNetworkState, SIGNAL(pressed()), this, SLOT(onPressedShowNetworkState()));

    QLabel *lblLogWindow = new QLabel("Network Log:", this);
    lblLogWindow->resize(LOG_WINDOW_LABEL_SIZE);
    lblLogWindow->move(LOG_WINDOW_LABEL_POS);
    lblLogWindow->show();
    lblLogWindow->setAttribute(Qt::WA_DeleteOnClose);

    m_etxLogWindow = new QPlainTextEdit(this);
    m_etxLogWindow->setReadOnly(true);
    m_etxLogWindow->resize(LOG_WINDOW_SIZE);
    m_etxLogWindow->move(LOG_WINDOW_POS);
    m_etxLogWindow->show();
    m_etxLogWindow->setAttribute(Qt::WA_DeleteOnClose);

    //data transfer
    QLabel *lblDataTransfer = new QLabel("Data Transfer:", this);
    lblDataTransfer->resize(DATA_TRANSFER_LABEL_SIZE);
    lblDataTransfer->move(DATA_TRANSFER_LABEL_POS);
    lblDataTransfer->show();
    lblDataTransfer->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblSensorList = new QLabel("Sensor List:", this);
    lblSensorList->resize(CB_SENSOR_LIST_LABEL_SIZE);
    lblSensorList->move(CB_SENSOR_LIST_LABEL_POS);
    lblSensorList->show();
    lblSensorList->setAttribute(Qt::WA_DeleteOnClose);

    m_cbSensorList = new QComboBox(this);
    m_cbSensorList->resize(CB_SENSOR_LIST_SIZE);
    m_cbSensorList->move(CB_SENSOR_LIST_POS);
    m_cbSensorList->show();
    m_cbSensorList->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblSensorMsg = new QLabel("Sensor Message:", this);
    lblSensorMsg->resize(SENSOR_MESSAGE_WINDOW_LABEL_SIZE);
    lblSensorMsg->move(SENSOR_MESSAGE_WINDOW_LABEL_POS);
    lblSensorMsg->show();
    lblSensorMsg->setAttribute(Qt::WA_DeleteOnClose);

    m_etxSensorMsg = new QPlainTextEdit(this);
    m_etxSensorMsg->resize(SENSOR_MESSAGE_WINDOW_SIZE);
    m_etxSensorMsg->move(SENSOR_MESSAGE_WINDOW_POS);
    m_etxSensorMsg->show();
    m_etxSensorMsg->setAttribute(Qt::WA_DeleteOnClose);

    m_btnSendMsg = new QPushButton("Send Message", this);
    m_btnSendMsg->resize(BTN_SEND_MSG_SIZE);
    m_btnSendMsg->move(BTN_SEND_MSG_POS);
    m_btnSendMsg->show();
    m_btnSendMsg->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_btnSendMsg, SIGNAL(pressed()), this, SLOT(onPressedSendMsg()));

    m_btnSensorBroadcast = new QPushButton("Start Sensor Broadcast", this);
    m_btnSensorBroadcast->resize(BTN_SENSOR_BROADCAST_SIZE);
    m_btnSensorBroadcast->move(BTN_SENSOR_BROADCAST_POS);
    m_btnSensorBroadcast->show();
    m_btnSensorBroadcast->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_btnSensorBroadcast, SIGNAL(pressed()), this, SLOT(onPressedSensorBroadcast()));

    //node info
    QLabel *lblNodeInfo = new QLabel("Node Info:", this);
    lblNodeInfo->resize(NODE_INFO_LABEL_SIZE);
    lblNodeInfo->move(NODE_INFO_LABEL_POS);
    lblNodeInfo->show();
    lblNodeInfo->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblNodeIDInfo = new QLabel("Node ID:", this);
    lblNodeIDInfo->resize(INFO_NODE_ID_LABEL_SIZE);
    lblNodeIDInfo->move(INFO_NODE_ID_LABEL_POS);
    lblNodeIDInfo->show();
    lblNodeIDInfo->setAttribute(Qt::WA_DeleteOnClose);

    m_etxNodeIDInfo = new QPlainTextEdit(this);
    m_etxNodeIDInfo->setReadOnly(true);
    m_etxNodeIDInfo->resize(INFO_NODE_ID_SIZE);
    m_etxNodeIDInfo->move(INFO_NODE_ID_POS);
    m_etxNodeIDInfo->show();
    m_etxNodeIDInfo->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblNodeRangeInfo = new QLabel("Node Range:", this);
    lblNodeRangeInfo->resize(INFO_NODE_RANGE_LABEL_SIZE);
    lblNodeRangeInfo->move(INFO_NODE_RANGE_LABEL_POS);
    lblNodeRangeInfo->show();
    lblNodeRangeInfo->setAttribute(Qt::WA_DeleteOnClose);

    m_etxNodeRangeInfo = new QPlainTextEdit(this);
    m_etxNodeRangeInfo->setReadOnly(true);
    m_etxNodeRangeInfo->resize(INFO_NODE_RANGE_SIZE);
    m_etxNodeRangeInfo->move(INFO_NODE_RANGE_POS);
    m_etxNodeRangeInfo->show();
    m_etxNodeRangeInfo->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblNodeStateInfo = new QLabel("Node State:", this);
    lblNodeStateInfo->resize(INFO_NODE_STATE_LABEL_SIZE);
    lblNodeStateInfo->move(INFO_NODE_STATE_LABEL_POS);
    lblNodeStateInfo->show();
    lblNodeStateInfo->setAttribute(Qt::WA_DeleteOnClose);

    m_etxNodeStateInfo = new QPlainTextEdit(this);
    m_etxNodeStateInfo->setReadOnly(true);
    m_etxNodeStateInfo->resize(INFO_NODE_STATE_SIZE);
    m_etxNodeStateInfo->move(INFO_NODE_STATE_POS);
    m_etxNodeStateInfo->show();
    m_etxNodeStateInfo->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *lblLastRxMsg = new QLabel("Last Received Data:", this);
    lblLastRxMsg->resize(LAST_MSG_LABEL_SIZE);
    lblLastRxMsg->move(LAST_MSG_LABEL_POS);
    lblLastRxMsg->show();
    lblLastRxMsg->setAttribute(Qt::WA_DeleteOnClose);

    m_etxLastRxMsg = new QPlainTextEdit(this);
    m_etxLastRxMsg->setReadOnly(true);
    m_etxLastRxMsg->resize(LAST_MSG_WINDOW_SIZE);
    m_etxLastRxMsg->move(LAST_MSG_WINDOW_POS);
    m_etxLastRxMsg->show();
    m_etxLastRxMsg->setAttribute(Qt::WA_DeleteOnClose);

    m_btnShowSinkPath = new QPushButton("Show Sink Path", this);
    m_btnShowSinkPath->resize(BTN_SHOW_SINK_PATH_SIZE);
    m_btnShowSinkPath->move(BTN_SHOW_SINK_PATH_POS);
    m_btnShowSinkPath->show();
    m_btnShowSinkPath->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_btnShowSinkPath, SIGNAL(pressed()), this, SLOT(onPressedShowSinkPath()));

}

void SensorWindow::redrawConnections()
{
    //check duplicates ? (does nothing bad and not have to create separate container for storing visited nodes
    m_networkConnectionLines.clear();
    quint16 i = 0;
    for(QVector<NetworkLayer*>::const_iterator layer = m_sensorNetwork->getIteratorToFirstLayer(); i < m_sensorNetwork->getNumberOfLayers(); layer++)
    {
        ++i;
        if(*layer)
        {
            quint16 j = 0;
            for(QVector<NetworkNode*>::const_iterator node = (*layer)->getIteratorToFirstNode(); j < (*layer)->getNumOfNodes(); node++)
            {
                ++j;
                if(*node)
                {
                    for(auto && neighbour : (*node)->getNeighbours())
                    {
                        NetworkNode *neighbourNode = (*layer)->getNode(neighbour.first);
                        if(neighbourNode)
                        {
                            QLine line((*node)->getNodePosition(), neighbourNode->getNodePosition());
                            QColor connectionTypeColor = QColor(0, 0, 0);
                            if((*node)-> getNodeType() == NetworkNode::NodeType::Cluster && neighbourNode->getNodeType() == NetworkNode::NodeType::Cluster)
                            {
                                connectionTypeColor = MAPPED_CLUSTER_COLOR;
                            }
                            else if(((*node)-> getNodeType() == NetworkNode::NodeType::Cluster && neighbourNode->getNodeType() == NetworkNode::NodeType::Sensor) ||
                                    ((*node)-> getNodeType() == NetworkNode::NodeType::Sensor && neighbourNode->getNodeType() == NetworkNode::NodeType::Cluster))
                            {
                                connectionTypeColor = SENSOR_COLOR;
                            }
                            LineConnection connection{line, connectionTypeColor};
                            m_networkConnectionLines.push_back(connection);
                        }
                    }
                    //check if connected to sink and then add line connection
                    if(m_sensorNetwork->networkHasSink())
                    {
                        if((*node)->getNodeType() == NetworkNode::NodeType::Cluster)
                        {
                            ClusterNode *cluster = static_cast<ClusterNode*>(*node);
                            if(cluster)
                            {
                                if(cluster->getCurrentState() == ClusterNode::ClusterStates::CONNECTED_TO_SINK)
                                {
                                    QLine line((*node)->getNodePosition(), m_sensorNetwork->getSinkPosition());
                                    LineConnection sinkConnection{line, DIRECT_CLUSTER_COLOR};
                                    m_networkConnectionLines.push_back(sinkConnection);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    m_draw = true;
    update();
}

void SensorWindow::drawConnection()
{

}

void SensorWindow::setNodeInfo(quint16 node_id, quint16 layer_id, quint16 range, bool isSink)
{
    //display clicked node status
    QString nodeIDInfo, nodeRange, lastMsg, status;
    if(isSink)
    {
        nodeIDInfo = "Sink Node";
        nodeRange = QString::number(range);
        lastMsg = m_sensorNetwork->getSinkLastMsg();
    }
    else
    {
        NetworkLayer *layer = m_sensorNetwork->getLayer(layer_id);
        if(layer)
        {
            NetworkNode *node = layer->getNode(node_id);
            if(node)
            {
                nodeIDInfo = QString::number(node_id);
                nodeRange = QString::number(range);
                lastMsg = node->getLastReceivedMsg();
                if(node->getNodeType() == NetworkNode::NodeType::Cluster)
                {
                    ClusterNode *cluster = static_cast<ClusterNode*>(node);
                    if(cluster)
                    {
                        status = QString("Cluster : ") + cluster->getCurrentStateName();
                    }
                }
                else if(node->getNodeType() == NetworkNode::NodeType::Sensor)
                {
                    SensorNode *sensor = static_cast<SensorNode*>(node);
                    if(sensor)
                    {
                        QString sensorState = (sensor->isConnectedToCluster()) ? "Connected" : "Disconnected";
                        status = QString("Sensor : ") + sensorState;
                    }
                }
            }

        }
    }
    m_etxNodeIDInfo->setPlainText(nodeIDInfo);
    m_etxNodeRangeInfo->setPlainText(nodeRange);
    m_etxLastRxMsg->setPlainText(lastMsg);
    m_etxNodeStateInfo->setPlainText(status);
}

bool SensorWindow::getNodeIDAndRange(quint16 &node_id, double &node_range) const
{
    bool success = false;
    bool noError = false;
    quint16 id = m_etxInputNodeID->toPlainText().toUInt(&noError);
    if(noError)
    {
        if(NetworkLayer::checkIfIdAvailable(id))
        {
            double range = m_etxInputNodeRange->toPlainText().toDouble(&noError);
            if(noError && range > 0)
            {
                node_id = id;
                node_range = range;
                success = true;
            }
        }
    }
    return success;
}

bool SensorWindow::getNodeID(quint16 &node_id) const
{
    bool success = false;
    bool noError = false;
    quint16 id = m_etxInputNodeID->toPlainText().toUInt(&noError);
    if(noError)
    {
        if(NetworkLayer::checkIfIdAvailable(id))
        {
            node_id = id;
            success = true;
        }
    }
    return success;
}

bool SensorWindow::getNodeRange(double &node_range) const
{
    bool success = false;
    bool noError = false;
    double range = m_etxInputNodeRange->toPlainText().toDouble(&noError);
    if(noError && range > 0)
    {
        node_range = range;
        success = true;
    }
    return success;
}



