#include "fileoptions.h"
#include "ui_fileoptions.h"

/*!
 * Constructor.
 */
FileOptions::FileOptions(QString name_site,QString version_site,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileOptions)
{
    ui->setupUi(this);
    parentwidget = dynamic_cast<MainWindow *>(parent);
    ui->lineEdit->setText(name_site);
    ui->lineEdit_2->setText(version_site);
    discription = "";
    namesite = "";
    versionsite = "";
}

/*!
 * Destructor.
 */
FileOptions::~FileOptions()
{
    delete ui;
}

/*!
 * Close the dialog.
 */
void FileOptions::on_pushButton_2_clicked()
{
    parentwidget->appmodepossible = 0;
    parentwidget->resultDialog = 0;
    this->close();
}

/*!
 * Accept and close dialog.
 */
void FileOptions::on_pushButton_clicked()
{
    namesite = ui->lineEdit->text();
    versionsite = ui->lineEdit_2->text();
    discription = ui->textEdit->toPlainText();
    parentwidget->appmodepossible = 1;
    parentwidget->resultDialog = 1;
    parentwidget->set_fileoptions(this->namesite, this->versionsite, this->discription);
    this->close();
}
/*** End of fileoptions.cpp ***/
