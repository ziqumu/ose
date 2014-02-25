#include "register_widget.h"


RegisterWidget::RegisterWidget(QWidget *parent) : QDockWidget(tr("Registers"), parent)
{
    //init
        table = new QTableWidget(this);
    //set params
        this->setDisabled(true);
        this->setMaximumWidth(140);
    //table Settings
        //columns
        table->insertColumn(0);
        QTableWidgetItem* temp = new QTableWidgetItem(tr("Name"));
        table->setHorizontalHeaderItem(0,temp);
        table->setColumnWidth(0,35);
        table->insertColumn(1);
        temp = new QTableWidgetItem(tr("Value"));
        table->setHorizontalHeaderItem(1,temp);
        table->setColumnWidth(1,70);
        //Other
        table->verticalHeader()->hide();
        table->horizontalHeader()->setFixedHeight(17);
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->horizontalScrollBar()->hide();
    //Construction
        this->setWidget(table);
    //Add lines
        //globals registers
        int i;
        for(i = 0; i < 16; i+=1)
        {
            table->insertRow(i);
            //Name
            table->setRowHeight(i,REGISTERSWIDGET_ROWHEIGHT);
            QTableWidgetItem* temp = new QTableWidgetItem;
            temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            temp->setText("r" + QString::number(i));
            table->setItem(i,0,temp);
            //Value
            table->setRowHeight(i,REGISTERSWIDGET_ROWHEIGHT);
            temp = new QTableWidgetItem;
            temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            table->setItem(i,1,temp);
        }
        table->insertRow(i);
        //Name
        table->setRowHeight(i,REGISTERSWIDGET_ROWHEIGHT);
        temp = new QTableWidgetItem;
        temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        temp->setText("Last jump origin");
        table->setItem(i,0,temp);
        //Value
        table->setRowHeight(i,REGISTERSWIDGET_ROWHEIGHT);
        temp = new QTableWidgetItem;
        temp->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        table->setItem(i,1,temp);
}


RegisterWidget::~RegisterWidget()
{
}


void RegisterWidget::setRegistersList(Registers newReg)
{
        reg = newReg;
        updateWidget();
}

void RegisterWidget::updateWidget()
{
    //globals registers
        int i;
        for(i = 0; i < 16; i+=1)
        {
            table->item(i,1)->setText(QString::number(reg.r[i],16));
        }
        table->item(i,1)->setText(QString::number(reg.lastJumpOrigin,16));
        this->setDisabled(false);
}
