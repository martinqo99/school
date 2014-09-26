/**
 * @file: visualoptions.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The visualoptions header dialog.
 */
#ifndef VISUALOPTIONS_H
#define VISUALOPTIONS_H

#include <QDialog>
#include <QColor>
#include <QLabel>
#include "mainwindow.h"

namespace Ui {
class VisualOptions;
}

class VisualOptions : public QDialog
{
    Q_OBJECT
    
public:
    explicit VisualOptions(QWidget *parent,QColor transout,QColor transtxt,QColor transback,QColor placout,QColor plactxt,QColor placback,QColor line,int font,int shape);
    ~VisualOptions();
    void updateColorLabel(QLabel *label, const QColor &color);
    void chooseColor(QLabel *label, QColor *color);

private slots:
    void on_buttransout_clicked();
    void on_buttoncancel_clicked();
    void on_buttransbackgr_clicked();
    void on_buttontranstext_clicked();
    void on_butplaceout_clicked();
    void on_butplacebackg_clicked();
    void on_butplacetext_clicked();
    void on_butarcline_clicked();
    void on_buttonselect_clicked();
    void on_buttonload_clicked();
    void on_buttonsave_clicked();

private:
    MainWindow *parentwidget;
    void saveFile(QString);
    Ui::VisualOptions *ui;
    /// colors of actual visual options
    QColor transbackground;
    QColor transoutline;
    QColor transtext;
    QColor placeoutline;
    QColor placebackground;
    QColor placetext;
    QColor arcline;
    /// type of font
    QString font;
    /// type of shape
    QString shape;
};

#endif // VISUALOPTIONS_H
/*** End of file visualoptions.h ***/
