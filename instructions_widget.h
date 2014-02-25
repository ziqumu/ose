#ifndef INSTRUCTIONSWIDGET_H
#define INSTRUCTIONSWIDGET_H

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

#define INSTRUCTIONWIDGET_ROWNBR 0x40
#define INSTRUCTIONWIDGET_ROWHEIGHT 17

struct RowData
{
    QString inlineComment;
    QString blockComment;
    QString information;
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
    QMap<uint32_t, RowData> offsetData;
    QVector<InstructionRow> rowData;
    QVector<InstructionRow> oldRowData;
    bool autoScroll = true;
    void updateInstructionsListWidget(bool forceCenter = false);
    int actualScroolValue = 0;
    int editedRow = -1;
    int editedColumn = -1;
    void keyPressEvent(QKeyEvent * event);

    QWidget* editedWidgetParent;
    Qt::WindowFlags editedWidgetFlags;
public slots:
    void setInitInstructionsList(QVector<InstructionRow> rows, bool forceCenter = false);
    void setInformation(QString info, uint32_t offset);
private slots:
    void sliderPressed();
    void sliderReleased();
    void itemEdit(QTableWidgetItem* item);
    void quitEdit();
    void goButton();
    void gopcButton();
    void updateCommentLineHeight(int row = -1);
    bool commentBlockButton(int row = -1, uint32_t offset = 0xffffffff, bool updateSize = true);
    void cellActivated (int row, int column , int previousRow, int PreviousColumn);
    void scrollValueChanged(int value);
signals:
    void initInstructionsList(bool forceCenter = false, unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);
    void initInstructionsListTo(uint32_t offset, bool forceCenter = false, unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);
    void Write_Word(uint32_t offset, uint16_t value);
};

#endif // INSTRUCTIONSWIDGET_H
