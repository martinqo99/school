/**
 * @file: searchdialog.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header for search dialog window.
 */
#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QStringListModel>
#include <QMessageBox>
#include "tablemodel.h"
#include "mainwindow.h"

class MainWindow;

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SearchDialog(QWidget *parent = 0);
    ~SearchDialog();
    
private slots:
    void on_pushButton_2_clicked();
    void on_loadfromserver_clicked();
    void on_pushButton_clicked();

private:
    Ui::SearchDialog *ui;
    MainWindow *parentwidget;
    /// tables
    TableModel *searchmodel;
    TableModel *historymodel;
    QTableView *searchview;
    QTableView *historyview;
    void filterSiteXml();
    QList <QList<QString> *> list;
    QList <QList<QString> *> reverse_list;
    QList <QList<QString> *> find_list;
};

#endif // SEARCHDIALOG_H
/*** End of file searchdialog.h ***/
