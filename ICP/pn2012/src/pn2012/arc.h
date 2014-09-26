/**
 * @file: arc.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for arc.h.
 */
#ifndef ARC_H
#define ARC_H

#include <QGraphicsItem>
#include "transition.h"
#include "place.h"

class QGraphicsPolygonItem;
class GraphicsLineItem;
class QRectF;
class QPainterPath;
class QGraphicsSceneMouseEvent;
class Transition;
class Place;

class Arc : public QGraphicsLineItem
{
public:
    Arc(Transition *trans, Place *plac, int direction, QColor linecolor,QString uniquename);
    ~Arc();
    int getDirection(){return direction;}
    void setDirection(int direction);
    QString getLabel(){return labelarc;}
    QString getName(){return uniqueName;}
    void setLabel(QString label);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    Transition *toTransition() const;
    Place *toPlace() const;
    void setFont(int font);
    void setColor(const QColor &color);
    QColor color() const;
    void removeArc();
    void trackNodes();

signals:
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    /// arc has unique name that user cannot change
    QString uniqueName;
    /// polygon of arrow
    QPolygonF arrowhead;
    /// color of arc
    QColor mycolor;
    /// connected transition
    Transition *transarc;
    /// connected place
    Place *placearc;
    /// direction
    int direction; /// 0 from transition to place
    QString labelarc;
    /// type of font
    int myfont;
};

#endif // ARC_H
/*** End of file arc.h ***/
