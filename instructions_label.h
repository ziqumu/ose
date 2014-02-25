#ifndef INSTRUCTIONSLABEL_H
#define INSTRUCTIONSLABEL_H

#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QPaintEvent>
#include <QTextDocument>
#include <cstdint>
#include <QTextEdit>

class InstructionsLabel : public QLabel
{
    Q_OBJECT
private:
    uint32_t offset = 0xffffffff;
    QString value = "";
public:
    explicit InstructionsLabel(QWidget *parent = 0);
    void mouseDoubleClickEvent(QMouseEvent * ev);
    void focusOutEvent(QFocusEvent *ev);
    void setOffset(uint32_t offset, QString value);
    void keyPressEvent(QKeyEvent * ev);
signals:
    void valueChanged(QString value);
public slots:

};

#endif // INSTRUCTIONSLABEL_H
