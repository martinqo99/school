/**
 * @file: setuser.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for set user dialog.
 */
#ifndef SETUSER_H
#define SETUSER_H

#include <QDialog>
#include "mainwindow.h"

class MainWindow;

namespace Ui {
    class SetUser;
}

class SetUser : public QDialog
{
    Q_OBJECT

public:
    explicit SetUser(QWidget *parent = 0);
    ~SetUser();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::SetUser *ui;
    MainWindow *parentwidget;
    QString user;
    QString user_password;
};

#endif // SETUSER_H
/*** End of file setuser.h ***/
