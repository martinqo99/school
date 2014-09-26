/**
 * @file: about.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Place class of petri net.
 */

#ifndef PLACE_H
#define PLACE_H

#include <QGraphicsItem>
#include <QSet>
#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
#include <QPen>
#include "arc.h"

class QPolygonF;
class Arc;

class Place : public QGraphicsItem
{
public:
    Place(QColor text,QColor out, QColor backg);
    ~Place();
    void setText(const QString &label, const QString &tokens);
    QString label() {return myPlaceName;}
    QString tokens() const;
    void setOutlineColor(const QColor &color);
    QColor outlineColor() const;
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;
    void addArc(Arc *arc);
    void setColors(QColor out,QColor text,QColor back);
    void removeArc(Arc *arc);
    void setShapeFont(int shape,int font);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPolygonF myPoly(){return myPolygon;}
    void setCustomFlags(int mode);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QRectF outlineRect() const;
    int roundness(double size) const;
    /// list of arcs
    QSet<Arc *> myArcs;
    /// string of tokens in format like "1,2,3"
    QString myTokens;
    /// name of place
    QString myPlaceName;
    /// colors of graphics item
    QColor myTextColor;
    QColor myOutlineColor;
    QColor myBackgroundColor;
    /// polygon of place
    QPolygonF myPolygon;
    /// mode => edit/simulation
    int mymode;
    /// shape => ellipse/triangle
    int myshape;
    /// myfont => arial/times new roman
    int myfont;
};

#endif // PLACE_H
/*** End of file place.h ***/
