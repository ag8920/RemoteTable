#ifndef TABLEMODEL_H
#define TABLEMODEL_H


#include <QAbstractTableModel>
#include <QAbstractListModel>
#include <QItemDelegate>
#include <QtWidgets>
#include <QtGui>
#include <QVariant>
#include <QMap>
#include <QColor>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
   public:
       TableModel(QObject *parent=nullptr );
       ~TableModel();
       //количество строк
       int rowCount(const QModelIndex &parent=QModelIndex()) const;
       //количество столбцов
       int columnCount(const QModelIndex &parent=QModelIndex()) const;
       //функция для передачи данных пользоавтелю
       QVariant data(const QModelIndex &index, int role) const;
       //функция для приема данных от пользователя
       bool setData(const QModelIndex &index, const QVariant &value, int role);
       //название столбцов
       QVariant headerData(int section, Qt::Orientation orientation, int role) const;
       //разрешить форматирование
       Qt::ItemFlags flags(const QModelIndex &index) const;

       bool hasChildren(const QModelIndex &parent=QModelIndex()) const;

   public slots:
       void loadData(QList<QString> *value, QList<QString> *name);

   private:
       QList <QString> *varName;   //наименование элемента
       QList <QString> *varValue;  //значение элемента

       QStringList myHeader;       //название столбцов

       QMap<QString,QVariant> *map;

};

class MyDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    MyDelegate(QObject *parent=0);
    ~MyDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

};


#endif // TABLEMODEL_H
