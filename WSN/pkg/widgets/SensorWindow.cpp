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

const QString MIME_DATA_FORMAT = "text/plain";
//Define widgets position - later create separate file containing wigets position configuration
const QPoint SENSOR_WIDGET_POS = QPoint(10, 10);
const QPoint CLUSTER_WIDGET_POS = QPoint(10, 100);
const QPoint SINK_WIDGET_POS = QPoint(10, 200);
const QPoint BUTTON_POS = QPoint(400, 400);
const QPoint LOG_WINDOW_POS = QPoint(600, 100);

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
                    emit addNewSink(newWidget->pos(), range, newWidget);
                    break;
                case DragWidget::DragWidgetType::Cluster:
                    if(!isOldWidgetRootWidget)
                    {
                        emit removeNode(node_id, layer_id);
                    }
                    emit addNewCluster(node_id, layer_id, newWidget->pos(), range, newWidget);
                    newWidget->connectToNode(node_id, layer_id, range);
                    break;
                case DragWidget::DragWidgetType::Sensor:
                    if(!isOldWidgetRootWidget)
                    {
                        emit removeNode(node_id, layer_id);
                    }
                    emit addNewSensor(node_id, layer_id, newWidget->pos(), range, newWidget);
                    newWidget->connectToNode(node_id, layer_id, range);
                    break;
            }
            m_dragWidgets.push_back(newWidget);
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
        double range = 30; // later get from coresponding text edit
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
                ++m_id;
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
    //draw test line
    QLine line;
    QPen pen;
    if(m_draw)
    {
        line.setLine(0, 0, 100, 100);
        pen.setColor(MESSAGE_RECEIVED_COLOR);

        pen.setWidth(4);
        QPainter painter(this);
        painter.setPen(pen);
        painter.drawLine(line);
        m_draw = false;
    }
}

void SensorWindow::pressedButton()
{
    QPushButton *button = static_cast<QPushButton*>(m_pressButton);
    if(button)
    {
        quint16 i = 0;
        for(QVector<NetworkLayer*>::const_iterator layer = m_sensorNetwork->getIteratorToFirstLayer(); i < m_sensorNetwork->getNumberOfLayers(); layer++)
        {
            ++i;
            for(QVector<NetworkNode*>::const_iterator node = (*layer)->getIteratorToFirstNode(); i < (*layer)->getNumOfNodes(); node++)
            {
                //list all information about every layer and its nodes
            }
        }

    }
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
    m_logWindow->resize(100, 600);
    m_logWindow->move(LOG_WINDOW_POS);
    m_logWindow->show();
    m_logWindow->setAttribute(Qt::WA_DeleteOnClose);
}

void SensorWindow::redrawConnections()
{

}

void SensorWindow::drawConnection()
{

}



