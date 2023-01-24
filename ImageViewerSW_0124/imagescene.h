#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QWidget>
#include <QBasicTimer>

class QPinchGesture;
class QGestureEvent;
class MovableItem;


class ImageScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit ImageScene(QWidget *parent = 0);

public:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
//    void wheelEvent(QGraphicsSceneWheelEvent *event) Q_DECL_OVERRIDE;
//    bool event(QEvent *event) Q_DECL_OVERRIDE;

    enum DrawType {
        Lines,
        FreeHand,
        Triangle
    };

private:
//    void pinchTriggered(QPinchGesture *gesture);
//    bool gestureEvent(QGestureEvent *event);
     bool m_isDrawable;
     DrawType m_drawType;
     QColor m_penColor;
     int m_penThickness;
     QList<QGraphicsPathItem*> m_pathList;


     QGraphicsPathItem* m_pathItem;
     QList<QGraphicsEllipseItem*> m_itemList;
     QGraphicsItem* m_currentItem;
     QPointF first;
     QPointF second;
     QPointF third;
     int tmp;


private slots:
    void ReceiveBrushColor(QColor);
    void ReceiveThickness(int);
    void ReceiveType(int);
    void updateSceneeee();

};


#endif // IMAGESCENE_H
