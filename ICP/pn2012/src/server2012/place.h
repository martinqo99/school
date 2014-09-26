/**
 * @file: place.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Place class of petri net.
*/

#ifndef PLACE_H
#define PLACE_H

#include "arc.h"
#include <QGraphicsItem>

class Arc;

class Place : public QGraphicsItem
{
public:
    Place();
    ~Place();
    void setText(const QString &label, QString &tokens);
    QString label() {return myPlaceName;}  ///return name of place
    QStringList tokens() const;
    void addArc(Arc *arc);
    void removeArc(Arc *arc);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QStringList myTokens;  ///list of tokens

private:
    QSet<Arc *> myArcs;  ///hash table
    QString myPlaceName; ///name of place
};

#endif // PLACE_H
