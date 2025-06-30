#include "m_graphicsview.h"

m_GraphicsView::m_GraphicsView(QWidget* parent)
    : QGraphicsView { parent }
{
    scene.addItem(&pixmapItem);
    pixmapItem.setTransformationMode(Qt::SmoothTransformation);

    setScene(&scene);
    setAlignment(Qt::AlignCenter);
}

void m_GraphicsView::setPixmap(QPixmap pixmap)
{
    pixmapItem.setPixmap(pixmap.copy());
    // 更新视图，保持等比缩放
    fitInView(&pixmapItem, Qt::KeepAspectRatio);

    // qDebug() << pixmapItem.pixmap();
}

void m_GraphicsView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    if (!pixmapItem.pixmap().isNull()) {
        fitInView(&pixmapItem, Qt::KeepAspectRatio);
    }
}
