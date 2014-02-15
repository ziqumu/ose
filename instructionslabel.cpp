#include "instructionslabel.h"

InstructionsLabel::InstructionsLabel(QWidget *parent) : QLabel(parent)
{
}


void InstructionsLabel::mouseDoubleClickEvent(QMouseEvent * ev)
{
    this->setStyleSheet("QLabel{background-color:white;}");
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
    QLabel::focusOutEvent(ev);
}
