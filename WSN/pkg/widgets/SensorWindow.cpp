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
const QPoint SENSOR_WIDGET_POS = QPoint(10, 10);
const QPoint CLUSTER_WIDGET_POS = QPoint(10, 100);
const QPoint SINK_WIDGET_POS = QPoint(10, 200);
const QPoint BUTTON_POS = QPoint(400, 400);
const QPoint LOG_WINDOW_POS = QPoint(450, 100);

SensorWindow::SensorWindow(QWidget *parent, const QSize &windowSize) :
    QMainWindow(parent),
    m_dragWidgets(QVector<DragWidget*>()),
    m_sinkCreated(false),
    m_draw(false),
    m_id(0)
{
    setMinimumSize(windowSize);
    setAcceptDrops(true);

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
            switch(widgetType)
            {
                case DragWidget::DragWidgetType::Sink:
                    if(!isOldWidgetRootWidget)
                    {
                        emit removeSink();
                    }
                    emit addNewSink(newWidget->getPosition(), range, newWidget);

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
            if(isOldWidgetRootWidget)
            {
                ++m_id;
            }
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
        double range = 100; // later get from coresponding text edit
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
                }
                else
                {
                    //temporary - later node id and layer will be taken here from
                    //text edit widgets
                    if(!NetworkLayer::checkIfIdAvailable(m_id))
                    {
                        createNode = false;
                        drag = false;
                    }
                    else
                    {
                        node_id = m_id;
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
    QString log;
    QPushButton *button = static_cast<QPushButton*>(m_pressButton);
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
        m_logWindow->setPlainText(log);
    }
}

void SensorWindow::onWidgetReceivedData(const DataFrame &data, quint16 node_id, quint16 layer_id)
{
    //handle displaying newly received data
}

void SensorWindow::initializeUiWidgets()
{
    //creating placeholders for ui widget layout
    // create newtork drag widgets
    WidgetFactory wFactory;

    DragWidget *sensor = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor, this, true);
    sensor->setWidgetImage(SENSOR_IMG_FILE);
    sensor->move(SENSOR_WIDGET_POS);
    sensor->show();
    sensor->setAttribute(Qt::WA_DeleteOnClose);

    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster, this, true);
    cluster->setWidgetImage(CLUSTER_IMG_FILE);
    cluster->move(CLUSTER_WIDGET_POS);
    cluster->show();
    cluster->setAttribute(Qt::WA_DeleteOnClose);

    DragWidget *sink = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sink, this, true);
    sink->setWidgetImage(SINK_IMG_FILE);
    sink->move(SINK_WIDGET_POS);
    sink->show();
    sink->setAttribute(Qt::WA_DeleteOnClose);

    //add other widgets for network configuration and user input
    m_pressButton = new QPushButton("Show Network Info", this);
    m_pressButton->move(BUTTON_POS);
    m_pressButton->show();
    m_pressButton->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_pressButton, SIGNAL(pressed()), this, SLOT(pressedButton()));

    m_logWindow = new QPlainTextEdit(this);
    m_logWindow->setReadOnly(true);
    m_logWindow->resize(300, 600);
    m_logWindow->move(LOG_WINDOW_POS);
    m_logWindow->show();
    m_logWindow->setAttribute(Qt::WA_DeleteOnClose);
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



