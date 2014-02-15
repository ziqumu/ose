#ifndef TABLEWIDGETITEMDELEGATE_H
#define TABLEWIDGETITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QDebug>
#include <QTextDocument>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QTableWidget>
#include <QApplication>
#include <QPalette>
#include <QStylePainter>

class TableWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    QAbstractItemView *view;

    TableWidgetItemDelegate(QAbstractItemView* view, QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(QObject *parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /*QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;*/

private slots:
    //void commitAndCloseEditor();
};

#endif // TABLEWIDGETITEMDELEGATE_H
