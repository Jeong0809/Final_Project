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

    enum DrawType {
        Lines,      //0
        FreeHand,   //1
        Laser,      //2
        Cursor,     //3
        Ellipse,    //4
        Rectangle,  //5
        Text,       //6
        Delete,     //7
        Clear,      //8
        Ceph,       //9
        Pano,       //10
        Angle,      //11
        Capture,    //12
        BrushColor, //13
        Implant     //14
    };

private:
     bool m_isDrawable;
     DrawType m_drawType;
     QColor m_penColor;
     int m_penThickness;
     int point;
     double xRate, yRate;

     QList<QGraphicsPathItem*> m_pathList;
     QList<QGraphicsPathItem*> m_laserList;
     QList<QGraphicsEllipseItem*> m_ellipseList;
     QList<QGraphicsRectItem*> m_rectList;
     QList<QGraphicsSimpleTextItem*> m_textList;
     QList<QGraphicsItem*> m_angleList;
     QList<QGraphicsItem*> m_implantList;
     QList<QGraphicsItem*> m_implantImageList;
     QList<QGraphicsItem*> m_lengthList;

     QGraphicsPathItem* m_pathItem;
     QGraphicsItem* m_currentItem;
     QPointF first;
     QPointF second;
     QPointF third;
     QPointF m_startPos;
     QPointF m_endPos;
     QString inputText;

     QGraphicsSimpleTextItem *lengthItem;

     void addEllipseItem(QPointF, QPointF);
     void addRectItem(QPointF, QPointF);
     void addTextItem(QPointF);

     int GetDistance(QPointF, QPointF);

private slots:
    void ReceiveBrushColor(QColor);
    void ReceiveThickness(int);
    void ReceiveType(int);
    void ReceiveText(QString);
    void ReceiveLength(int, int, int, int);
    void ReceiveFontSize(int);

signals:
    void SendMeasurement(QString, double);
    void SendCapturePos(QPointF, QPointF);
    void SendItemList(QList<QGraphicsEllipseItem*>);

};


#endif // IMAGESCENE_H
