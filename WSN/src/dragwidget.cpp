#include "dragwidget.h"

DragWidget::DragWidget(QWidget *parent)
{

}

DragWidget::~DragWidget()
{

}

void DragWidget::setWidgetImage(const QString &imgName)
{
    QLabel.setPixmap(imgName);
    m_imgName = imgName;
}

QString DragWidget::getImageName()
{
    return m_imgName;
}




