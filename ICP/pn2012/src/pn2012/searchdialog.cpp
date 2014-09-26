/**
 * @file: searchdialog.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The search dialog window.
 */
#include "searchdialog.h"
#include "ui_searchdialog.h"
#include <QMessageBox>
#include <QDomElement>

/*!
 * Constructor.
 */
SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    this->setFixedSize(702,411);

    //connect(ui->loadfromserver, SIGNAL(clicked()), SLOT(sendXML()));

    parentwidget = dynamic_cast<MainWindow *>(parent);

    filterSiteXml();
    /// removes initial tabs
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(0);

    /// set table model from "tablemodel.h", search
    searchmodel = new TableModel(this,list.count(),6,list);
    searchview = new QTableView(this);
    searchview->setModel(searchmodel);

    /// design search layout/or table view
    searchview->verticalHeader()->setDefaultSectionSize(20);
    searchview->setStyleSheet("QTableView {selection-background-color: #f3f3f3; selection-color: #000000;}");
    searchview->verticalHeader()->setVisible(false);
    searchview->setSelectionMode(QAbstractItemView::SingleSelection);
    searchview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabWidget->addTab(searchview,"Search");

//    QModelIndexList f =  searchview->selectedIndexes();
//    /// set table model, history
//    historymodel = new TableModel(this,2,5);
//    historyview = new QTableView(this);
//    historyview->setModel(historymodel);

//    /// set design/layout of history table
//    historyview->verticalHeader()->setDefaultSectionSize(20);
//    historyview->setStyleSheet("QTableView {selection-background-color: #f3f3f3; selection-color: #000000;}");
//    historyview->verticalHeader()->setVisible(false);
//    historyview->setSelectionMode(QAbstractItemView::SingleSelection);
//    historyview->setSelectionBehavior(QAbstractItemView::SelectRows);
//    ui->tabWidget->addTab(historyview,"History");

    QStringList combo ;
    combo << "Author" << "Net Name" << "Version" << "Time Simulation" << "Login Simulation" << " ";
    ui->comboBox->addItems(combo);
}

/*!
 * Destructor. Remove items from qlist.
 */
SearchDialog::~SearchDialog()
{
    foreach(QList<QString> * item,list)
    {
        delete item;
    }
    list.clear();
    delete ui;
}

/*!
 * Close this dialog.
 */
void SearchDialog::on_pushButton_2_clicked()
{
    this->close();
}

/*!
 * Load selected site/row from server.
 */
void SearchDialog::on_loadfromserver_clicked()
{
    QModelIndexList indexes = searchview->selectionModel()->selectedRows();
    if(indexes.count() == 0)
    {
        QMessageBox::information(this,"Select error","You must select site!");
        return;
    }
    int row = indexes.at(0).row();

    QString namesite = list.at(row)->at(1);
    parentwidget->filename = namesite;
    parentwidget->doConnect("LOADXML");
    this->close();
}

/*!
 * Parse site xml, and load table.
 */
void SearchDialog::filterSiteXml()
{
    /// open file
    QFile file(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
    QDomDocument doc;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
    }
    else
        return;

    QDomElement root = doc.documentElement();
    if( root.tagName() != "Sites" )
    {
        QMessageBox::warning(this,"Error","Wrong format xml file!");
        return;
    }

    int site = 0;
    QDomNode n = root.firstChild();

    /// parse xml
    while( !n.isNull() )
    {
      site++;

      QDomElement e = n.firstChildElement();
      QList<QString> * infoList = new QList<QString>();
      while( !e.isNull())
      {

        if(e.tagName() == "Author")
        {
            infoList->append(e.text());
        }
        else if(e.tagName() == "Site")
        {
            infoList->append(e.text());
        }
        else if(e.tagName() == "Version")
        {
            infoList->append(e.text());
        }
        else if(e.tagName() == "Time_Simulation")
        {
            infoList->append(e.text());
        }
        else if(e.tagName() == "Login_Simulation")
        {
            infoList->append(e.text());
        }
        else if(e.tagName() == "Description")
        {
            infoList->append(e.text());
        }
        e = e.nextSiblingElement();
      }
      list.append(infoList);

      n = n.nextSibling();
    }

    if( site == 0 )
    {
        file.close();
        QMessageBox::warning(this,"Server","No sites on server!");
        return;
    }

    /// show newest sites upper
    for(int i = list.count()-1;  i >= 0; i--)
    {
        reverse_list.append(list.at(i));
    }

    list.clear();
    list = reverse_list;

    file.close();
}

/*!
 *  Set the table.
 */
void SearchDialog::on_pushButton_clicked()
{
    find_list.clear();
    QString combo = ui->comboBox->currentText();
    QString search_text;
    search_text = ui->lineEdit->text().toLower();

    qDebug() << search_text;

    for(int i = 0; i < list.count(); i++)
    {
        if(combo == "Author")
        {
            QString text = list.at(i)->at(0);
            text.toLower();
            if(text.contains(search_text))
            {
                find_list.append(list.at(i));
            }
        }
        else if(combo == "Net Name")
        {
            QString text = list.at(i)->at(1);
            text.toLower();
            if(text.contains(search_text))
            {
                find_list.append(list.at(i));
            }

        }
        else if(combo == "Version")
        {
            QString text = list.at(i)->at(2);
            text.toLower();
            if(text.contains(search_text))
            {
                find_list.append(list.at(i));
            }

        }
        else if(combo == "Time Simulation")
        {
            QString text = list.at(i)->at(3);
            text.toLower();
            if(text.contains(search_text))
            {
                find_list.append(list.at(i));
            }

        }
        else if(combo == "Login Simulation")
        {
            QString text = list.at(i)->at(4);
            text.toLower();
            if(text.contains(search_text))
            {
                find_list.append(list.at(i));
            }

        }
        else if(combo == " ")
        {
            find_list.append(list.at(i));
        }
    }

    ui->tabWidget->removeTab(0);

    searchmodel = new TableModel(this,find_list.count(),6,find_list);
    searchview = new QTableView(this);
    searchview->setModel(searchmodel);

    /// design search layout/or table view
    searchview->verticalHeader()->setDefaultSectionSize(20);
    searchview->setStyleSheet("QTableView {selection-background-color: #f3f3f3; selection-color: #000000;}");
    searchview->verticalHeader()->setVisible(false);
    searchview->setSelectionMode(QAbstractItemView::SingleSelection);
    searchview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabWidget->addTab(searchview,"Search");

}
/*** End of file searchdialog.cpp ***/
