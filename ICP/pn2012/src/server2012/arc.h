/**
 * @file: arc.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Arc class of petri net.
*/

#ifndef ARC_H
#define ARC_H

#include "transition.h"
#include "place.h"
#include <QGraphicsLineItem>

class Transition;
class Place;

class Arc : public QGraphicsLineItem
{
public:
    Arc(Transition *trans, Place *plac, int direction, QString uniquename);
    ~Arc();
    int getDirection(){return direction;} ///get direction of arc
    void setDirection(int direction); ///set direction of arc
    QString getLabel(){return labelarc;}  ///get label of arc
    QString getName(){return uniqueName;} ///get name of arc
    void setLabel(QString label);   ///set label of arc
    Transition *toTransition() const;  ///get a trancition that is associated with arc
    Place *toPlace() const;  ///get a place that is associated with arc
    void removeArc();  ///remove arc
    Transition *getTransition(){return transarc;} ///get transition
    Place *getPlace(){return placearc;}  ///get place

private:
    QString uniqueName;  ///uniqueName of arc
    Transition *transarc; ///transition
    Place *placearc; ///place
    int direction; ///0 from transition to place
    QString labelarc; ///label of arc
};

#endif // ARC_H
