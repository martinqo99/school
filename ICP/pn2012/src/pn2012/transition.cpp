/**
 * @file: transition.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Transition class which is connected with arc to place.
 */
#include <QtGui>
#include <QApplication>
#include "transition.h"
#include "arc.h"
#include "transitionproperties.h"

#define SIMULATION 1
#define EDITING 2

/*!
 * Constructor, set flags and colors.
 */
Transition::Transition(QColor text,QColor outline, QColor background)
{
    myfont = 1;
    myTextColor = text;
    myOutlineColor = outline;
    myBackgroundColor = background;
    mymode = EDITING;
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges | ItemSendsGeometryChanges);
}

/*!
 * Destructor, remove arcs.
 */
Transition::~Transition()
{
    foreach(Arc *arc, myArcs)
        delete arc;
}

/*!
 * Add arc to transition.
 */
void Transition::addArc(Arc *arc)
{
    myArcs.insert(arc);
}

/*!
 * Remove specified arc.
 */
void Transition::removeArc(Arc *arc)
{
    myArcs.remove(arc);
}

/*!
 * Set flags for simulation.
 */
void Transition::setCustomFlags(int mode)
{
    if(mode == SIMULATION)
    {
        mymode = SIMULATION;
        this->setFlags(ItemIsSelectable | ItemSendsScenePositionChanges | ItemSendsGeometryChanges);
    }
    else
    {
        mymode = EDITING;
        this->setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges | ItemSendsGeometryChanges);
    }
    update();
}

/*!
 * Set font for transition
 */
void Transition::setFont(int font)
{
    this->myfont = font;
    update();
}

/*!
 * Set text for transition.
 * Set condition and operation.
 */
void Transition::setText(const QString &condition,const QString &operation, const QString &name)
{
    prepareGeometryChange();
    myCondition = condition;
    myOperation = operation;
    myName = name;
    update();
}

/*!
 * Set condition.
 */
QString Transition::condition() const
{
    return myCondition;
}

/*!
 * Set outline color for transition.
 */
void Transition::setOutlineColor(const QColor &color)
{
    myOutlineColor = color;
    update();
}

/*!
 * Get outline color.
 */
QColor Transition::outlineColor() const
{
    return myOutlineColor;
}

/*!
 * Set background color.
 */
void Transition::setBackgroundColor(const QColor &color)
{
    myBackgroundColor = color;
    update();
}

/*!
 * Get background color.
 */
QColor Transition::backgroundColor() const
{
    return myBackgroundColor;
}

/*!
 * Set colors of transition.
 */
void Transition::setColors(QColor outline, QColor text,QColor background)
{
    myTextColor = text;
    myBackgroundColor = background;
    myOutlineColor = outline;
    update();
}

/*!
 * Get bounding rect.
 */
QRectF Transition::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin,-Margin,+Margin,+Margin);
}

/*!
 * Get path of transition.
 */
QPainterPath Transition::shape() const
{
    QRectF rect = outlineRect();
    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

/*!
 * Paint transition.
 */
void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(myOutlineColor);
    if(option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
    }

    if(myfont == 1)
    {
        QFont f("Arial");
        painter->setFont(f);
    }
    else
    {
        QFont f("Times New Roman");
        painter->setFont(f);
    }


    painter->setPen(pen);
    painter->setBrush(myBackgroundColor);
    QRectF rect = outlineRect();
    QPainterPath path = shape();
    myPolygon = path.toFillPolygon();
    painter->drawRoundRect(rect,roundness(rect.width()),roundness(rect.height()));
    painter->drawLine(rect.x()+4,rect.bottomLeft().y()-rect.height()/2,rect.bottomLeft().x()+rect.width()-4,rect.bottomLeft().y()-rect.height()/2);
    painter->setPen(myTextColor);
    rect.setY(rect.y()+5);
    painter->drawText(rect,Qt::AlignTop|Qt::AlignHCenter,myCondition);
    rect.setHeight(rect.height()-8);
    painter->drawText(rect,Qt::AlignBottom|Qt::AlignHCenter,myOperation);
}

/*!
 * Show dialog for transition properties.
 */
void Transition::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(mymode == EDITING)
    {
        TransitionProperties dialog(this);
        dialog.exec();
    }
}

/*!
 * When item position change, track arcs.
 */
QVariant Transition::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange)
    {
        foreach(Arc * arc, myArcs)
            arc->trackNodes();
    }
    return QGraphicsItem::itemChange(change,value);
}

/*!
 * Get outline rectangle.
 */
QRectF Transition::outlineRect() const
{
    const int Padding = 10;
    QFontMetrics metrics = qApp->font();
    QRectF rect = metrics.boundingRect(myCondition);
    rect.setHeight(rect.height()*2.5);
    rect.adjust(-Padding,-Padding,+Padding,+Padding);
    rect.translate(-rect.center());

    QRectF rect2 = metrics.boundingRect(myOperation);
    if(rect2.width()>rect.width())
    {
        rect2.setHeight(rect2.height()*2.5);
        rect2.adjust(-Padding,-Padding,+Padding,+Padding);
        rect2.translate(-rect2.center());
        return rect2;
    }
    else
        return rect;
}

/*!
 * Get roundness.
 */
int Transition::roundness(double size) const
{
    const int Diameter = 12;
    return 100* Diameter / int(size);
}
/*** End of file transition.cpp ***/
