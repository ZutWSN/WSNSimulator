#include "Test_Widgets.h"
#include "widgetfactory.h"

void Test_Widgets::test_WidgetFactory()
{
    WidgetFactory wFactory;

    DragWidget *sensor = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sensor);
    DragWidget *cluster = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Cluster);
    DragWidget *sink = wFactory.getNewDragWidget(DragWidget::DragWidgetType::Sink);

    bool sensorCreated = (sensor->getWidgetType() == DragWidget::DragWidgetType::Sensor);
    bool clusterCreated = (cluster->getWidgetType() == DragWidget::DragWidgetType::Cluster);
    bool sinkCreated = (sink->getWidgetType() == DragWidget::DragWidgetType::Sink);

    qDebug("\nTest widgets creation if returns proper widget type\n");

    QVERIFY(sensorCreated == true);
    QVERIFY(clusterCreated == true);
    QVERIFY(sinkCreated == true);

    qDebug("\nTest returning wrong widget\n");
    DragWidget *widget = wFactory.getNewDragWidget(static_cast<DragWidget::DragWidgetType>(10));
    QVERIFY(widget == nullptr);
}
