//foreign dependencies
#include "SensorWindow.h"
#include "widgetfactory.h"
#include "NodeConfiguration.h"
//i/o
#include <iostream>
//widgets
#include <QPushButton>
//events
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
//drag&drop
#include <QDrag>
#include <QMimeData>

const QString MIME_DATA_FORMAT = "text/plain";

SensorWindow::SensorWindow(QWidget *parent, const QSize &windowSize) :
    QMainWindow(parent),
    m_dragWidgets(QVector<DragWidget*>())
{
    setMinimumSize(windowSize);
    setAcceptDrops(true);

    initializeUiWidgets();
}

SensorWindow::~SensorWindow()
{

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
        dataStream >> offset >> imgName;

        DragWidget *newWidget = new DragWidget(this, false);
        newWidget->setWidgetImage(imgName);
        newWidget->move(event->pos() - offset);
        newWidget->show();
        newWidget->setAttribute(Qt::WA_DeleteOnClose);

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

void SensorWindow::mousePressEvent(QMouseEvent *event)
{
    DragWidget *child = static_cast<DragWidget*>(childAt(event->pos()));
    if (child)
    {
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);

        dataStream << QPoint(event->pos() - child->pos()) << child->getImageName();

        //change definition of mime data to be a drag widget getter
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(MIME_DATA_FORMAT, itemData);

        QDrag *drag = new QDrag(this);
        drag->setPixmap(*child->pixmap()); //shows this during dragging of the object
        drag->setMimeData(mimeData);
        drag->setHotSpot(event->pos() - child->pos());

        if(child->isRootWidget())
        {
            //save old widget
            DragWidget *oldWidget = new DragWidget(this, true);
            oldWidget->setWidgetImage(child->getImageName());
            oldWidget->move(child->pos());
            oldWidget->show();
            oldWidget->setAttribute(Qt::WA_DeleteOnClose);
        }

        if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
           child->close();
        } else {
           child->show();
        }
    }
}

void SensorWindow::pressedButton()
{
    QPushButton *button = static_cast<QPushButton*>(m_pressButton);
    if(button)
    {
        if(button->text() == "Press")
        {
            button->setText("Pressed");
        }
        else
        {
            button->setText("Press");
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
    sensor->move(10, 10);
    sensor->show();
    sensor->setAttribute(Qt::WA_DeleteOnClose);
    m_dragWidgets.push_back(sensor);

    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster, this, true);
    cluster->setWidgetImage(CLUSTER_IMG_FILE);
    cluster->move(10, 100);
    cluster->show();
    cluster->setAttribute(Qt::WA_DeleteOnClose);
    m_dragWidgets.push_back(cluster);

    //add other widgets for network configuration and user input
    m_pressButton = new QPushButton("Press", this);
    m_pressButton->move(500, 500);
    m_pressButton->show();
    m_pressButton->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_pressButton, SIGNAL(pressed()), this, SLOT(pressedButton()));
}



