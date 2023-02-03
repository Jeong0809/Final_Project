#include "imagescene.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>
#include <QTimeLine>
#include <QPinchGesture>
#include <QGraphicsItem>
#include <QTimer>

#define DotWidth   5

ImageScene::ImageScene(QWidget *parent)
    : QGraphicsScene(parent)
{
    QPainterPath path;
    m_pathItem = addPath(path);

    //펜의 색상, 두께의 초깃값 설정
    m_penColor = Qt::red;
    m_penThickness = 6;

    //그리기 타입 초깃값 설정(레이저 그리기)
    m_drawType = Laser;
    m_currentItem = nullptr;
    point = 0;
    update();
}

void ImageScene::ReceiveType(int type)
{
    //각도를 위한 삼각형 데이터는 다른 버튼 클릭 시 바로 삭제
    if(!m_angleList.empty()){
        foreach(auto item, m_angleList)
            removeItem(item);
        update();
    }
    m_angleList.clear();

    //레이저를 위한 데이터는 다른 버튼 클릭 시 바로 삭제
    if(!m_laserList.empty()){
        foreach(auto item, m_laserList)
            removeItem(item);
        update();
    }
    m_laserList.clear();


    if(type == 0){
        m_drawType = Lines;
    }

    else if(type == 1){
        m_drawType = FreeHand;
    }

    else if(type == 2){
        m_drawType = Laser;
    }

    else if(type == 3){
        m_drawType = Cursor;
    }

    else if(type == 4){
        m_drawType = Ellipse;
    }

    else if(type == 5){
        m_drawType = Rectangle;
    }

    else if(type == 6){
        m_drawType = Text;
    }

    //Delete : 선택된 아이템 삭제
    else if(type == 7){
        m_drawType = Delete;
        foreach(auto item, selectedItems())
            removeItem(item);
        update();
    }

    //Clear : 원본을 눌렀을 때 저장되어있던 원, 사각형, 텍스트 데이터 삭제
    else if(type == 8){
        m_ellipseList.clear();
        m_rectList.clear();
        m_textList.clear();
        m_angleList.clear();

        //도구 탭을 이동했을 때 이전에 그려져있던 레이저 그리기를 삭제
        if(!m_laserList.empty()){
            foreach(auto item, m_laserList)
                removeItem(item);
            update();
        }
        m_laserList.clear();
    }

    //Ceph : 길이 측정
    else if(type == 9){
        m_drawType = Ceph;
    }

    //Pano : 길이 측정
    else if(type == 10){
        m_drawType = Pano;
    }

    //각도 측정 기능
    else if(type == 11){
        m_drawType = Angle;
    }

    //캡쳐 기능
    else if(type == 12){
        m_drawType = Capture;
    }

    else if(type == 13){
        m_drawType = BrushColor;
    }

    else if(type == 14){
        m_drawType = Implant;
    }

    qDebug() << "ImageView : " << type;

    //원본을 누르지 않았으면 다시 이동이 불가하도록 하고 선택만 가능하도록 설정
    if(!m_ellipseList.empty()){
        foreach(auto item, m_ellipseList) {
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }
    }

    //원본을 누르지 않았으면 다시 이동이 불가하도록 하고 선택만 가능하도록 설정
    if(!m_rectList.empty()){
        foreach(auto item, m_rectList) {
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }
    }

    //원본을 누르지 않았으면 다시 이동이 불가하도록 하고 선택만 가능하도록 설정
    if(!m_textList.empty()){
        foreach(auto item, m_textList) {
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }
    }
}

void ImageScene::ReceiveFontSize(int size)
{
    if(!m_textList.empty()){
        foreach(auto item, m_textList) {
            if(item->isSelected()){
                QFont Font;
                Font.setPointSize(size);
                Font.setBold(true);
                Font.setStyleHint(QFont::Courier);
                item->setFont(Font);
            }
        }
    }
}

void ImageScene::ReceiveText(QString Text)
{
    inputText = Text;
}

void ImageScene::ReceiveBrushColor(QColor paintColor)
{
    m_penColor = paintColor;

    if(m_drawType == DrawType::BrushColor){
        if(!m_textList.empty()){
            foreach(auto item, m_textList) {
                if(item->isSelected()){
                    item->setBrush(m_penColor);

                }
            }
        }

        if(!m_ellipseList.empty()){
            foreach(auto item, m_ellipseList) {
                if(item->isSelected()){
                    QPen pen(m_penColor, item->pen().width());
                    item->setPen(pen);
                    qDebug() << item->brush();
                    qDebug() << item->pen().width();
                }
            }
        }

        if(!m_rectList.empty()){
            foreach(auto item, m_rectList) {
                if(item->isSelected()){
                    QPen pen(m_penColor, item->pen().width(), Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
                    item->setPen(pen);
                }
            }
        }
    }

    ReceiveType(DrawType::Cursor);
}

void ImageScene::ReceiveThickness(int Thickness)
{
    m_penThickness = Thickness;

    if(!m_ellipseList.empty()){
        foreach(auto item, m_ellipseList) {
            if(item->isSelected()){
                QPen pen(item->pen().color(), m_penThickness);
                item->setPen(pen);
                qDebug() << item->brush();
                qDebug() << item->pen().width();
            }
        }
    }

    if(!m_rectList.empty()){
        foreach(auto item, m_rectList) {
            if(item->isSelected()){
                QPen pen(item->pen().color(), m_penThickness, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
                item->setPen(pen);
            }
        }
    }

    ReceiveType(DrawType::Cursor);
}

void ImageScene::ReceiveLength(int origWidth, int origHeight, int sceneWidth, int sceneHeight)
{
    xRate =  (double)origWidth / sceneWidth;
    yRate =  (double)origHeight / sceneHeight;
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("mousePressEvent");
    //    if(items(event->scenePos()).isEmpty()) {
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

    else if(m_drawType == Laser){
        m_isDrawable = true;

        //다른 그리기 수행 시 바로 삭제
        if(!m_laserList.empty()){
            foreach(auto item, m_laserList)
                removeItem(item);
            update();
        }
        m_laserList.clear();

        QPainterPath path;
        QGraphicsPathItem* item = addPath(path, QPen(Qt::red, m_penThickness,
                                                     Qt::DotLine, Qt::RoundCap));
        path = item->path();
        path.moveTo(event->scenePos());
        item->setPath(path);
        m_laserList.append(item);
    }

    else if(m_drawType == FreeHand){
        m_isDrawable = true;
        m_startPos = event->scenePos();

        QPainterPath path;
        QGraphicsPathItem* item = addPath(path, QPen(m_penColor, m_penThickness,
                                                     Qt::SolidLine, Qt::RoundCap));
        path = item->path();
        path.moveTo(event->scenePos());
        item->setPath(path);
        m_pathList.append(item);
    }

    else if(m_drawType == Ellipse){
        m_startPos = event->scenePos();
    }

    else if(m_drawType == Rectangle){
        m_startPos = event->scenePos();
    }

    else if(m_drawType == Text) {
        m_startPos = event->scenePos();
    }

    else if(m_drawType == Ceph) {
        m_startPos = event->scenePos();
    }

    else if(m_drawType == Pano) {
        m_startPos = event->scenePos();
    }

    else if(m_drawType == Capture){
        m_startPos = event->scenePos();
    }

    else if(m_drawType == Implant){
        switch (point) {
        case 0: {
            if(!m_implantList.empty()){
                foreach(auto item, m_implantList)
                    removeItem(item);
                update();
            }
            m_implantList.clear();

            first = event->scenePos();
            QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
            item1->setPen(QPen(Qt::black));
            item1->setBrush(QBrush(QColor(Qt::red), Qt::SolidPattern));
            item1->setRect(first.x()-DotWidth/2, first.y()-DotWidth/2, DotWidth, DotWidth);
            item1->setTransformOriginPoint(DotWidth/2, DotWidth/2);
            addItem(item1);
            m_implantList.append(item1);
            break;
        }

        case 1: {
            second = event->scenePos();
            QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
            item1->setPen(QPen(Qt::black));
            item1->setBrush(QBrush(QColor(Qt::white), Qt::SolidPattern));
            item1->setRect(second.x()-DotWidth/2, second.y()-DotWidth/2, DotWidth, DotWidth);
            item1->setTransformOriginPoint(DotWidth/2, DotWidth/2);
            addItem(item1);
            m_implantList.append(item1);
            break;
        }

        case 2: {
            third = event->scenePos();
            QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
            item1->setPen(QPen(Qt::black));
            item1->setBrush(QBrush(QColor(Qt::white), Qt::SolidPattern));
            item1->setRect(third.x()-DotWidth/2, third.y()-DotWidth/2, DotWidth, DotWidth);
            item1->setTransformOriginPoint(DotWidth/2, DotWidth/2);
            addItem(item1);
            m_implantList.append(item1);
            break;
        }
        }

        point++;
        if(point >= 3)
        {
            float delta_x = second.x() - first.x();
            float delta_y = first.y() - second.y();
//            float diff_deg = atan2f(delta_y, delta_x) * 180 / 3.14;
            float diff_deg = qRadiansToDegrees(atan2f(delta_y, delta_x));
//            diff_deg = (diff_deg < 0) ? (360 + diff_deg) : diff_deg;

            diff_deg = diff_deg + 90;
            diff_deg = (diff_deg < 0) ? (360 + diff_deg) : diff_deg;
            qDebug() << diff_deg;

            QRectF rect = QRectF(first, QPointF(third.x(), second.y())).normalized();
            QGraphicsRectItem *rectItem = new QGraphicsRectItem(rect);
            rectItem->setFlags(QGraphicsItem::ItemIsSelectable);
            rectItem->setPen(QPen(m_penColor, m_penThickness, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            QTransform transform;
            transform.translate(first.x(),first.y());
            transform.rotate(-diff_deg);
            transform.translate(-first.x(), -first.y());
            rectItem->setTransform(transform);

            addItem(rectItem);

            point = 0;
        }
    }

    else if(m_drawType == Angle){
        switch (point) {
        case 0: {

            if(!m_angleList.empty()){
                foreach(auto item, m_angleList)
                    removeItem(item);
                update();
            }
            m_angleList.clear();

            first = event->scenePos();
            QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
            item1->setPen(QPen(Qt::black));
            item1->setBrush(QBrush(QColor(Qt::white), Qt::SolidPattern));
            item1->setRect(first.x()-DotWidth/2, first.y()-DotWidth/2, DotWidth, DotWidth);
            item1->setTransformOriginPoint(DotWidth/2, DotWidth/2);
            addItem(item1);
            m_angleList.append(item1);
            break;
        }

        case 1: {
            second = event->scenePos();
            QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
            item1->setPen(QPen(Qt::black));
            item1->setBrush(QBrush(QColor(Qt::white), Qt::SolidPattern));
            item1->setRect(second.x()-DotWidth/2, second.y()-DotWidth/2, DotWidth, DotWidth);
            item1->setTransformOriginPoint(DotWidth/2, DotWidth/2);
            addItem(item1);
            m_angleList.append(item1);

            QPainterPath path(first);
            path.quadTo(first, second);
            QGraphicsPathItem* line = addPath(path, QPen(Qt::yellow, 2,
                                                         Qt::SolidLine, Qt::RoundCap));
            m_angleList.append(line);
            break;
        }

        case 2: {
            third = event->scenePos();
            QGraphicsEllipseItem* item1 = new QGraphicsEllipseItem();
            item1->setPen(QPen(Qt::black));
            item1->setBrush(QBrush(QColor(Qt::white), Qt::SolidPattern));
            item1->setRect(third.x()-DotWidth/2, third.y()-DotWidth/2, DotWidth, DotWidth);
            item1->setTransformOriginPoint(DotWidth/2, DotWidth/2);

            addItem(item1);
            m_angleList.append(item1);

            QPainterPath path(second);
            path.quadTo(second, third);
            QGraphicsPathItem* line = addPath(path, QPen(Qt::yellow, 2,
                                                         Qt::SolidLine, Qt::RoundCap));
            m_angleList.append(line);
            break;
        }
        }

        point++;
        if(point >= 3)
        {
            double A = sqrt(pow(first.x() - third.x(), 2) + pow(first.y() - third.y(), 2));
            double B = sqrt(pow(first.x() - second.x(), 2) + pow(first.y() - second.y(), 2));
            double C = sqrt(pow(second.x() - third.x(), 2) + pow(second.y() - third.y(), 2));
            double Degree = acos((pow(B, 2) + pow(C, 2) - pow(A, 2)) / (2 * B * C));
            Degree = Degree * (180 / M_PI);

            emit SendMeasurement("Angle", Degree);
            point = 0;
        }
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

    case Cursor:
        break;

    case Rectangle:
        addRectItem(m_startPos, event->scenePos());
        break;

    case Ellipse:
        addEllipseItem(m_startPos, event->scenePos());
        break;

    case Text:
        addTextItem(m_startPos);
        break;

    case Capture:
        emit SendCapturePos(m_startPos, event->scenePos());
        break;

    case Ceph:
        foreach(auto item, m_lengthList)
            removeItem(item);
        update();
        m_lengthList.clear();
        break;

    case Pano:
        foreach(auto item, m_lengthList)
            removeItem(item);
        update();
        m_lengthList.clear();
        break;

    case Lines: {
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_pathList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
            }
            m_isDrawable = false;
        }
    }
        break;


    case Laser: {
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_laserList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
            }
            m_isDrawable = false;
        }
    }
        break;

    case FreeHand: {
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_pathList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
            }
            m_endPos = event->scenePos();
            QPainterPath path2(m_endPos);
            path2.quadTo(m_endPos, m_startPos);
            addPath(path2,  QPen(m_penColor, m_penThickness,
                                 Qt::SolidLine, Qt::RoundCap));
            m_isDrawable = false;
        }
    }
        break;

    default:
        break;
    }

    m_currentItem = nullptr;
    QGraphicsScene::mouseReleaseEvent(event);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("mouseMoveEvent");
    switch (m_drawType) {
    case Lines: {
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_pathList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
            }
        }
    }
        break;

    case Laser: {
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_laserList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
            }
        }
    }
        break;


    case FreeHand: {
        if(m_isDrawable) {
            QGraphicsPathItem* item = m_pathList.last();
            if(item) {
                QPainterPath path = item->path();
                path.lineTo(event->scenePos());
                item->setPath(path);
            }
        }
    }
        break;

    case Rectangle: {
        if(m_currentItem != nullptr)
            delete m_currentItem;
        QRectF rect = QRectF(m_startPos, event->scenePos()).normalized();
        QGraphicsRectItem *item = new QGraphicsRectItem(rect);
        item->setPen(QPen(QColor(Qt::yellow), 2));
        addItem(item);
        m_currentItem = item;
    }
        break;

    case Ellipse: {
        if(m_currentItem != nullptr)
            delete m_currentItem;
        QRectF rect = QRectF(m_startPos, event->scenePos()).normalized();
        QGraphicsEllipseItem *item = new QGraphicsEllipseItem(rect);
        item->setPen(QPen(QColor(Qt::yellow), 2));
        addItem(item);
        m_currentItem = item;
    }
        break;

    case Capture: {
        if(m_currentItem != nullptr)
            delete m_currentItem;
        QRectF rect = QRectF(m_startPos, event->scenePos()).normalized();
        QGraphicsRectItem *item = new QGraphicsRectItem(rect);
        item->setPen(QPen(QColor(Qt::yellow), 2));
        addItem(item);
        m_currentItem = item;
    }
        break;

    case Text: {
            if(m_currentItem != nullptr)
                delete m_currentItem;
            QRectF rect = QRectF(m_startPos, event->scenePos()).normalized();
            QGraphicsRectItem *item = new QGraphicsRectItem(rect);
            item->setPen(QPen(QColor(Qt::yellow), 2));
            addItem(item);
            m_currentItem = item;
        }
        break;

    case Ceph: {
        if(m_currentItem != nullptr)
            delete m_currentItem;
        QLineF line(m_startPos, event->scenePos());
        QGraphicsLineItem *item = new QGraphicsLineItem(line);
        item->setPen(QPen(QColor(Qt::red), 1));
        addItem(item);
        m_currentItem = item;

        double length = qSqrt(qPow((m_startPos.x() - event->scenePos().x())*xRate, 2)
                               + qPow((m_startPos.y() - event->scenePos().y())*yRate, 2)) * 0.08733;

        if(m_lengthList.empty()){
            lengthItem = new QGraphicsSimpleTextItem();
            QFont Font;
            Font.setPointSize(15);
            Font.setBold(true);
            Font.setStyleHint(QFont::Courier);
            lengthItem->setPos(m_startPos.x() + 5, m_startPos.y() - 10);
            lengthItem->setFont(Font);
            lengthItem->setBrush(Qt::black);
            addItem(lengthItem);
            m_lengthList.append(lengthItem);
        }
        QString Result = QString::number(length);
        lengthItem->setText(Result);

        emit SendMeasurement("Ceph", length);
    }
        break;

    case Pano: {
        if(m_currentItem != nullptr)
            delete m_currentItem;
        QLineF line(m_startPos, event->scenePos());
        QGraphicsLineItem *item = new QGraphicsLineItem(line);
        item->setPen(QPen(QColor(Qt::red), 1));
        addItem(item);
        m_currentItem = item;

        double length = qSqrt(qPow((m_startPos.x() - event->scenePos().x())*xRate, 2)
                               + qPow((m_startPos.y() - event->scenePos().y())*yRate, 2)) * 0.096358;

        if(m_lengthList.empty()){
            lengthItem = new QGraphicsSimpleTextItem();
            QFont Font;
            Font.setPointSize(15);
            Font.setBold(true);
            Font.setStyleHint(QFont::Courier);
            lengthItem->setPos(m_startPos.x() + 5, m_startPos.y() - 10);
            lengthItem->setFont(Font);
            lengthItem->setBrush(Qt::black);
            addItem(lengthItem);
            m_lengthList.append(lengthItem);
        }
        QString Result = QString::number(length);
        lengthItem->setText(Result);

        emit SendMeasurement("Pano", length);
    }
        break;

    case Cursor: {
        //커서 모드일 때 선택된 원형 이동
        foreach(auto item, m_ellipseList) {
            if(item->isSelected()){
                item->setFlags(QGraphicsItem::ItemIsMovable| QGraphicsItem::ItemIsSelectable);
            }
        }

        //커서 모드일 때 선택된 사각형 이동
        foreach(auto item, m_rectList) {
            if(item->isSelected()){
                item->setFlags(QGraphicsItem::ItemIsMovable| QGraphicsItem::ItemIsSelectable);
            }
        }

        //커서 모드일 때 선택된 텍스트 이동
        foreach(auto item, m_textList) {
            if(item->isSelected()){
                item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            }
        }
    }
        break;

    default:
        break;
    }
    QGraphicsScene::mouseMoveEvent(event);
}


void ImageScene::addEllipseItem(QPointF stPos, QPointF edPos)
{
    QRectF rect = QRectF(stPos, edPos).normalized();
    QGraphicsEllipseItem *ellipseItem = new QGraphicsEllipseItem(rect);

    QBrush brush(m_penColor, Qt::NoBrush);
    ellipseItem->setBrush(brush);
    ellipseItem->setFlags(QGraphicsItem::ItemIsSelectable);
    ellipseItem->setPen(QPen(m_penColor, m_penThickness));
    addItem(ellipseItem);
    m_ellipseList.append(ellipseItem);
}

void ImageScene::addRectItem(QPointF stPos, QPointF edPos)
{
    QRectF rect = QRectF(stPos, edPos).normalized();
    QGraphicsRectItem *rectItem = new QGraphicsRectItem(rect);
    rectItem->setFlags(QGraphicsItem::ItemIsSelectable);
    rectItem->setPen(QPen(m_penColor, m_penThickness, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    addItem(rectItem);
    m_rectList.append(rectItem);
}

void ImageScene::addTextItem(QPointF stPos)
{
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(inputText);
    QFont Font;
    Font.setPointSize(10);
    Font.setBold(true);
    Font.setStyleHint(QFont::Courier);

    textItem->setFlags(QGraphicsItem::ItemIsSelectable);
    textItem->setPos(stPos);
    textItem->setFont(Font);
    textItem->setBrush(m_penColor);
    addItem(textItem);
    m_textList.append(textItem);

    m_drawType = Cursor;
}
