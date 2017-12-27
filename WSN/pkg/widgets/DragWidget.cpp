#include "dragwidget.h"
#include "NetworkLayer.h"

DragWidget::DragWidget(QWidget *parent, bool rootWidget):
    QLabel(parent),
    m_rootWidget(rootWidget),
    m_connectedToNode(false)
{

}

DragWidget::~DragWidget()
{

}

void DragWidget::setWidgetImage(const QString &imgName)
{
    QPixmap img(imgName);
    setMinimumSize(QSize(img.width(), img.height()));
    setMaximumSize(QSize(img.width(), img.height()));
    QLabel::setPixmap(img);
    m_imgName = imgName;
}

void DragWidget::setConnectedToNode(bool connected)
{
    m_connectedToNode = connected;
}

bool DragWidget::connectToNode(quint16 node_id, quint16 layer_id, double range)
{
    if(NetworkLayer::checkIfIdAvailable(node_id))
    {
        m_connectedToNode = true;
        m_node_id = node_id;
        m_layer_id = layer_id;
        m_range = range;
    }
}

bool DragWidget::isRootWidget() const
{
    return m_rootWidget;
}

DragWidget::DragWidgetType DragWidget::getWidgetType() const
{
    return DragWidgetType::NoType;
}

QString DragWidget::getImageName() const
{
    return m_imgName;
}

bool DragWidget::isConnectedToNode() const
{
    return m_connectedToNode;
}

quint16 DragWidget::getNodeID() const
{
    return m_node_id;
}

quint16 DragWidget::getLayerID() const
{
    return m_layer_id;
}

double DragWidget::getNodeRange() const
{
    return m_range;
}

void DragWidget::onNodeReceivedData(const DataFrame &data)
{
    switch(data.getMsgType())
    {
        emit sendWidgetReceivedData(data, m_node_id, m_layer_id);
    }

    //handle node received new data;
    //notify sensorwindow parent widget
    //signal its slot which later will call its paintevent update function
}




