#include "instructions_widget.h"


InstructionsWidget::InstructionsWidget(QWidget *parent) : QDockWidget(tr("Disassembler"), parent)
{
    //init
        rowData.resize(INSTRUCTIONWIDGET_ROWNBR*2+1);
        QWidget* gridWidget = new QWidget(this);
        QGridLayout* grid = new QGridLayout(gridWidget);
        table = new QTableWidget(gridWidget);
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
        go->setMaximumWidth(40);
        gopc->setMaximumWidth(40);
        go->setMaximumHeight(22);
        addcomment->setMaximumHeight(22);
        gopc->setMaximumHeight(22);
        grid->setMargin(0);
        QRegExpValidator* tempV = new QRegExpValidator(QRegExp("[0-9a-fA-F]{1,8}"),this);
        goEdit->setValidator(tempV);
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
        table->setColumnWidth(2,150);
        table->insertColumn(3);
        temp = new QTableWidgetItem(tr("Result"));
        table->setHorizontalHeaderItem(3,temp);
        table->setColumnWidth(3,100);
        table->insertColumn(4);
        temp = new QTableWidgetItem(tr("Comment"));
        table->setHorizontalHeaderItem(4,temp);
        table->setColumnWidth(4,400);
        table->insertColumn(5);
        temp = new QTableWidgetItem(tr("Information"));
        table->setHorizontalHeaderItem(5,temp);
        table->setColumnWidth(5,300);
        //Other
        table->verticalHeader()->hide();
        table->horizontalHeader()->setFixedHeight(17);
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //Construction
        grid->addWidget(table,1,1,1,7);
        grid->addWidget(addcomment,2,4);
        grid->addWidget(gopc,2,5);
        grid->addWidget(goEdit,2,6);
        grid->addWidget(go,2,7);
        gridWidget->setLayout(grid);
        this->setWidget(gridWidget);
    //Add lines
        for(int i = 0; i < (INSTRUCTIONWIDGET_ROWNBR*2+1)*2; i+=2)
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
            QLabel *tempL = new QLabel();
            tempL->setTextFormat(Qt::RichText);
            table->setCellWidget(i,1,tempL);
            //Instruction
            temp = new QTableWidgetItem;
            table->setItem(i,2,temp);
            tempL = new QLabel();
            tempL->setTextFormat(Qt::RichText);
            table->setCellWidget(i,2,tempL);
            //Result
            temp = new QTableWidgetItem;
            table->setItem(i,3,temp);
            //Comment
            temp = new QTableWidgetItem;
            temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            table->setItem(i,4,temp);
            //Info
            temp = new QTableWidgetItem;
            temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            table->setItem(i,5,temp);
            //Comment line
            table->insertRow(i+1);
            table->hideRow(i+1);
        }
    //connects
        connect(go,SIGNAL(clicked()),this,SLOT(goButton()));
        connect(goEdit,SIGNAL(returnPressed()),this,SLOT(goButton()));
        connect(gopc,SIGNAL(clicked()),this,SLOT(gopcButton()));
        connect(addcomment,SIGNAL(clicked()),this,SLOT(commentBlockButton()));
        connect(table,SIGNAL(currentCellChanged(int, int, int, int)),this,SLOT(cellActivated(int, int, int, int)));
        connect(table->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(scrollValueChanged(int)));
        connect(table->verticalScrollBar(),SIGNAL(sliderPressed()),this,SLOT(sliderPressed()));
        connect(table->verticalScrollBar(),SIGNAL(sliderReleased()),this,SLOT(sliderReleased()));
        connect(table,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(itemEdit(QTableWidgetItem*)));
        connect(table,SIGNAL(itemSelectionChanged()),this,SLOT(quitEdit()));
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
            delete table->item(i,3);
            delete table->item(i,4);
            delete table->item(i,5);
        }
    delete table;
}

void InstructionsWidget::itemEdit(QTableWidgetItem* item)
{
    quitEdit();
    if(item->column() == 1 || item->column() == 2) //Only labels
    {
        //Enter edit mode
            editedRow = item->row();
            editedColumn = item->column();
        //add the edit widget to the cell
            QLineEdit* edit = new QLineEdit(table);
            if(item->column() == 2)
                edit->setReadOnly(true);
            else if(item->column() == 1){
                QRegExpValidator* temp = new QRegExpValidator(QRegExp("[0-9a-fA-F]{1,4}"),this);
                edit->setValidator(temp);
            }
        //Prepare text
            QTextDocument doc;
            doc.setHtml(((QLabel*)table->cellWidget(item->row(),item->column()))->text());
            edit->setText(doc.toPlainText());
            connect(edit,SIGNAL(editingFinished()),this,SLOT(quitEdit()));
            table->setCellWidget(item->row(), item->column(), edit);
        //Selection
            edit->selectAll();
    }
}

void InstructionsWidget::quitEdit()
{
    if(editedRow != -1 && editedColumn != -1)
    {
        //save value
            if(editedColumn == 1)
            {
                bool ok;
                uint32_t value = ((QLineEdit*)table->cellWidget(editedRow,editedColumn))->text().toULong(&ok,16);
                if(ok && value <= 0xffff)
                {
                    emit Write_Word(rowData[editedRow/2].offset, value);
                }
            }
        //Restore widgets
            table->removeCellWidget(editedRow, editedColumn);
            if(editedColumn == 1 || editedColumn == 2) //Only labels
            {
                QLabel* label = new QLabel(table);
                table->setCellWidget(editedRow, editedColumn, label);
            }
        //Quit edit mode
            editedRow = -1;
            editedColumn = -1;
        //Restore display
            oldRowData = rowData;
            updateInstructionsListWidget();

    }
}

void InstructionsWidget::keyPressEvent(QKeyEvent * event)
{

    if(table->currentItem() != NULL && (event->key() == Qt::Key_F2))
        itemEdit(table->currentItem());

}

void InstructionsWidget::updateInstructionsListWidget(bool forceCenter)
{

    quitEdit();
    autoScroll = false;
    int fromTop = 0;
    QString scroolSaveOffset = "";
    if(!forceCenter){
        //Save scroll position
        int row = 0;
        for(int i = 0; i < INSTRUCTIONWIDGET_ROWNBR*2-1; i++)
        {
            fromTop += table->rowHeight(row);
            row++;
        }
        scroolSaveOffset = table->item(row-1,0)->text();
    }
    //Save old data
        for(int i = 0; i < table->rowCount(); i++)
        {
            if(table->cellWidget(i,0) != NULL) //block comment
            {
                if(table->item(i-1,0) != NULL && !((QPlainTextEdit*)table->cellWidget(i,0))->toPlainText().isEmpty())
                {
                    offsetData[oldRowData[(i-1)/2].offset].blockComment = ((QPlainTextEdit*)table->cellWidget(i,0))->toPlainText();
                }
                //Delete the line
                table->removeCellWidget(i,0);
                table->hideRow(i);
            }
            else if(table->cellWidget(i,4) != NULL)//inline comment
            {
                if(table->item(i,0) != NULL && !((QPlainTextEdit*)table->cellWidget(i,4))->toPlainText().isEmpty())
                {
                    offsetData[oldRowData[i/2].offset].inlineComment = ((QPlainTextEdit*)table->cellWidget(i,4))->toPlainText();
                }
                //clear comment
                table->removeCellWidget(i,4);
            }

        }
    //Update with new lines
    int i = 0;
    foreach(InstructionRow row, rowData)
    {
        table->item(i,0)->setText(QString("%1").arg(row.offset, 8, 16, QChar('0')));
        ((QLabel*)table->cellWidget(i,1))->setText(row.value);
        ((QLabel*)table->cellWidget(i,2))->setText(row.instruction);
        ((QLabel*)table->cellWidget(i,2))->setToolTip(row.hint);
        table->item(i,3)->setText(row.result);
        if(offsetData.contains(row.offset) && !offsetData[row.offset].information.isEmpty())
            table->item(i,5)->setText(offsetData[row.offset].information);
        else
            table->item(i,5)->setText("");
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
        if(i == INSTRUCTIONWIDGET_ROWNBR*2)
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
        if(offsetData.contains(row.offset))
        {
            if(!offsetData[row.offset].inlineComment.isEmpty())
            {
                QPlainTextEdit *temp = new QPlainTextEdit(table);
                temp->document()->setDocumentMargin(1);
                table->setCellWidget(i,4,temp);
                QFont font;
                font.setPointSize(8);
                temp->setFont(font);
                temp->setStyleSheet("*{border:none;}");
                temp->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                temp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                temp->setTabChangesFocus(true);
                connect(temp, SIGNAL(textChanged()), this, SLOT(updateCommentLineHeight()));
                temp->setPlainText(offsetData[row.offset].inlineComment);
            }
            if(!offsetData[row.offset].blockComment.isEmpty()){
                commentBlockButton(i+1, row.offset,false);
            }
        }
        i+=2;
    }
    updateCommentLineHeight();
    setDisabled(false);
    //Restore scroll position
        int fromTop2 = 0;
        bool found = false;
        int pos = -1;
        if(!forceCenter){
            for(int i = 0; i < table->rowCount(); i++)
            {
                fromTop2 += table->rowHeight(i);
                if(table->item(i,0) != NULL && table->item(i,0)->text() == scroolSaveOffset){
                    found = true;
                    pos = i;
                    break;
                }
            }
        }
        if(found == false || forceCenter) //Can't center on the row centered before widget update -> center on the center row (or force center)
        {
            // [Number of pixel before the center of the main row] - [half of the view height] -> scroll value
            fromTop = 0;
            int i;
            for(i = 0; i < INSTRUCTIONWIDGET_ROWNBR*2; i++)
            {
                fromTop += table->rowHeight(i) ;
            }
            fromTop += table->rowHeight(i+2)/2;
            table->verticalScrollBar()->setValue(fromTop - (table->verticalScrollBar()->height() - table->horizontalHeader()->height())/2);
        }
        else if(fromTop - fromTop2 != 0) // Move the scroll so the line scroolSaveOffset doesn't move when we add line
        {

            table->verticalScrollBar()->setValue(table->verticalScrollBar()->value() - fromTop + fromTop2); //TODO : find a way to be able to use comment rows (currently crash) //Bug when a line is big.. (inline comment more than one line)

            foreach(QTableWidgetItem* item, table->selectedItems())
            {
                item->setSelected(false);
                if(item->row()-(INSTRUCTIONWIDGET_ROWNBR*2-2 - pos) >= 0 && item->row()-(INSTRUCTIONWIDGET_ROWNBR*2-2 - pos) < table->rowCount())
                    table->item(item->row()- (INSTRUCTIONWIDGET_ROWNBR*2-2 - pos), item->column())->setSelected(true);
            }
        }
    autoScroll = true;
}



void InstructionsWidget::setInitInstructionsList(QVector<InstructionRow> rows, bool forceCenter)
{
    if(rows.size() == rowData.size())
    {
        oldRowData = rowData;
        rowData = rows;
        updateInstructionsListWidget(forceCenter);
    }
}

void InstructionsWidget::setInformation(QString info, uint32_t offset)
{
    offsetData[offset].information = info;
}

void InstructionsWidget::sliderPressed()
{
    autoScroll = false;
}
void InstructionsWidget::sliderReleased()
{
    autoScroll = true;
    scrollValueChanged(table->verticalScrollBar()->value());
}

void InstructionsWidget::cellActivated( int row, int column, int previousRow, int PreviousColumn)
{
    //If previous column is comment column, we delete the QPlainTextEdit if empty
    if(PreviousColumn == 4 && table->cellWidget(previousRow,4) != NULL)
    {
        if(((QPlainTextEdit*)table->cellWidget(previousRow,4))->toPlainText().isEmpty())
        {
            table->removeCellWidget(previousRow,4);
        }
    }
    //if the new column is comment column, we init the new QPlainTextEdit
    if(column == 4 && table->cellWidget(row,4) == NULL)
    {
        QPlainTextEdit *temp = new QPlainTextEdit(table);
        temp->document()->setDocumentMargin(1);
        table->setCellWidget(row,4,temp);
        QFont font;
        font.setPointSize(8);
        temp->setFont(font);
        temp->setStyleSheet("*{border:none;}");
        temp->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        temp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        temp->setTabChangesFocus(true);
        connect(temp, SIGNAL(textChanged()), this, SLOT(updateCommentLineHeight()));
        temp->setFocus();
    }
}

void InstructionsWidget::scrollValueChanged(int value)
{
    if(autoScroll)
    {
        //Get the top and bottom line number
        int fromTop = 0;
        int TopLine = -1;
        int BottomLine = 0;
        for(int line = 0; line < table->rowCount(); line++)
        {
            fromTop += table->rowHeight(line);
            if(fromTop >= value && TopLine == -1)
                TopLine = line;
            if(fromTop >= (value + table->verticalScrollBar()->height() - table->horizontalHeader()->height())){
                BottomLine = line;
                break;
            }
        }
        //Middle line
        int middleLine = (TopLine+BottomLine)/2;
        //Update if different from the row n° INSTRUCTIONWIDGET_ROWNBR+1
        if(middleLine != INSTRUCTIONWIDGET_ROWNBR*2)
        {
            //Test if it's a commentline..
                if(table->item(middleLine,0) == NULL)
                    middleLine--;

            autoScroll = false;
            emit initInstructionsListTo(rowData[middleLine/2].offset);
        }
    }
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
        }
        else if(table->cellWidget(i,4) == NULL) //Update comment cell without comment cell (deleted) => restore oriinal size
        {
            table->setRowHeight(i, INSTRUCTIONWIDGET_ROWHEIGHT);
        }

    }
}

void InstructionsWidget::goButton()
{
    bool ok;
    uint32_t offset = goEdit->text().toULong(&ok,16);
    if(ok)
    {
        //actualOffset = offset;
        autoScroll = false;
        emit initInstructionsListTo(offset);
        table->clearSelection();
        table->selectRow(INSTRUCTIONWIDGET_ROWNBR*2);
        table->setFocus();
    }
    else
        goEdit->setText(QString("%1").arg(actualOffset, 4, 16, QChar('0')));
}

bool InstructionsWidget::commentBlockButton(int row, uint32_t offset, bool updateSize)
{
    //Get the row
    if(row == -1){
        if(!table->selectedRanges().isEmpty())
            row = table->selectedRanges().first().topRow() + 1;
        else
            return false;
    }
    //Get the row offset
    if(offset == 0xffffffff){
        offset = rowData[(row-1)/2].offset;
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
        table->showRow(row);
        table->setSpan(row,0,1,6);
        table->setCellWidget(row,0,edit);
        connect(edit, SIGNAL(textChanged()), this, SLOT(updateCommentLineHeight()));
    //Fill it with text if we have
        if(offsetData.contains(offset))
            edit->setPlainText(offsetData[offset].blockComment);
    //update size
        if(updateSize){
            this->updateCommentLineHeight(row);
            edit->setFocus();
        }
        return true;
}

void InstructionsWidget::gopcButton()
{
    //actualOffset = actualPC;
    emit initInstructionsList(true);
    goEdit->setText(QString("%1").arg(actualPC, 4, 16, QChar('0')));
    table->clearSelection();
}
