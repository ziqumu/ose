#ifndef INSTRUCTIONSLABEL_H
#define INSTRUCTIONSLABEL_H

#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QPaintEvent>
#include <QTextDocument>

class InstructionsLabel : public QLabel
{
    Q_OBJECT
public:
    explicit InstructionsLabel(QWidget *parent = 0);
    void mouseDoubleClickEvent(QMouseEvent * ev);
    void focusOutEvent(QFocusEvent *ev);
signals:

public slots:

};

#endif // INSTRUCTIONSLABEL_H
