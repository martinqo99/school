/**
 * @file: tablemodel.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for table.
 */
#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QVariant>
#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(QObject *parent,int rows,int columns, QList <QList<QString> *> list);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    int rows;
    int columns;
    QList <QList<QString> *> list;
};

#endif // TABLEMODEL_H
/*** End of file tablemodel.h ***/
