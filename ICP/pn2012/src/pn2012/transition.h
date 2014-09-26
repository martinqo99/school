/**
 * @file: transition.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for transition.
 */
#ifndef TRANSITION_H
#define TRANSITION_H

#include <QGraphicsItem>
#include <QSet>
#include "arc.h"
class Arc;
class QPolygonF;
class Transition : public QGraphicsItem
{
public:
    Transition(QColor text, QColor outline, QColor bacground);
    ~Transition();
    void addArc(Arc *arc);
    void removeArc(Arc *arc);
    void setText(const QString &condition, const QString &operation, const QString &name);
    QString label() {return myName;}
    QString condition() {return myCondition;}
    QString operation() {return myOperation;}
    QString name() {return myName;}
    void setOutlineColor(const QColor &color);
    QColor  outlineColor() const;
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;
    void setColors(QColor outline,QColor text,QColor background);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPolygonF myPoly(){return myPolygon;}
    QString condition() const;
    void setCustomFlags(int mode);
    void setFont(int font);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

signals:
    
public slots:

private:
    QRectF outlineRect() const;
    int roundness(double size) const;
    /// list of arcs connected to transition
    QSet<Arc *> myArcs;
    /// condition of transition
    QString myCondition;
    /// condition of operation
    QString myOperation;
    /// name of transition
    QString myName;
    /// colors of graphics item
    QColor myTextColor;
    QColor myOutlineColor;
    QColor myBackgroundColor;
    QPolygonF myPolygon;
    /// mode editing/simulation
    int mymode;
    /// mode of font => arial/times news roman
    int myfont;
};

#endif // TRANSITION_H
/*** End of file transition.h ***/
