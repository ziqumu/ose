#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QDockWidget>
#include <QTableWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QHeaderView>
#include <QScrollBar>
#include <QDebug>
#include <cstdint>
#include "tablewidgetitemdelegate.h"
#include <QLabel>
#include <QMenu>
#include "disassembler.h"
#include <QPlainTextEdit>

#define REGISTERSWIDGET_ROWHEIGHT 17

struct Registers
{
    uint32_t r[16] = {0};
    uint32_t lastJumpOrigin = 0;
};


class RegisterWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit RegisterWidget(QWidget *parent = 0);
    ~RegisterWidget();
private:
    QTableWidget *table;
    Registers reg;
public slots:
    void setRegistersList(Registers newReg);
    void updateWidget();
private slots:
signals:
    void setRegisters(Registers values);
};

#endif // REGISTERWIDGET_H
