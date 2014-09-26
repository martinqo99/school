/**
 * @file: transition.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Transition class of petri net.
*/

#ifndef TRANSITION_H
#define TRANSITION_H
#include "arc.h"
#include <QString>
#include <QSet>
#include <QGraphicsItem>
class Arc;

class Transition : public QGraphicsItem
{
public:
    Transition();
    ~Transition();
    void addArc(Arc *arc);
    void removeArc(Arc *arc);
    void setText(const QString &condition, const QString &operation, const QString &name);
    QString label() {return myName;}   ///return label of transition
    QString condition() {return myCondition;}   ///return condition of transition
    QString operation() {return myOperation;}   ///return operation of transition
    void appendCondition(QString cond);
    QString name() {return myName;}  ///return name of transition
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QSet<Arc *> myArcs;  ///hash table

private:
    QString myCondition;  ///condition of transition
    QString myOperation;  ///operation of transition
    QString myName;       ///name of transition
    int mymode;           ///mode of transition
};

#endif // TRANSITION_H
