/**
 * @file: placeproperties.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for place properties dialog.
 */
#ifndef PLACEPROPERTIES_H
#define PLACEPROPERTIES_H

#include <QDialog>
#include "place.h"

namespace Ui {
class PlaceProperties;
}

class PlaceProperties : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlaceProperties(Place *place,QWidget *parent = 0);
    ~PlaceProperties();
    
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::PlaceProperties *ui;
    Place *place;
};

#endif // PLACEPROPERTIES_H
/*** End of file placeproperties.h ***/
