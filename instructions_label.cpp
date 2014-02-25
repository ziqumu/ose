#include "instructions_label.h"

InstructionsLabel::InstructionsLabel(QWidget *parent) : QLabel(parent)
{
    offset = 0xffffffff;
}


void InstructionsLabel::mouseDoubleClickEvent(QMouseEvent * ev)
{
    this->setStyleSheet("QLabel{background-color:white;}");
    if(offset != 0xffffffff)
        this->setTextInteractionFlags(Qt::TextEditorInteraction);
    else
        this->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    //Selection (get the length of the text after html)
        QTextDocument text;
        text.setHtml(this->text());
        this->setSelection(0,text.toPlainText().length());
    QLabel::mouseDoubleClickEvent(ev);
}

void InstructionsLabel::focusOutEvent(QFocusEvent *ev)
{
    this->setStyleSheet("QLabel{background-color:none;}");
    this->setTextInteractionFlags(Qt::NoTextInteraction);
    //if offset setted, we signal the value change
    if(offset != 0xffffffff)
        emit valueChanged(this->text());
    QLabel::focusOutEvent(ev);
}

void InstructionsLabel::keyPressEvent(QKeyEvent * ev)
{
    if(offset != 0xffffffff && ((ev->text().size() == 1 && this->value.size() <= 4
                && ((ev->text()[0].toLower() >= QChar('a') && ev->text()[0].toLower() <= QChar('f')) || ev->text()[0].isDigit()))
                || ev->key()==Qt::Key_Backspace || ev->key()==Qt::Key_Delete || ev->key()==Qt::Key_Left || ev->key()==Qt::Key_Right))
    {

        //if(ev->text().size() == 1 && this->value.size() <= 4 && ((ev->text()[0].toLower() >= QChar('a') && ev->text()[0].toLower() <= QChar('f')) || ev->text()[0].isDigit()))
        //    this->value += ev->text()[0].toLower();

        qDebug() << this->selectionStart();
       // QLabel::keyPressEvent(ev);
        qDebug() << this->value;
    }
    qDebug() << this->value;
}

void InstructionsLabel::setOffset(uint32_t offset, QString value)
{
    this->offset = offset;
    //Update value
    QTextDocument doc;
    doc.setHtml(value);
    this->value = doc.toPlainText();
}
