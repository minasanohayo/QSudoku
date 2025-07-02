#include "m_graphicsview.h"

#include <QScrollBar>
#include <QWheelEvent>

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

void m_GraphicsView::wheelEvent(QWheelEvent* event)
{
    // 是否在用户按下 Ctrl 键的情况下触发的。
    if (event->modifiers() == Qt::ControlModifier) {
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        this->setSceneRect(QRectF(0, 0, 1000, 1000)); // 或 scene->setSceneRect(...)
        // 获取当前鼠标相对于view的位置;
        QPointF cursorPoint = event->position();

        // 获取当前鼠标相对于scene的位置;
        QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));

        // 获取view的宽高;
        qreal viewWidth = this->viewport()->width();
        qreal viewHeight = this->viewport()->height();

        // 获取当前鼠标位置相当于view大小的横纵比例;
        qreal hScale = cursorPoint.x() / viewWidth;
        qreal vScale = cursorPoint.y() / viewHeight;

        // 滚轮的滚动量
        QPoint scrollAmount = event->angleDelta();
        // 正值表示放大，负值表示缩小
        // qDebug() << this->transform();
        scrollAmount.y() > 0 ? scale(1.1, 1.1) : scale(1 / 1.1, 1 / 1.1); // 放大|缩小

        // 将scene坐标转换为放大缩小后的坐标;                   //map（QPoint）：创建并返回一个QPoint对象，该对象是给定点的副本，映射到这个矩阵定义的坐标系统中。注意，转换后的坐标会四舍五入到最接近的整数。
        // QPointF viewPoint = this->matrix().map(scenePos); // matrix()：返回视图的当前转换矩阵。如果没有设置当前转换，则返回单位矩阵。
        QPointF viewPoint = this->transform().map(scenePos);
        // 通过滚动条控制view放大缩小后的展示scene的位置;
        setHorScrollValue(int(viewPoint.x() - viewWidth * hScale));
        setVerScrollValue(int(viewPoint.y() - viewHeight * vScale));
    }
}

void m_GraphicsView::setHorScrollValue(int x)
{
    // 设置的view是左上角的点
    QScrollBar* hScrolBar = horizontalScrollBar();
    hScrolBar->setValue(x);
}

void m_GraphicsView::setVerScrollValue(int y)
{
    // 设置的view是左上角的点
    QScrollBar* vScrolBar = verticalScrollBar();
    vScrolBar->setValue(y);
}
