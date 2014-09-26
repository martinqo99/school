/**
 * @file: arcproperties.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Arc properties dialog header file.
 */
#ifndef ARCPROPERTIES_H
#define ARCPROPERTIES_H

#include <QDialog>
#include "arc.h"

namespace Ui {
class ArcProperties;
}

class ArcProperties : public QDialog
{
    Q_OBJECT
    
public:
    explicit ArcProperties(Arc *arc,QWidget *parent = 0);
    ~ArcProperties();
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Arc *arc;
    Ui::ArcProperties *ui;
};

#endif // ARCPROPERTIES_H
/*** End of file arcproperties.h ***/
