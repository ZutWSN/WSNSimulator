#include "SensorWindow.h"
#include <iostream>
#include <QtWidgets>

SensorWindow::SensorWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumSize(200, 200);
    setAcceptDrops(true);

    QLabel *sensor = new QLabel("Sensor", this);
    sensor->move(10, 10);
    sensor->setBaseSize(10, 10);
    sensor->show();
    sensor->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *cluster = new QLabel("Cluster", this);
    cluster->setBaseSize(10, 10);
    cluster->move(100, 10);
    cluster->show();
    cluster->setAttribute(Qt::WA_DeleteOnClose);


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
        dataStream << child->text() << QPoint(event->pos() - child->pos());

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("text/plain", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setHotSpot(event->pos() - child->pos());

        if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
           child->close();
        } else {
           child->show();
        }
    }
}



