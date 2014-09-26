/**
 * @file: setuser.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Set user dialog window.
 */
#include "setuser.h"
#include "ui_setuser.h"

/*!
 * Constructor, set ui texts.
 */
SetUser::SetUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetUser)
{
    ui->setupUi(this);
    this->setFixedSize(310,180);
    parentwidget = dynamic_cast<MainWindow *>(parent);
}

/*!
 * Destructor.
 */
SetUser::~SetUser()
{
    delete ui;
}

/*!
 * Close dialog.
 */
void SetUser::on_pushButton_2_clicked()
{
    this->close();
}

/*!
 * Connect with user to server.
 */
void SetUser::on_pushButton_clicked()
{
  user = ui->lineEdit->text();
  user_password = ui->lineEdit_2->text();

  /// must be same passwords
  if(ui->lineEdit_2->text() == ui->lineEdit_3->text())
  {
      parentwidget->set_user(this->user,this->user_password);

      parentwidget->doConnect("USERNEW");
      this->close();
  }
  else
  {
      QMessageBox msgBox;
      msgBox.setText("Password not match");
      msgBox.exec();
  }
}
/*** End of file setuser.cpp ***/
