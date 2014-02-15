#include "instructions_widget.h"

#define INSTRUCTIONWIDGET_ROWHEIGHT 17

InstructionsWidget::InstructionsWidget(QWidget *parent) : QDockWidget(tr("Instructions"), parent)
{
    //init
        QWidget* gridWidget = new QWidget(this);
        QGridLayout* grid = new QGridLayout(gridWidget);
        table = new QTableWidget(gridWidget);
        QPushButton* up = new QPushButton(tr("Up"),gridWidget);
        QPushButton* down = new QPushButton(tr("Down"),gridWidget);
        QPushButton* gopc = new QPushButton(tr("Go PC"),gridWidget);
        goEdit = new QLineEdit(gridWidget);
        QPushButton* go = new QPushButton(tr("Go"),gridWidget);
        QPushButton* addcomment = new QPushButton(tr("Add comment"),gridWidget);
    //set params
        this->setDisabled(true);
        grid->setSpacing(0);
        goEdit->setMaximumWidth(70);
        goEdit->setMinimumWidth(40);
        addcomment->setMaximumWidth(100);
        up->setMaximumWidth(40);
        down->setMaximumWidth(40);
        go->setMaximumWidth(40);
        gopc->setMaximumWidth(40);
        up->setMaximumHeight(22);
        down->setMaximumHeight(22);
        go->setMaximumHeight(22);
        addcomment->setMaximumHeight(22);
        gopc->setMaximumHeight(22);
        grid->setMargin(0);
    //table Settings
        //columns
        table->insertColumn(0);
        QTableWidgetItem* temp = new QTableWidgetItem(tr("Offset"));
        table->setHorizontalHeaderItem(0,temp);
        table->setColumnWidth(0,60);
        table->insertColumn(1);
        temp = new QTableWidgetItem(tr("Value"));
        table->setHorizontalHeaderItem(1,temp);
        table->setColumnWidth(1,35);
        table->insertColumn(2);
        temp = new QTableWidgetItem(tr("Instruction"));
        table->setHorizontalHeaderItem(2,temp);
        table->setColumnWidth(2,250);
        table->insertColumn(3);
        temp = new QTableWidgetItem(tr("Result"));
        table->setHorizontalHeaderItem(3,temp);
        table->setColumnWidth(3,250);
        table->insertColumn(4);
        temp = new QTableWidgetItem(tr("Comment"));
        table->setHorizontalHeaderItem(4,temp);
        table->setColumnWidth(4,150);
        table->insertColumn(5);
        temp = new QTableWidgetItem(tr("Error"));
        table->setHorizontalHeaderItem(5,temp);
        table->setColumnWidth(4,150);
        //Other
        table->verticalHeader()->hide();
        table->horizontalHeader()->setFixedHeight(17);
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    //Construction
        grid->addWidget(table,1,1,1,7);
        grid->addWidget(up,2,1);
        grid->addWidget(down,2,2);
        grid->addWidget(addcomment,2,4);
        grid->addWidget(gopc,2,5);
        grid->addWidget(goEdit,2,6);
        grid->addWidget(go,2,7);
        gridWidget->setLayout(grid);
        this->setWidget(gridWidget);
    //Add lines
        for(int i = 0; i < INSTRUCTIONWIDGET_ROWNBR*2+1; i++)
        {
            table->insertRow(i);
            //Offset
            table->setRowHeight(i,INSTRUCTIONWIDGET_ROWHEIGHT);
            QTableWidgetItem* temp = new QTableWidgetItem;
            temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            table->setItem(i,0,temp);
            //Value - Widget and item at the same time to keep blue color when selected and there is already background color on label
            temp = new QTableWidgetItem;
            table->setItem(i,1,temp);
            InstructionsLabel *tempL = new InstructionsLabel();
            tempL->setTextFormat(Qt::RichText);
            table->setCellWidget(i,1,tempL);
            //Instruction
            temp = new QTableWidgetItem;
            table->setItem(i,2,temp);
            tempL = new InstructionsLabel();
            tempL->setTextFormat(Qt::RichText);
            table->setCellWidget(i,2,tempL);
            //Result
            temp = new QTableWidgetItem;
            table->setItem(i,3,temp);
            tempL = new InstructionsLabel();
            tempL->setTextFormat(Qt::RichText);
            table->setCellWidget(i,3,tempL);
            //Comment

            temp = new QTableWidgetItem;
            temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            table->setItem(i,4,temp);


            QPlainTextEdit *temp2 = new QPlainTextEdit(table);
            temp2->document()->setDocumentMargin(1);
            table->setCellWidget(i,4,temp2);
            QFont font;
            font.setPointSize(8);
            temp2->setFont(font);
            temp2->setStyleSheet("*{border:none;}");
            temp2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            temp2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            temp2->setTabChangesFocus(true);
            connect(temp2, SIGNAL(textChanged()), this, SLOT(updateCommentLineHeight()));
            //Error
            temp = new QTableWidgetItem;
            temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            table->setItem(i,5,temp);
        }
    //connects
        connect(go,SIGNAL(clicked()),this,SLOT(goButton()));
        connect(gopc,SIGNAL(clicked()),this,SLOT(gopcButton()));
        connect(up,SIGNAL(clicked()),this,SLOT(upButton()));
        connect(down,SIGNAL(clicked()),this,SLOT(downButton()));
        connect(addcomment,SIGNAL(clicked()),this,SLOT(commentBlockButton()));
}




InstructionsWidget::~InstructionsWidget()
{
    //Collumns
        delete table->horizontalHeaderItem(0);
        delete table->horizontalHeaderItem(1);
        delete table->horizontalHeaderItem(2);
        delete table->horizontalHeaderItem(3);
        delete table->horizontalHeaderItem(4);
    //rows
        for(int i = 0; i < INSTRUCTIONWIDGET_ROWNBR*2+1; i++)
        {
            delete table->item(i,0);
            delete table->cellWidget(i,1);
            delete table->cellWidget(i,2);
            delete table->cellWidget(i,3);
            delete table->item(i,4);
            delete table->item(i,5);
        }
    delete table;
}



void InstructionsWidget::updateCommentLineHeight(int row)
{
    int min,max;
    if(row < 0)
    {
        min = 0;
        max = table->rowCount()-1;
    }
    else
    {
        min = row;
        max = row;
    }
    for(int i = min; i <= max; i++)
    {
        if(table->cellWidget(i,0) != NULL) //Update commentline
        {
            table->setRowHeight(i, (((QPlainTextEdit*)table->cellWidget(i,0))->document()->documentLayout()->documentSize().height() - 1)
                                * ((QPlainTextEdit*)table->cellWidget(i,0))->fontMetrics().height() + INSTRUCTIONWIDGET_ROWHEIGHT + 2); // +2 because of the border
        }
        else if(table->cellWidget(i,4) != NULL)//Update comment  cell
        {
            table->setRowHeight(i, (((QPlainTextEdit*)table->cellWidget(i,4))->document()->documentLayout()->documentSize().height() - 1)
                                * ((QPlainTextEdit*)table->cellWidget(i,4))->fontMetrics().height() + INSTRUCTIONWIDGET_ROWHEIGHT);
            //table->setRowHeight(0,table->sizeHint().height() - table->horizontalHeader()->height()); //Max */
        }

    }
}


void InstructionsWidget::setInstructionsList(QVector<InstructionRow> rows)
{
    //Save old data
        for(int i = 0; i < table->rowCount(); i++)
        {
            if(table->cellWidget(i,0) != NULL) //block comment
            {
                if(table->item(i-1,0) != NULL && !((QPlainTextEdit*)table->cellWidget(i,0))->toPlainText().isEmpty())
                {
                    bool ok = false;
                    uint32_t offset = table->item(i-1,0)->text().toULong(&ok, 16);
                    if(ok)
                    {
                        rowdata[offset].blockComment = ((QPlainTextEdit*)table->cellWidget(i,0))->toPlainText();
                    }
                }
                //Delete the line
                delete table->cellWidget(i,0);
                table->removeRow(i);
            }
            else if(table->cellWidget(i,4) != NULL)//inline comment
            {
                if(table->item(i,0) != NULL && !((QPlainTextEdit*)table->cellWidget(i,4))->toPlainText().isEmpty())
                {
                    bool ok = false;
                    uint32_t offset = table->item(i,0)->text().toULong(&ok, 16);
                    if(ok)
                    {
                        rowdata[offset].inlineComment = ((QPlainTextEdit*)table->cellWidget(i,4))->toPlainText();
                    }
                }
                //clear comment
                ((QPlainTextEdit*)table->cellWidget(i,4))->clear();
            }

        }
    //Update with new lines
    int i = 0;
    foreach(InstructionRow row, rows)
    {
        table->item(i,0)->setText(QString("%1").arg(row.offset, 8, 16, QChar('0')));
        ((QLabel*)table->cellWidget(i,1))->setText(row.value);
        ((QLabel*)table->cellWidget(i,2))->setText(row.instruction);
        ((QLabel*)table->cellWidget(i,2))->setToolTip(row.hint);
        ((QLabel*)table->cellWidget(i,3))->setText(row.result);
        //Set the line in orange if this is PC-4
        if(row.actualOffset)
        {
            table->item(i,0)->setBackgroundColor(QColor(255,193,142));
            table->item(i,1)->setBackgroundColor(QColor(255,193,142));
            table->item(i,2)->setBackgroundColor(QColor(255,193,142));
            table->item(i,3)->setBackgroundColor(QColor(255,193,142));
            table->item(i,4)->setBackgroundColor(QColor(255,193,142));
            table->item(i,5)->setBackgroundColor(QColor(255,193,142));
            actualPC = row.offset;

        }
        else
        {
            //Remove all colors
            table->item(i,0)->setBackgroundColor(QColor(255,255,255));
            table->item(i,1)->setBackgroundColor(QColor(255,255,255));
            table->item(i,2)->setBackgroundColor(QColor(255,255,255));
            table->item(i,3)->setBackgroundColor(QColor(255,255,255));
            table->item(i,4)->setBackgroundColor(QColor(255,255,255));
            table->item(i,5)->setBackgroundColor(QColor(255,255,255));
        }
        if(i == INSTRUCTIONWIDGET_ROWNBR)
        {
            actualOffset = row.offset;
            goEdit->setText(QString("%1").arg(row.offset, 8, 16, QChar('0')));
        }
        //Set the line in red if there is a breakpoint here
        if(row.breakpoint)
        {
            table->item(i,0)->setBackgroundColor(QColor(250,69,69));
        }
        //Add comments
        if(rowdata.contains(row.offset))
        {
            if(!rowdata[row.offset].inlineComment.isEmpty())
                ((QPlainTextEdit*)table->cellWidget(i,4))->setPlainText(rowdata[row.offset].inlineComment);
            if(!rowdata[row.offset].blockComment.isEmpty()){
                if(commentBlockButton(i+1, row.offset))
                    i++;
            }
        }
        i++;
    }
    updateCommentLineHeight();
    setDisabled(false);
    table->verticalScrollBar()->setValue(table->verticalScrollBar()->maximum()/2);
}

void InstructionsWidget::goButton()
{
    bool ok;
    uint32_t offset = goEdit->text().toULong(&ok,16);
    if(ok)
    {
        actualOffset = offset;
        emit updateInstructionsListTo(offset);
        table->clearSelection();
        table->selectRow(INSTRUCTIONWIDGET_ROWNBR);
        table->setFocus();
    }
    else
        goEdit->setText(QString("%1").arg(actualOffset, 4, 16, QChar('0')));
}

bool InstructionsWidget::commentBlockButton(int row, uint32_t offset)
{
    //Get the row
    if(row == -1){
        if(!table->selectedRanges().isEmpty())
            row = table->selectedRanges().first().topRow() + 1;
        else
            return false;
    }
    //Get the row offset
    if(offset == -1){
        bool ok = false;
        offset = table->item(row,0)->text().toULong(&ok, 16);
        if(!ok)
            return false;
    }
    //Test if the row selected is a commentblock OR if there is already one
    if(table->cellWidget(row-1,0) != NULL || table->cellWidget(row,0) != NULL)
        return false;
    //Add the new comment block
        QPlainTextEdit* edit = new QPlainTextEdit;
        edit->setTabChangesFocus(true);
        edit->document()->setDocumentMargin(1);
        edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QFont font;
        font.setPointSize(8);
        edit->setFont(font);
        table->insertRow(row);
        table->setSpan(row,0,1,6);
        table->setCellWidget(row,0,edit);
        connect(edit, SIGNAL(textChanged()), this, SLOT(updateCommentLineHeight()));
    //Fill it with text if we have
        if(rowdata.contains(offset))
            edit->setPlainText(rowdata[offset].blockComment);
       return true;
}

void InstructionsWidget::gopcButton()
{
        actualOffset = actualPC;
        emit updateInstructionsListTo(actualPC);
        goEdit->setText(QString("%1").arg(actualPC, 4, 16, QChar('0')));
        table->clearSelection();
}

void InstructionsWidget::upButton()
{
        actualOffset-=INSTRUCTIONWIDGET_ROWNBR*2;
        emit updateInstructionsListTo(actualOffset);
        goEdit->setText(QString("%1").arg(actualOffset, 4, 16, QChar('0')));
        table->clearSelection();
}

void InstructionsWidget::downButton()
{
    actualOffset+=INSTRUCTIONWIDGET_ROWNBR*2;
    emit updateInstructionsListTo(actualOffset);
    goEdit->setText(QString("%1").arg(actualOffset, 4, 16, QChar('0')));
    table->clearSelection();
}

