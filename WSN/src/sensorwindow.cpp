#include "SensorWindow.h"
#include <iostream>
#include <QPushButton>
#include "widgetfactory.h"
#include "NodeConfiguration.h"
#include <QDrag>

const QString MIME_DATA_FORMAT = "text/plain";

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
    if (event->mimeData()->hasFormat(MIME_DATA_FORMAT))
    {
        QByteArray itemData = event->mimeData()->data(MIME_DATA_FORMAT);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString text = nullptr;
        QPoint offset = nullptr;
        QString imgName = nullptr;
        dataStream >> text >> offset >> imgName;

        DragWidget *newWidget = new DragWidget(this);
        newWidget->setWidgetImage(imgName);
        newWidget->setText(text);
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

        dataStream << child->text() << QPoint(event->pos() - child->pos()) << child->getImageName();

        //change definition of mime data to be a drag widget getter
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(MIME_DATA_FORMAT, itemData);

        QDrag *drag = new QDrag(this);
        drag->setPixmap(child->pixmap()); //for drag to show temp object
        drag->setMimeData(mimeData);
        drag->setHotSpot(event->pos() - child->pos());

        //save old widget
        DragWidget *oldIcon = new DragWidget(this);
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

bool SensorWindow::initializeUiWidgets()
{
    //creating placeholders for ui widget layout
    // create newtork drag widgets
    WidgetFactory wFactory;

    DragWidget *sensor = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor, this);
    sensor->setText("Sensor");
    sensor->setWidgetImage(SENSOR_IMG_FILE);
    sensor->move(10, 10);
    sensor->show();
    m_dragWidgets.push_back(sensor);

    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster, this);
    cluster->setText("Cluster");
    sensor->setWidgetImage(CLUSTER_IMG_FILE);
    cluster->move(100, 10);
    sensor->show();
    m_dragWidgets.push_back(cluster);

    //add other widgets for network configuration and user input
    m_pressButton = new QPushButton("Press", this);
    m_pressButton->setBaseSize(10, 10);
    m_pressButton->move(10, 100);
    m_pressButton->show();
    m_pressButton->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_pressButton, SIGNAL(released()),this, SLOT(pressedButton()));
}



