#include "dragwidget.h"

DragWidget::DragWidget(QWidget *parent, bool rootWidget):
    QLabel(parent),
    m_rootWidget(rootWidget)
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

void DragWidget::onNodeReceivedData(const DataFrame &data)
{
    //handle node received new data;
}




