#include "SensorWindow.h"
#include <iostream>
#include <QPushButton>
#include "widgetfactory.h"
#include "NodeConfiguration.h"

SensorWindow::SensorWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumSize(200, 200);
    setAcceptDrops(true);

    initializeUiWidgets();
}

SensorWindow::~SensorWindow()
{

}

//cyclic event
void SensorWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain"))
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
    if (event->mimeData()->hasFormat("text/plain"))
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
    if (event->mimeData()->hasFormat("text/plain"))
    {
        QByteArray itemData = event->mimeData()->data("text/plain");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString text;
        QPoint offset;
        dataStream >> text >> offset;

        QLabel *newIcon = new QLabel(this);
        newIcon->setText(text);
        newIcon->move(event->pos() - offset);
        newIcon->show();
        newIcon->setAttribute(Qt::WA_DeleteOnClose);
        std::cout << "Drop Event position :  pos : x " << event->pos().x() << " y: " << event->pos().y() << "\n";
        std::cout << "Old position : x " << offset.x() << " y: " << offset.y() << "\n";

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
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if (child)
    {
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        std::cout << "Drag widgtet text : " << child->text().toStdString() << " pos : x " << child->pos().x() << " y: " << child->pos().y() << "\n";
        std::cout << "Press Event position :  pos : x " << event->pos().x() << " y: " << event->pos().y() << "\n";
        dataStream << child->text() << QPoint(event->pos() - child->pos());

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("text/plain", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setHotSpot(event->pos() - child->pos());

        //save old widget
        QLabel *oldIcon = new QLabel(this);
        oldIcon->setText(child->text());
        oldIcon->move(child->pos());
        oldIcon->show();
        oldIcon->setAttribute(Qt::WA_DeleteOnClose);

        if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
           child->close();
        } else {
           child->show();
        }
    }
}

void SensorWindow::pressedButton()
{

}

bool SensorWindow::initializeUiWidgets()
{
    //creating placeholders for ui widget layout
    // create newtork drag widgets
    WidgetFactory wFactory;

    DragWidget *sensor = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor, this);
    QPixmap sensorImg(SENSOR_IMG_FILE);
    sensor->setText("Sensor");
    sensor->setPixmap(sensorImg);
    sensor->move(10, 10);
    sensor->show();
    m_dragWidgets.push_back(sensor);

    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster, this);
    QPixmap clusterImg(CLUSTER_IMG_FILE);
    cluster->setPixmap(clusterImg);
    cluster->setText("Cluster");
    cluster->move(100, 10);
    sensor->show();
    m_dragWidgets.push_back(cluster);

    //add other widgets for network configuration and user input
    QPushButton *button = new QPushButton("Press", this);
    button->setBaseSize(10, 10);
    button->move(10, 100);
    button->show();
    button->setAttribute(Qt::WA_DeleteOnClose);
    connect(button, SIGNAL(released()),this, SLOT(pressedButton()));
}



