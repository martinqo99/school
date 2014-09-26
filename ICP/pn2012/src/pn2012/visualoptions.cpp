/**
 * @file: visualoptions.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Visual options dialog window.
 */
#include "visualoptions.h"
#include "ui_visualoptions.h"
#include <QColorDialog>
#include <QFileDialog>

/*!
 * Constructor, set actual colors, fonts ...
 */
VisualOptions::VisualOptions(QWidget *parent,
                             QColor transout,QColor transtxt,QColor transback,QColor placout,QColor plactxt,QColor placback, QColor line,int font,int shape) :
    QDialog(parent),
    ui(new Ui::VisualOptions)
{
    ui->setupUi(this);
    this->setFixedSize(364,477);
    transoutline = transout;
    transtext = transtxt;
    transbackground = transback;
    placeoutline = placout;
    placebackground = placback;
    placetext = plactxt;
    arcline = line;
    parentwidget = dynamic_cast<MainWindow *>(parent);
    ui->comboBox->addItem("Ellipse");
    ui->comboBox->addItem("Triangle");
    ui->comboBox_2->addItem("Times New Roman");
    ui->comboBox_2->addItem("Arial");

    /// init font
    if(font == 0)
    {
        this->font = "Times New Roman";
        ui->comboBox_2->setCurrentIndex(0);
    }
    else
    {
        this->font = "Arial";
        ui->comboBox_2->setCurrentIndex(1);
    }

    /// init shape
    if(shape == 0) // 0 ellipse 1 triangle
        this->shape = "Ellipse";
    else
        this->shape = "Triangle";

    if(this->shape == "Ellipse")
        ui->comboBox->setCurrentIndex(0);
    else
        ui->comboBox->setCurrentIndex(1);

    /// update colors in ui
    updateColorLabel(ui->labtransoutline,transoutline);
    updateColorLabel(ui->labtranstext,transtext);
    updateColorLabel(ui->labtransbacground,transbackground);
    updateColorLabel(ui->labplaceout,placeoutline);
    updateColorLabel(ui->labplacebackg,placebackground);
    updateColorLabel(ui->labplacetext,placetext);
    updateColorLabel(ui->labarcline,arcline);
}

/*!
 * Destructor.
 */
VisualOptions::~VisualOptions()
{
    delete ui;
}

/*!
 * Update colors and labels.
 */
void VisualOptions::updateColorLabel(QLabel *label, const QColor &color)
{
    QPixmap pixmap(16,16);
    pixmap.fill(color);
    label->setPixmap(pixmap);
}

/*!
 * Set new color and update label.
 */
void VisualOptions::chooseColor(QLabel *label, QColor *color)
{
    QColor newcolor = QColorDialog::getColor(*color,this);
    if(newcolor.isValid())
    {
        *color = newcolor;
        updateColorLabel(label,*color);
    }
}

/*!
 * Choose color outline.
 */
void VisualOptions::on_buttransout_clicked()
{
    chooseColor(ui->labtransoutline,&transoutline);
}

/*!
 * Close this dialog.
 */
void VisualOptions::on_buttoncancel_clicked()
{
    this->close();
}

/*!
 * Choose color for background.
 */
void VisualOptions::on_buttransbackgr_clicked()
{
    chooseColor(ui->labtransbacground,&transbackground);
}

/*!
 * Choose text color.
 */
void VisualOptions::on_buttontranstext_clicked()
{
    chooseColor(ui->labtranstext,&transtext);
}

/*!
 *  Choose out color.
 */
void VisualOptions::on_butplaceout_clicked()
{
    chooseColor(ui->labplaceout,&placeoutline);
}

/*!
 * Choose background color of place.
 */
void VisualOptions::on_butplacebackg_clicked()
{
    chooseColor(ui->labplacebackg,&placebackground);
}

/*!
 * Choose place color text.
 */
void VisualOptions::on_butplacetext_clicked()
{
    chooseColor(ui->labplacetext,&placetext);
}

/*!
 * Choose arc line color.
 */
void VisualOptions::on_butarcline_clicked()
{
    chooseColor(ui->labarcline,&arcline);
}

/*!
 * Set font and shape.
 */
void VisualOptions::on_buttonselect_clicked()
{
    shape = ui->comboBox->currentText();
    font = ui->comboBox_2->currentText();
    parentwidget->setColors(transoutline,transtext,transbackground,placetext,placeoutline,placebackground,arcline,font,shape);
    this->close();
}

/*!
 * Load visual xml file.
 */
void VisualOptions::on_buttonload_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open visual settings xml file."),"","Xml files (*.xml);;All Files (*)");
    if(filename.isEmpty())
        return;

    QFile file(filename);
    QDomDocument document;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        if(!document.setContent(&file))
            return;
    }
    else
        return;

    QDomElement root = document.firstChildElement();

    /// transition colors
    QDomNodeList node = root.elementsByTagName("transition");
    QDomNode itemnode = node.at(0);
    QDomElement itemele = itemnode.toElement();
    QStringList list = itemele.text().split('#');
    transoutline = QColor("#" + list.at(1));
    transtext = QColor("#" + list.at(3));;
    transbackground = QColor("#" + list.at(2));
    list.clear();

    /// place colors
    node = root.elementsByTagName("place");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    list = itemele.text().split('#');
    placeoutline = QColor("#"+list.at(3));
    placebackground = QColor("#"+list.at(2));
    placetext = QColor("#"+list.at(1));
    list.clear();

    /// arc color
    node = root.elementsByTagName("arc");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    list = itemele.text().split('#');
    arcline = QColor("#"+list.at(1));

    /// font type
    node = root.elementsByTagName("font");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    font = itemele.text();

    /// shape type
    node = root.elementsByTagName("shape");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    shape = itemele.text();

    /// update font and shapes
    if(font == "Times New Roman")
        ui->comboBox_2->setCurrentIndex(0);
    else
        ui->comboBox_2->setCurrentIndex(1);

    if(shape == "Ellipse")
        ui->comboBox->setCurrentIndex(0);
    else
        ui->comboBox->setCurrentIndex(1);

    /// update labelcolors
    updateColorLabel(ui->labtransoutline,transoutline);
    updateColorLabel(ui->labtranstext,transtext);
    updateColorLabel(ui->labtransbacground,transbackground);
    updateColorLabel(ui->labplaceout,placeoutline);
    updateColorLabel(ui->labplacebackg,placebackground);
    updateColorLabel(ui->labplacetext,placetext);
    updateColorLabel(ui->labarcline,arcline);
}

/*!
 * Save xml visual file.
 */
void VisualOptions::on_buttonsave_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,"Save xml file","Xml file *.xml");
    if(filename.isEmpty())
        return;
    else
        saveFile(filename);
}

/*!
 * Save xml file.
 */
void VisualOptions::saveFile(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly |QIODevice::Text))
         return;

    QTextStream out(&file);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><visualoptions>";
    out << "<transition>";
    out << "<outline_color>";
    out << transoutline.name();
    out << "</outline_color>";
    out << "<background_color>";
    out << transbackground.name();
    out << "</background_color>";
    out << "<text_color>";
    out << transtext.name();
    out << "</text_color>";
    out << "</transition>";
    out << "<place>";
    out << "<outline_color>";
    out << placeoutline.name();
    out << "</outline_color>";
    out << "<background_color>";
    out << placebackground.name();
    out << "</background_color>";
    out << "<text_color>";
    out << placetext.name();
    out << "</text_color>";
    out << "</place>";
    out << "<arc>";
    out << "<line_color>";
    out << arcline.name();
    out << "</line_color>";
    out << "</arc>";
    out << "<font>";
    out << ui->comboBox_2->currentText();
    out << "</font>";
    out << "<shape>";
    out << ui->comboBox->currentText();
    out << "</shape>";
    out << "</visualoptions>";
    file.close();
}
/*** End of file visualoptions.cpp ***/
