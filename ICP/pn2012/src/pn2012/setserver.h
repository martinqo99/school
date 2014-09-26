/**
 * @file: setserver.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for set server dialog.
 */
#ifndef SETSERVER_H
#define SETSERVER_H

#include <QDialog>
#include "mainwindow.h"

class MainWindow;

namespace Ui {
    class SetServer;
}

class SetServer : public QDialog
{
    Q_OBJECT

public:
    explicit SetServer(QString serv,QString log, QString pass, int port,QWidget *parent = 0);
    ~SetServer();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::SetServer *ui;
    MainWindow *parentwidget;
    /// login information
    QString server;
    int port;
    QString login;
    QString password;

};

#endif // SETSERVER_H
/*** End of file setserver.h ***/
