#ifndef M_GRAPHICSVIEW_H
#define M_GRAPHICSVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QObject>
#include <QTimer>
#include <QWidget>
class m_GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit m_GraphicsView(QWidget* parent = nullptr);

    void setPixmap(QPixmap pixmap);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QGraphicsScene scene;
    QGraphicsPixmapItem pixmapItem;

    //    void scrollContentsBy(int dx, int dy) override
    //    {

    //        QGraphicsView::scrollContentsBy(dx, dy);
    //        // 视口在场景中移动了 dx,dy
    //        // TEST_FUNCTION();

    //        scrollEvent_time.setSingleShot(true); // 只触发一次
    //        scrollEvent_time.start(100);
    //    }

signals:

public:
    QTimer scrollEvent_time;

signals:
};

#endif // M_GRAPHICSVIEW_H
