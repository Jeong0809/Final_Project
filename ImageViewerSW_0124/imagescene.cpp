#include "imagescene.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>
#include <QTimeLine>
#include <QPinchGesture>
#include <QGraphicsItem>

#define ItemWidth   16

ImageScene::ImageScene(QWidget *parent)
    : QGraphicsScene(parent)
{
    QPainterPath path;
    m_pathItem = addPath(path);

    //펜의 색상, 두께의 초깃값 설정
    m_penColor = Qt::black;
    m_penThickness = 10;

    //그리기 타입 초깃값 설정(일반 선 그리기)
    m_drawType = Lines;

    m_currentItem = nullptr;
    update();
}

void ImageScene::ReceiveType(int type)
{
    if(type == 0){
        m_drawType = Lines;
    }
    else if(type == 1){
        m_drawType = Triangle;
        tmp = 0;
    }
    qDebug() << "ImageView : " << type;
}

void ImageScene::ReceiveBrushColor(QColor paintColor)
{
    m_penColor = paintColor;
    qDebug() << "ImageView : " << m_penColor;
}

void ImageScene::ReceiveThickness(int Thickness)
{
    m_penThickness = Thickness;
    qDebug() << "ImageView : " << m_penThickness;
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("mousePressEvent");
    if(items(event->scenePos()).isEmpty()) {
        if(m_drawType == Lines) {
            m_isDrawable = true;

            QPainterPath path;
            QGraphicsPathItem* item = addPath(path, QPen(m_penColor, m_penThickness,
                                                         Qt::SolidLine, Qt::RoundCap));
            path = item->path();
            path.moveTo(event->scenePos());
            item->setPath(path);

            m_pathList.append(item);
        }

        else if(m_drawType == Triangle){
            qDebug() << "나는 삼각형";


            switch (tmp) {
            case 0: {
                first = event->scenePos();
                QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
                item1->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
                item1->setBrush(QBrush(QColor(m_penColor), Qt::SolidPattern));
                item1->setRect(first.x()-ItemWidth/2, first.y()-ItemWidth/2, ItemWidth, ItemWidth);
                item1->setTransformOriginPoint(ItemWidth/2, ItemWidth/2);

                addItem(item1);
                m_itemList.append(item1);
            }

            case 1: {
                second = event->scenePos();
                QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
                item1->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
                item1->setBrush(QBrush(QColor(m_penColor), Qt::SolidPattern));
                item1->setRect(second.x()-ItemWidth/2, second.y()-ItemWidth/2, ItemWidth, ItemWidth);
                item1->setTransformOriginPoint(ItemWidth/2, ItemWidth/2);

                addItem(item1);
                m_itemList.append(item1);

                QPainterPath path(first);
                            path.quadTo(first, second);
                            addPath(path, QColorConstants::Svg::red);
            }

            case 2: {
                third = event->scenePos();
                QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
                item1->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
                item1->setBrush(QBrush(QColor(m_penColor), Qt::SolidPattern));
                item1->setRect(third.x()-ItemWidth/2, third.y()-ItemWidth/2, ItemWidth, ItemWidth);
                item1->setTransformOriginPoint(ItemWidth/2, ItemWidth/2);

                addItem(item1);
                m_itemList.append(item1);

                QPainterPath path(second);
                path.quadTo(second, third);
                addPath(path, QColorConstants::Svg::red);

                QPainterPath path1(third);
                path.quadTo(third, first);
                addPath(path, QColorConstants::Svg::red);
            }
            }

            tmp++;
            if(tmp > 3)
            {
                return;
            }

        }
//        else if(m_drawType != Cursor) {
//            m_startPos = event->scenePos();
//        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("mouseReleaseEvent");
    Q_UNUSED(event);

    if(m_currentItem != nullptr)
        delete m_currentItem;

    switch (m_drawType) {
//    case Cursor:
//        break;
//    case Line:
//        addLineItem(m_startPos, mouseEvent->scenePos());
//        break;
//    case Rect:
//        addRectItem(m_startPos, mouseEvent->scenePos());
//        break;
//    case Ellipse:
//        addEllipseItem(m_startPos, mouseEvent->scenePos());
//        break;
    case Lines:
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_pathList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
//                item->setFlag(QGraphicsItem::ItemIsMovable, true);
//                item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            }
            m_isDrawable = false;
        }
        break;
//    case Image:
//        addImageItem(m_startPos, mouseEvent->scenePos());
//        break;
//    case Star:
//        addStarItem(m_startPos, mouseEvent->scenePos());
//        break;
//    }
    }

    m_currentItem = nullptr;
    QGraphicsScene::mouseReleaseEvent(event);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("mouseMoveEvent");
    if(m_drawType == Lines) {
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_pathList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
            }
        }
    }
    else if(m_drawType == Triangle){
    }

    /*else if(m_currentShape == Line) {
        if(m_currentItem != nullptr)
            delete m_currentItem;
        QLineF line(m_startPos, mouseEvent->scenePos());
        QGraphicsLineItem *item = new QGraphicsLineItem(line);
        item->setPen(QPen(QColor(Qt::red), 1));
        addItem(item);
        m_currentItem = item;
    } else if(m_currentShape != Cursor) {
        if(m_currentItem != nullptr)
            delete m_currentItem;
        QRectF rect(m_startPos, mouseEvent->scenePos());
        QGraphicsRectItem *item = new QGraphicsRectItem(rect);
        item->setPen(QPen(QColor(Qt::red), 1));
        addItem(item);
        m_currentItem = item;
    }*/
    QGraphicsScene::mouseMoveEvent(event);
}

void ImageScene::updateSceneeee()
{
//    qDebug("updateScene");
//    QPen pen(m_penColor, m_penThickness);
//    m_pathItem->setPen(pen);
//    //    m_pathItem->setBrush(Qt::transparent);
//    QPainterPath path = m_pathItem->path();

//    int cnt = 0;
//    path.clear();
//    if(tmp >= 3){
//        foreach(auto item, m_itemList) {
//            //        qDebug() << item->scenePos();
//            QPointF p = item->scenePos();
//            if(cnt++ == 0) {
//                path.moveTo(p.rx()+ItemWidth/2, p.ry()+ItemWidth/2);
//            } else {
//                path.lineTo(p.rx()+ItemWidth/2, p.ry()+ItemWidth/2);
//            }

//        }
//        path.closeSubpath();
//        m_pathItem->setPath(path);
//    }
}

//void ImageScene::wheelEvent(QGraphicsSceneWheelEvent *event)
//{
//    qDebug("wheelEvent");
//    int numDegrees = /*event->angleDelta().y() / */8;
//    int numSteps = numDegrees / 15; // see QWheelEvent documentation
//    _numScheduledScalings += numSteps;
//    if (_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
//        _numScheduledScalings = numSteps;

//    QTimeLine *anim = new QTimeLine(350, this);
//    anim->setUpdateInterval(20);

//    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
//    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
//    anim->start();
//}

//void ImageScene::scalingTime(qreal x)
//{
//    Q_UNUSED(x);
//    qreal factor = 1.0 + qreal(_numScheduledScalings) / 300.0;
////    scale(factor, factor);
//}

//void ImageScene::animFinished()
//{
//    (_numScheduledScalings > 0)?_numScheduledScalings--:_numScheduledScalings++;
//    //delete sender();     //    sender()->~QObject();
//}

//bool ImageScene::event(QEvent *event)
//{
//    if (event->type() == QEvent::Gesture)
//        return gestureEvent(static_cast<QGestureEvent*>(event));
//    return QGraphicsScene::event(event);
//}

//bool ImageScene::gestureEvent(QGestureEvent *event)
//{
//    qDebug("gestureEvent");
//    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
//        pinchTriggered(static_cast<QPinchGesture *>(pinch));
//    return true;
//}

//void ImageScene::pinchTriggered(QPinchGesture *gesture)
//{
//    qDebug("pinchTriggered");
//    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
//    if (changeFlags & QPinchGesture::RotationAngleChanged) {
//        qreal rotationDelta = gesture->rotationAngle() - gesture->lastRotationAngle();
//        _rotationAngle += rotationDelta;
//    }
//    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
//        _currentStepScaleFactor = gesture->totalScaleFactor();
//    }
//    if (gesture->state() == Qt::GestureFinished) {
//        _scaleFactor *= _currentStepScaleFactor;
//        _currentStepScaleFactor = 1;
//    }
//    update();
//}
