/**
 * @file: transition.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Transition class of petri net.
*/

#include <QtGui>
#include <QApplication>
#include "transition.h"
#include "arc.h"

#define SIMULATION 1
#define EDITING 2

/*!
 * A constructor.
 */
Transition::Transition()
{
}

/*!
 * A destructor.
 */
Transition::~Transition()
{
    foreach(Arc *arc, myArcs)
        delete arc;
}

/*!
 * Add arc to hash table.
 */
void Transition::addArc(Arc *arc)
{
    myArcs.insert(arc);
}

/*!
 * Remove arc from hash table.
 */
void Transition::removeArc(Arc *arc)
{
    myArcs.remove(arc);
}

/*!
 * Get condition, operation and name from transition.
 */
void Transition::setText(const QString &condition,const QString &operation, const QString &name)
{
    myCondition = condition;
    myOperation = operation;
    myName = name;
}

/*!
 * Append condition.
 */
void Transition::appendCondition(QString cond)
{
    myCondition.append(cond);
}

/*!
 * Return QRectF.
 */
QRectF Transition::boundingRect() const
{
    return QRectF();
}

/*!
 * Paint.
 */
void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
}
