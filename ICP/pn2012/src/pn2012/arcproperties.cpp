/**
 * @file: arcproperties.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The main function.
 */

#include "arcproperties.h"
#include "ui_arcproperties.h"

/*!
 * Constructor.
 */
ArcProperties::ArcProperties(Arc *arc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArcProperties)
{
    ui->setupUi(this);
    this->arc = arc;
    ui->lineEdit->setText(arc->getLabel());
    if(arc->getDirection() == 1)
    {
        ui->radioButton->setChecked(true);
    }
    else
    {
        ui->radioButton_3->setChecked(true);
    }
}

/*!
 * Destructor.
 */
ArcProperties::~ArcProperties()
{
    delete ui;
}

/*!
 * Close this dialog.
 */
void ArcProperties::on_pushButton_clicked()
{
    this->close();
}

/*!
 * Set and return arc properties, also close dialog.
 */
void ArcProperties::on_pushButton_2_clicked()
{
    if(ui->radioButton->isChecked())
        arc->setDirection(1);
    else
        arc->setDirection(0);
    arc->setLabel(ui->lineEdit->text());
    this->close();
}
/*** End of file arcproperties.cpp ***/
