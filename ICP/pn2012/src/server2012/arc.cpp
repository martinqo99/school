/**
 * @file: arc.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Arc class of petri net.
*/

#include <QtGui>
#include "arc.h"
#include <QDebug>
#include <math.h>

/// PI constant
#define PINUMBER 3.141516

/*!
 * A constructor.
 */
Arc::Arc(Transition *trans, Place *plac, int direction, QString uniquename)
{
    this->direction = direction;
    transarc = trans;
    placearc = plac;
    labelarc = "x";
    uniqueName = uniquename;
    transarc->addArc(this);
    placearc->addArc(this);
}

/*!
 * A destructor.
 */
Arc::~Arc()
{
    transarc->removeArc(this);
    placearc->removeArc(this);
}

/*!
 * Set directions of arc.
 */
void Arc::setDirection(int direction)
{
    this->direction = direction;
}

/*!
 * Set label of arc.
 */
void Arc::setLabel(QString label)
{
    this->labelarc = label;
}

/*!
 * Get a transition that is associated with arc.
 */
Transition *Arc::toTransition() const
{
    return transarc;
}

/*!
 * Get a place that is associated with arc.
 */
Place *Arc::toPlace() const
{
    return placearc;
}

/*!
 * Remove arc from transition and place.
 */
void Arc::removeArc()
{
    transarc->removeArc(this);
    placearc->removeArc(this);
}
