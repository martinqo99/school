/**
 * @file: customhraphicsscene.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for custom graphics scene.
 */
#ifndef CUSTOMGRAPHICSSCENE_H
#define CUSTOMGRAPHICSSCENE_H

#include <QGraphicsScene>

class CustomGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CustomGraphicsScene(QObject *parent = 0);
    
signals:

protected:
       void drawBackground(QPainter * painter, const QRectF & rect );

public slots:
    
};

#endif // CUSTOMGRAPHICSSCENE_H
/*** End of file customgraphicsscene.h ***/
