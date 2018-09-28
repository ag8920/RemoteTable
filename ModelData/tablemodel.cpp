//------------------------------------------------------------------------------
//     Данный модуль создает табличную модель предстваления данных,
//     реализуя технологию "Модель-Представление"
//     Автор: Щербаков Александр
//     дата создания: 20.07.2018
//
//------------------------------------------------------------------------------

#include "tablemodel.h"


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
    myHeader<<tr("Значение");
    varValue = new QList<QString>;
    varName = new  QList<QString>;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TableModel::~TableModel()
{
    varValue->clear();
    varName->clear();
    delete varValue;
    delete varName;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int TableModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid() || varName->count()==0)
          return 0;
      else
          return varName->count();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    if(myHeader.count())
           return myHeader.count();
       else
           return 0;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || !varName->count())
            return QVariant();
//    if(role == Qt::TextColorRole)
//        return QColor(Qt::white);
    if(role == Qt::DisplayRole || role==Qt::EditRole)
            return varValue->at(index.row());
        return QVariant();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool TableModel::setData(const QModelIndex &index, const QVariant &value,
                         int role)
{
    if(index.isValid() && (role==Qt::EditRole || role==Qt::DisplayRole))
        {
            //varValue->replace(index.row(),value.toString());
            (*varValue)[index.row()]=value.toString();
            emit dataChanged(index,index);
            return true;
        }
        return false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if(role !=Qt::DisplayRole) return QVariant();
        if(orientation == Qt::Horizontal)
                return myHeader.at(section);
        if(orientation == Qt::Vertical)
                return varName->at(section);
        return QAbstractTableModel::headerData(section,orientation,role);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid() || !varName->count()) return nullptr;
       return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool TableModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TableModel::loadData(QList<QString> *value, QList<QString> *name)
{
    this->beginResetModel(); //сбрасываем таблицу для обновления данных таблицы

    //this->varName->clear();
    //this->varValue->clear();

    this->varValue=value;
    this->varName=name;

    this->endResetModel(); //окончание beginResetModel

}



//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
MyDelegate::MyDelegate(QObject *parent):QItemDelegate(parent)
{

}
//------------------------------------------------------------------------------
//Назначение: создает элемент редактирования ячейки таблицы
//------------------------------------------------------------------------------
QWidget *MyDelegate::createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);

    QLineEdit *editor = new QLineEdit(parent);
    QDoubleValidator *m_doubleValidator = new QDoubleValidator;
    m_doubleValidator->setNotation(QDoubleValidator::StandardNotation); //0,1232...
//     m_doubleValidator->setNotation(QDoubleValidator::ScientificNotation); // 1,232e-1...
    m_doubleValidator->setLocale(QLocale::English);
    editor->installEventFilter(const_cast<MyDelegate*>(this));
    editor->setValidator(m_doubleValidator);

//    QPalette palette;
//    palette.setColor(QPalette::Base,Qt::green);
//    palette.setColor(QPalette::Background,Qt::blue);
//    editor->setPalette(palette);

    return editor;
}
//------------------------------------------------------------------------------
//Назначение: записивыет данные из модели в элемент редактирования
//------------------------------------------------------------------------------

void MyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString str=index.model()->data(index,Qt::EditRole).toString();
    QLineEdit *le=static_cast<QLineEdit*>(editor);
    le->setText(str);

}
//------------------------------------------------------------------------------
//Назначение: возвращает обновленные данные в модель
//------------------------------------------------------------------------------

void MyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                              const QModelIndex &index) const
{
    QLineEdit *le=static_cast<QLineEdit*>(editor);
    QString str=le->text();
    model->setData(index,str);
}

//------------------------------------------------------------------------------
//Назначение: задает размеры и положение элемента редактирования
//------------------------------------------------------------------------------
void MyDelegate::updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect.adjusted(-1,-1,1,1));
}
