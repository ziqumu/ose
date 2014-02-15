#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

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
#include "instructionslabel.h"
#include "disassembler.h"
#include <QPlainTextEdit>

//#define INSTRUCTIONWIDGET_ROWNBR 0x40
#define INSTRUCTIONWIDGET_ROWNBR 0x20

struct RowData
{
    QString inlineComment;
    QString blockComment;
};




class InstructionsWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit InstructionsWidget(QWidget *parent = 0);
    ~InstructionsWidget();
    void updateInstructionsList(int NbrRows);
private:
    QTableWidget *table;
    QLineEdit* goEdit;
    uint32_t actualOffset;
    uint32_t actualPC;
    QMap<uint32_t, RowData> rowdata;
public slots:
    void setInstructionsList(QVector<InstructionRow> rows);
    void goButton();
    void gopcButton();
    void upButton();
    void downButton();
    void updateCommentLineHeight(int row = -1);
    bool commentBlockButton(int row = -1, uint32_t offset = -1);
private slots:
signals:
    void updateInstructionsList(unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);
    void updateInstructionsListTo(uint32_t offset, unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);
};

#endif // INSTRUCTIONS_H
