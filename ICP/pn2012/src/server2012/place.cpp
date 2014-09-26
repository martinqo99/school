/**
 * @file: place.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Place class of petri net.
 */

#include "arc.h"
#include "place.h"
#include <QDebug>

#define SIMULATION 1
#define EDITING 2

/*!
 * A constructor.
 */
Place::Place()
{
}

/*!
 * A destructor.
 */
Place::~Place()
{
    foreach(Arc *arc,myArcs)
        delete arc;
}

/*!
 * Get tokens and name from place.
 */
void Place::setText(const QString &label, QString &tokens)
{
    QString tokens1 = tokens.replace(QRegExp(" +"),"");
    myTokens << tokens1.split(",");
    myPlaceName = label;
}

/*!
 * Return tokens.
 */
QStringList Place::tokens() const
{
    return myTokens;
}

/*!
 *  Add arc to hash table.
 */
void Place::addArc(Arc *arc)
{
    myArcs.insert(arc);
}

/*!
 * Remove arc from hash table.
 */
void Place::removeArc(Arc *arc)
{
    myArcs.remove(arc);
}

/*!
 * Return QRectF.
 */
QRectF Place::boundingRect() const
{
    return QRectF();
}

/*!
 * Paint.
 */
void Place::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
}
