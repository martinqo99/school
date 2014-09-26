/**
 * @file: about.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The about application dialog.
 */

#include "about.h"
#include "ui_about.h"

/*!
 * Constructor.
 */
About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setFixedSize(560,240);
}

/*!
 * Destructor.
 */
About::~About()
{
    delete ui;
}

/*!
 * Close about dialogg.
 */
void About::on_pushButton_clicked()
{
    this->close();
}
/*** End of file about.cpp ***/
