/**
 * @file: customgraphicsscene.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The main function.
 */

#include "customgraphicsscene.h"
#include <QPainter>

// source code from
// http://tufangorel.blogspot.com/2011/08/draw-grid-on-qgraphicsscene.html
static const int GRID_STEP = 25;

inline qreal round(qreal val, int step)
{
   int tmp = int(val) + step /2;
   tmp -= tmp % step;
   return qreal(tmp);
}

/*!
 * Constructor
 */
CustomGraphicsScene::CustomGraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

/*!
 * Draw grid in background.
 */
void CustomGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{
   int step = GRID_STEP;
   painter->setPen(QPen(QColor(240, 240, 240, 255)));

   /// draw horizontal grid
   qreal start = round(rect.top(), step);
   if (start > rect.top())
      start -= step;

   for (qreal y = start - step; y < rect.bottom(); )
   {
      y += step;
      painter->drawLine(rect.left(), y, rect.right(), y);
   }

   /// draw vertical grid
   start = round(rect.left(), step);
   if (start > rect.left())
      start -= step;

   for (qreal x = start - step; x < rect.right(); )
   {
      x += step;
      painter->drawLine(x, rect.top(), x, rect.bottom());
   }
}
/*** End of file customgraphicsscene.cpp ***/
