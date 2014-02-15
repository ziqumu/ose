#include "tablewidgetitemdelegate.h"


TableWidgetItemDelegate::TableWidgetItemDelegate(QAbstractItemView* view, QWidget *parent) : QStyledItemDelegate(parent)
{
    this->view = view;
}


void TableWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if(index.column() == 2)
    {


        QStylePainter stylePainter(view);
        stylePainter.setBackground(QBrush(QColor(QApplication::palette().color(QPalette::Highlight))));

        //QTableWidget *p = (QTableWidget*)parent;

        painter->save();
       /*QTextDocument doc;
       doc.setHtml("<b>" + index.data().toString() + "</b>");
       doc.setTextWidth(option.rect.size().width());
       QAbstractTextDocumentLayout::PaintContext context;
       painter->translate(option.rect.x(), option.rect.y());
       doc.documentLayout()->draw(painter, context);

*/

       painter->setBackground(QBrush(QColor(QApplication::palette().color(QPalette::Highlight))));

       painter->restore();
    // painter->drawText(option.rect, Qt::AlignLeft | Qt::TextWordWrap, index.data().toString());

        //QStyledItemDelegate::paint(painter, option, index);
        /*
        if (index.data().canConvert<StarRating>()) {
            StarRating starRating = qvariant_cast<StarRating>(index.data());

            if (option.state & QStyle::State_Selected)
                painter->fillRect(option.rect, option.palette.highlight());

            starRating.paint(painter, option.rect, option.palette,
                             StarRating::ReadOnly);
        } else {
            QStyledItemDelegate::paint(painter, option, index);
        }*/

      }
    else
    { painter->setBackground(QBrush(QColor(QApplication::palette().color(QPalette::Highlight))));

        QStyledItemDelegate::paint(painter, option, index);
        painter->setBackground(QBrush(QColor(QApplication::palette().color(QPalette::Highlight))));
    }
}

QSize TableWidgetItemDelegate::sizeHint(QObject *parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
QTableWidget *p = (QTableWidget*)parent;
QFontMetrics fm(p->font());

float rw = float(p->viewport()->size().width());
float tw = fm.width("<b>" + index.data().toString() + "</b>");
float ratio = tw/rw;
int lines = 0;
if(ratio - int(ratio) < 0.1f)
lines = int(ratio);
else
lines = int(ratio) + 1;
return QSize(rw,lines*fm.height());
}
