/**
 * @file: placeproperties.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Place properties dialog.
 */
#include "placeproperties.h"
#include "ui_placeproperties.h"
#include "place.h"

/*!
 * Constructor.
 */
PlaceProperties::PlaceProperties(Place *place,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceProperties)
{
    ui->setupUi(this);
    this->setWindowTitle("Properties");
    this->place = place;
    ui->lineEdit->setText(place->label());
    ui->lineEdit_2->setText(place->tokens());
}

/*!
 * Destructor.
 */
PlaceProperties::~PlaceProperties()
{
    delete ui;
}

/*!
 * Close properties dialog.
 */
void PlaceProperties::on_pushButton_2_clicked()
{
    this->close();
}

/*!
 * Close and set new properties.
 */
void PlaceProperties::on_pushButton_clicked()
{
    place->setText(ui->lineEdit->text(),ui->lineEdit_2->text());
    this->close();
}
/*** End of file placeproperties.cpp ***/
