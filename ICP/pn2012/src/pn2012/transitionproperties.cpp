/**
 * @file: transitionproperties.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The transition properties dialog.
 */
#include "transitionproperties.h"
#include "ui_transitionproperties.h"
#include "transition.h"

/*!
 * Constructor.
 */
TransitionProperties::TransitionProperties(Transition *transition,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransitionProperties)
{
    ui->setupUi(this);
    this->setWindowTitle("Properties");
    this->transition = transition;
    ui->lineEdit_3->setText(transition->name());
    ui->lineEdit_2->setText(transition->operation());
    ui->lineEdit->setText(transition->condition());
}

/*!
 * Destructor.
 */
TransitionProperties::~TransitionProperties()
{
    delete ui;
}

/*!
 * Exit dialog.
 */
void TransitionProperties::on_pushButton_2_clicked()
{
    this->close();
}

/*!
 * Close dialog and set transition properties.
 */
void TransitionProperties::on_pushButton_clicked()
{
    transition->setText(ui->lineEdit->text(),ui->lineEdit_2->text(),ui->lineEdit_3->text());
    this->close();
}
/*** End of file transitionproperties.cpp ***/
