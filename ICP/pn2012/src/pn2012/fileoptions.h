/**
 * @file: fileoptions.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for file options dialog.
 */
#ifndef FILEOPTIONS_H
#define FILEOPTIONS_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
    class FileOptions;
}

class FileOptions : public QDialog
{
    Q_OBJECT

public:
    explicit FileOptions(QString name_site,QString version_site,QWidget *parent = 0);
    ~FileOptions();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::FileOptions *ui;
    /// main window dialog object
    MainWindow *parentwidget;
    /// name of net
    QString namesite;
    /// version of net
    QString versionsite;
    /// description of net
    QString discription;
};

#endif // FILEOPTIONS_H
/*** End of file fileoptions.h ***/
