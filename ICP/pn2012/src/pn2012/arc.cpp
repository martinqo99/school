/**
 * @file: arc.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Arc class, which connects place and transition.
 */

#include <QtGui>
#include "arc.h"
#include <QDebug>
#include <math.h>
#include "arcproperties.h"

#define PINUMBER 3.141516

/*!
 * Constructor.
 */
Arc::Arc(Transition *trans, Place *plac, int direction, QColor arcolor,QString uniquename)
{
    myfont = 1;
    this->direction = direction;
    transarc = trans;
    placearc = plac;
    labelarc = "x";
    uniqueName = uniquename;
    transarc->addArc(this);
    placearc->addArc(this);

    setFlags(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);
    //setColor(Qt::black);
    mycolor = arcolor;
    setPen(QPen(mycolor,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
}

/*!
 * Destructor.
 */
Arc::~Arc()
{
    transarc->removeArc(this);
    placearc->removeArc(this);
}

/*!
 * Set direction of arc 0/1.
 */
void Arc::setDirection(int direction)
{
    prepareGeometryChange();
    this->direction = direction;
    update();
}

/*!
 * Set label for arc.
 */
void Arc::setLabel(QString label)
{
    prepareGeometryChange();
    this->labelarc = label;
    update();
}

/*!
 * Return transition connected with arc.
 */
Transition *Arc::toTransition() const
{
    return transarc;
}

/*!
 * Return place connected to arc.
 */
Place *Arc::toPlace() const
{
    return placearc;
}

/*!
 * Set font of arc.
 */
void Arc::setFont(int font)
{
    this->myfont = font;
    update();
}

/*!
 * Set color.
 */
void Arc::setColor(const QColor &color)
{
    mycolor = color;
}

/*!
 * Return color.
 */
QColor Arc::color() const
{
    return mycolor;
}

/*!
 * Remove arc from place and transition.
 */
void Arc::removeArc()
{
    transarc->removeArc(this);
    placearc->removeArc(this);
}

/*!
 * Return boundingRect.
 */
QRectF Arc::boundingRect() const
{
    qreal extra = (pen().width()+20)/2.0;
    return QRectF(line().p1(),QSizeF(line().p2().x()-line().p1().x(),
                                     line().p2().y()-line().p1().y())).normalized().adjusted(-extra,-extra,extra,extra);
}

/*!
 * Return shape of arc.
 */
QPainterPath Arc::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowhead);
    return path;
}

/*!
 * Doubleclik shows Arcproperties.
 */
void Arc::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    ArcProperties dialog(this);
    dialog.exec();
}

/*!
 * Update graphics of line.
 */
void Arc::trackNodes()
{
  setLine(QLineF(transarc->pos(),placearc->pos()));
  update();
}

/*!
 * Paint line.
 */
void Arc::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    /// specify pen and painter
    QPen myPen = pen();
    myPen.setColor(mycolor);
    qreal arrowSize = 7;
    painter->setPen(myPen);
    painter->setBrush(mycolor);

    if(option->state & QStyle::State_Selected) {
        myPen.setStyle(Qt::DotLine);
        myPen.setWidth(2);
        painter->setPen(myPen);
    }

    /// restriction of line to edge of transition
    QLineF centerLine(transarc->pos(),placearc->pos());
    QPolygonF endPolygon = transarc->myPoly();
    QPointF p1 = endPolygon.first() + transarc->pos();
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for(int i = 1; i < endPolygon.count(); ++i)
    {
        p2 = endPolygon.at(i) + transarc->pos();
        polyLine = QLineF(p1,p2);
        QLineF::IntersectType intersecttype = polyLine.intersect(centerLine,&intersectPoint);

        if(intersecttype == QLineF::BoundedIntersection)
            break;
        p1 = p2;
    }

    /// restriction of line to edge of place
    QPolygonF endPolygon2 = placearc->myPoly();
    QPointF p11 = endPolygon2.first() + placearc->pos();
    QPointF p21;
    QPointF intersectPoint2;
    for(int i = 1; i < endPolygon2.count(); ++i)
    {
        p21 = endPolygon2.at(i) + placearc->pos();
        polyLine = QLineF(p11,p21);
        QLineF::IntersectType intersecttype2 = polyLine.intersect(centerLine,&intersectPoint2);
        if(intersecttype2 == QLineF::BoundedIntersection)
            break;
        p11 = p21;
    }

    /// set line from transition to place
    setLine(QLineF(intersectPoint,intersectPoint2));

    /// arrow head on one edge
    if(direction == 1 or direction == 2)
    {
        double angle = ::acos(line().dx() / line().length());
        if(line().dy() >= 0)
            angle = (PINUMBER * 2) - angle;
        QPointF arrowP1 = line().p1() + QPointF(sin(angle+PINUMBER/3)*arrowSize , cos(angle + PINUMBER/3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle+PINUMBER -PINUMBER/3) * arrowSize , cos(angle +PINUMBER - PINUMBER/3) * arrowSize);
        arrowhead.clear();
        arrowhead << line().p1() << arrowP1 << arrowP2;
        painter->drawPolygon(arrowhead);
    }

    /// arrow head on second edge
    if(direction == 0 or direction == 2)
    {
        double angle2 = ::acos(line().dx() / line().length());
        if(line().dy() >= 0)
            angle2 = (PINUMBER * 2) - angle2;
        QPointF arrowP11 = (line().p2() - QPointF(sin(angle2+PINUMBER/3)*arrowSize,cos(angle2+PINUMBER/3)*arrowSize));
        QPointF arrowP21 = (line().p2() - QPointF(sin(angle2+PINUMBER-PINUMBER/3)*arrowSize,cos(angle2+PINUMBER-PINUMBER/3)*arrowSize));
        arrowhead.clear();
        arrowhead << line().p2() << arrowP11 << arrowP21;
        painter->drawPolygon(arrowhead);
    }

    /// draw label
    painter->drawText((line().p1().x() + line().p2().x())/2-5,(line().p1().y() + line().p2().y())/2-15,labelarc);

    /// draw final line
    painter->drawLine(line());
    update();
}
/*** End of file arc.cpp ***/
