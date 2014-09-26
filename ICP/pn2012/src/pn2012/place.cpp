/**
 * @file: about.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Place class of petri net.
*/

#include "arc.h"
#include "place.h"
#include <QInputDialog>
#include <QApplication>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "placeproperties.h"


#define SIMULATION 1
#define EDITING 2

/*!
 * Constructor.
 */
Place::Place(QColor colortext,QColor coloroutline, QColor colorbackground)
{
    myTextColor = colortext;
    myOutlineColor = coloroutline;
    myBackgroundColor = colorbackground;
    mymode = EDITING;
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges | ItemSendsGeometryChanges);
    this->myshape = 0;// 0 is ellipse and 1 is triangle
    this->myfont = 1;// 1 is arial 0 is times new roman
}

/*!
 * Destructor, delete all arcs.
 */
Place::~Place()
{
    foreach(Arc *arc,myArcs)
        delete arc;
}

/*!
 * Set new text(tokens and label).
 */
void Place::setText(const QString &label, const QString &tokens)
{
    prepareGeometryChange();
    myTokens = tokens;
    myPlaceName = label;
    update();
}

/*!
 * Return tokens from place.
 */
QString Place::tokens() const
{
    return myTokens;
}

/*!
 * Set outline color.
 */
void Place::setOutlineColor(const QColor &color)
{
    myOutlineColor = color;
    update();
}

/*!
 * Return outline color.
 */
QColor Place::outlineColor() const
{
    return myOutlineColor;
}

/*!
 * Set background color.
 */
void Place::setBackgroundColor(const QColor &color)
{
    myBackgroundColor = color;
    update();
}

/*!
 * Return background color.
 */
QColor Place::backgroundColor() const
{
    return myBackgroundColor;
}

/*!
 * Add arc to list.
 */
void Place::addArc(Arc *arc)
{
    myArcs.insert(arc);
}

/*!
 * Set colors for place.
 */
void Place::setColors(QColor out, QColor text, QColor back)
{
    myOutlineColor = out;
    myTextColor = text;
    myBackgroundColor = back;
    update();
}

/*!
 * Remove arc from list.
 */
void Place::removeArc(Arc *arc)
{
    myArcs.remove(arc);
}

/*!
 * Set shape and font for place.
 */
void Place::setShapeFont(int shape,int font)
{
    this->myshape = shape;
    this->myfont = font;
    update();
}

/*!
 * Return bounding rect.
 */
QRectF Place::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin,-Margin,+Margin,+Margin);
}

/*!
 * Set path of shape.
 */
QPainterPath Place::shape() const
{
    QRectF rect = outlineRect();
    QPainterPath path;
    path.addRoundedRect(rect,roundness(rect.width()),
                        roundness(rect.height()));
    return path;
}

/*!
 * Paint place to qgraphicsscene.
 */
void Place::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(myOutlineColor);
    if(option->state & QStyle::State_Selected)
    {
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

    /// if is triangle
    if(myshape)
    {
        painter->setPen(pen);
        painter->setBrush(myBackgroundColor);
        QRectF rect = outlineRect();
        QPainterPath path = shape();
        QPolygonF Triangle;
        Triangle.append(QPointF(-40,0));
        Triangle.append(QPointF(40,0));
        Triangle.append(QPointF(0,-40));
        myPolygon = Triangle;
        painter->drawPolygon(Triangle);
        painter->setPen(myTextColor);
        painter->drawText(rect.x()+24,rect.y()+24,myTokens);
        painter->drawText(rect.x()+5,rect.y()+8,myPlaceName);
    }
    /// else if ellipse(default)
    else
    {
        painter->setPen(pen);
        painter->setBrush(myBackgroundColor);
        QRectF rect = outlineRect();
        QPainterPath path = shape();
        myPolygon = path.toFillPolygon();
        painter->drawEllipse(rect);
        painter->setPen(myTextColor);
        painter->drawText(rect,Qt::AlignCenter,myTokens);
        painter->drawText(rect.x(),rect.y()-7,myPlaceName);
    }
}

/*!
 * Set flags for graphicsitem.
 */
void Place::setCustomFlags(int mode)
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
 * Doubleclick event on place show Properties dialog.
 */
void Place::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(mymode == EDITING)
    {
        PlaceProperties dialog(this);
        dialog.exec();
    }
}

/*!
 * When item change => change arcs.
 */
QVariant Place::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionHasChanged)
    {
        foreach(Arc *arc, myArcs)
            arc->trackNodes();
    }
    return QGraphicsItem::itemChange(change,value);
}

/*!
 * Return outlineRect.
 */
QRectF Place::outlineRect() const
{
    const int Padding = 8;
    QFontMetrics metrics = qApp->font();
    int width = metrics.boundingRect(myTokens).width() - 3;
    if(width < 21) width = 21;
    if(width > 45) width = 45;

    if(myshape)
        width = 60;

    QRectF rect = QRectF(metrics.boundingRect(myTokens).topLeft(),QSize(width,width));
    rect.adjust(-Padding,-Padding,+Padding,+Padding);
    rect.translate(-rect.center());
    return rect;
}

/*!
 * Set roundness of place.
 */
int Place::roundness(double size) const
{
    const int Diameter = 5;
    return 100 * Diameter / int(size);
}
/*** End of file place.cpp ***/
