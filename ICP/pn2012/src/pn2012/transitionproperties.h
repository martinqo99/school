/**
 * @file: transitionproperties.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for transition properties dialog.
 */
#ifndef TRANSITIONPROPERTIES_H
#define TRANSITIONPROPERTIES_H

#include <QDialog>
#include "transition.h"

namespace Ui {
class TransitionProperties;
}

class TransitionProperties : public QDialog
{
    Q_OBJECT
    
public:
    explicit TransitionProperties(Transition *transition,QWidget *parent = 0);
    ~TransitionProperties();
    
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::TransitionProperties *ui;
    Transition *transition;
};

#endif // TRANSITIONPROPERTIES_H
/*** End of file transitionproperties.h ***/
