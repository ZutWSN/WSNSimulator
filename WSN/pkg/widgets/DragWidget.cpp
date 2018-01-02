#include "dragwidget.h"
#include "NetworkLayer.h"
#include <QTimer>

const quint16 STATE_TIMER_TIMEOUT = 500;
const quint16 STATE_TIMER_RESET_TIMEOUT = 2000;

DragWidget::DragWidget(QWidget *parent, bool rootWidget):
    QLabel(parent),
    m_rootWidget(rootWidget),
    m_connectedToNode(false),
    m_widgetSize(QSize(0, 0)),
    m_imgScale(0.5)
{
    QWidget::setWhatsThis(DRAG_WIDGET_LABEL);

    m_rxStateTimer = new QTimer(this);
    m_rxStateTimer->setSingleShot(true);
    connect(m_rxStateTimer, SIGNAL(timeout()), this, SLOT(onStateUpdate()));

    m_txStateTimer = new QTimer(this);
    m_txStateTimer->setSingleShot(true);
    connect(m_txStateTimer, SIGNAL(timeout()), this, SLOT(onStateUpdate()));

    m_stateResetTimer = new QTimer(this);
    m_stateResetTimer->setSingleShot(true);
    connect(m_stateResetTimer, SIGNAL(timeout()), this, SLOT(onStateUpdate()));
    m_events = QVector<bool>{true, false, false};
}

DragWidget::~DragWidget()
{

}

void DragWidget::setWidgetImage(const QString &imgName, ImageType imgType, double scale)
{
    if(scale > 0)
    {
        QPixmap img(imgName);
        img = img.scaled(img.size() * scale);
        switch(imgType)
        {
            case ImageType::DEFAULT:
                setMinimumSize(QSize(img.width(), img.height()));
                setMaximumSize(QSize(img.width(), img.height()));
                m_widgetSize = QSize(img.width(), img.height());
                m_imgScale = scale;
                m_defaultImgName = imgName;
                QLabel::setPixmap(img);
                break;
            case ImageType::RECEIVED_DATA:
                if(!m_defaultImgName.isEmpty())
                {
                    if(img.size() == m_widgetSize && scale == m_imgScale)
                    {
                        m_rxDataImgName = imgName;
                    }
                }
                break;
            case ImageType::SEND_DATA:
                if(!m_defaultImgName.isEmpty())
                {
                    if(img.size() == m_widgetSize && scale == m_imgScale)
                    {
                        m_txDataImgName = imgName;
                    }
                }
                break;
        }
    }
}

void DragWidget::setConnectedToNode(bool connected)
{
    m_connectedToNode = connected;
}

void DragWidget::setNodeID(quint16 node_id)
{
    if(NetworkLayer::checkIfIdAvailable(node_id))
    {
        m_node_id = node_id;
    }
}

void DragWidget::setRange(double range)
{
    if(range > 0)
    {
        m_range = range;
    }
}

void DragWidget::setNodeLayer(quint16 layer_id)
{
    m_layer_id = layer_id;
}

void DragWidget::connectToNode(quint16 node_id, quint16 layer_id, double range)
{
    m_connectedToNode = true;
    m_node_id = node_id;
    m_layer_id = layer_id;
    m_range = range;
}

bool DragWidget::isRootWidget() const
{
    return m_rootWidget;
}

DragWidget::DragWidgetType DragWidget::getWidgetType() const
{
    return DragWidgetType::NoType;
}

QString DragWidget::getImageName(ImageType imgType) const
{
    QString imgName;
    switch(imgType)
    {
        case ImageType::DEFAULT:
            imgName = m_defaultImgName;
            break;
        case ImageType::RECEIVED_DATA:
            imgName = m_rxDataImgName;
            break;
        case ImageType::SEND_DATA:
            imgName = m_txDataImgName;
            break;
    }
    return imgName;
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

QPoint DragWidget::getPosition() const
{
    QPoint widgetCenter = QWidget::pos();
    widgetCenter.setX(widgetCenter.x() + (m_widgetSize.width() / 2));
    widgetCenter.setY(widgetCenter.y() + (m_widgetSize.height() / 2));
    return widgetCenter;
}

QSize DragWidget::getWidgetSize() const
{
    return m_widgetSize;
}

double DragWidget::getImgScale() const
{
    return m_imgScale;
}

void DragWidget::processReceivedData(const DataFrame &data)
{
    m_events[1] = true;
    m_rxStateTimer->start(STATE_TIMER_TIMEOUT);
}

void DragWidget::processDataSend(const DataFrame &data)
{
    m_events[2] = true;
    m_txStateTimer->start(STATE_TIMER_TIMEOUT);
}

void DragWidget::onNodeReceivedData(const DataFrame &data)
{
    processReceivedData(data);
}

void DragWidget::onNodeSendData(const DataFrame &data)
{
    processDataSend(data);
}

void DragWidget::onStateUpdate()
{
    if(m_events[1] && !m_events[2])
    {
        QPixmap img(m_rxDataImgName);
        if(!img.isNull())
        {
            img = img.scaled(img.size() * m_imgScale);
            setMinimumSize(QSize(img.width(), img.height()));
            setMaximumSize(QSize(img.width(), img.height()));
            QLabel::setPixmap(img);
        }
        m_events[1] = false;
        m_stateResetTimer->start(STATE_TIMER_RESET_TIMEOUT);
    }
    else if(m_events[2] && !m_events[1])
    {
        QPixmap img(m_txDataImgName);
        if(!img.isNull())
        {
            img = img.scaled(img.size() * m_imgScale);
            setMinimumSize(QSize(img.width(), img.height()));
            setMaximumSize(QSize(img.width(), img.height()));
            QLabel::setPixmap(img);
        }
        m_events[2] = false;
        m_stateResetTimer->start(STATE_TIMER_RESET_TIMEOUT);
    }
    else if(m_events[1] && m_events[2])
    {
        if(m_txStateTimer->isActive())
        {
            //rx was first reset tx timer and set rx as current img
            QPixmap img(m_rxDataImgName);
            if(!img.isNull())
            {
                img = img.scaled(img.size() * m_imgScale);
                setMinimumSize(QSize(img.width(), img.height()));
                setMaximumSize(QSize(img.width(), img.height()));
                QLabel::setPixmap(img);
            }
            m_events[1] = false;
            m_txStateTimer->start(STATE_TIMER_TIMEOUT);
            m_stateResetTimer->start(STATE_TIMER_RESET_TIMEOUT);
        }
        else if(m_rxStateTimer->isActive())
        {
            //tx was first reset rx timer and set tx as current img
            QPixmap img(m_txDataImgName);
            if(!img.isNull())
            {
                img = img.scaled(img.size() * m_imgScale);
                setMinimumSize(QSize(img.width(), img.height()));
                setMaximumSize(QSize(img.width(), img.height()));
                QLabel::setPixmap(img);
            }
            m_events[2] = false;
            m_rxStateTimer->start(STATE_TIMER_TIMEOUT);
            m_stateResetTimer->start(STATE_TIMER_RESET_TIMEOUT);
        }
    }
    else if(!m_events[1] && !m_events[2])
    {
        QPixmap img(m_defaultImgName);
        if(!img.isNull())
        {
            img = img.scaled(img.size() * m_imgScale);
            setMinimumSize(QSize(img.width(), img.height()));
            setMaximumSize(QSize(img.width(), img.height()));
            QLabel::setPixmap(img);
        }
        m_stateResetTimer->start(STATE_TIMER_RESET_TIMEOUT);
    }

}




