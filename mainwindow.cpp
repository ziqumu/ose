#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QTableWidget>
#include <QScrollBar>
#include <QPushButton>
#include "corethread.h"

#define InstructionsList_ROWNBR 50

MainWindow::MainWindow(CoreThread *coreThread, QWidget *parent) : QMainWindow(parent),  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // to hide the title bar completely must replace the default widget with a generic one
    //QWidget* titleWidget = new QWidget(this); /* where this a QMainWindow object */
    //ui->buttons->setTitleBarWidget( titleWidget );
    this->coreThread = coreThread;



    ui->emulatorLogText->setReadOnly(true);
    ui->emulatorLogText->setUndoRedoEnabled(false);

    //Init Instructions dock widget
        instructions = new InstructionsWidget(this);
        addDockWidget(Qt::BottomDockWidgetArea, instructions);
        this->tabifyDockWidget(ui->EmulatorLog,instructions);
        //Connects
        qRegisterMetaType<QVector<InstructionRow>>("QVector<InstructionRow>");
        connect(this->instructions, SIGNAL(initInstructionsList(bool, unsigned int)), coreThread, SLOT(initInstructionsList(bool, unsigned int)));
        connect(this->instructions, SIGNAL(initInstructionsListTo(uint32_t, bool, unsigned int)), coreThread, SLOT(initInstructionsListTo(uint32_t, bool,unsigned int)));
        connect(coreThread, SIGNAL(setInitInstructionsList(QVector<InstructionRow>, bool)), this->instructions, SLOT(setInitInstructionsList(QVector<InstructionRow>, bool)));
    //Init Registers dock widget
        qRegisterMetaType<Registers>("Registers");
        registers = new RegisterWidget(this);
        addDockWidget(Qt::RightDockWidgetArea, registers);
        connect(coreThread, SIGNAL(setRegistersList(Registers)), this->registers, SLOT(setRegistersList(Registers)));
    //Other connects
        connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(play()));
        connect(ui->actionPause, SIGNAL(triggered()), this, SLOT(pause()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete instructions;
}



Screen* MainWindow::getScreen()
{
   return ui->screen;
}



void MainWindow::initCoreSignal(QString flashLink, DisplayDriver* DD)
{
    emit initCore(flashLink, DD);
}


void MainWindow::screenSetByte(uint8_t x, uint8_t y, uint8_t byte)
{
    ui->screen->setByte(x, y, byte);
}



void MainWindow::setConnexions(CoreThread *coreThread, DisplayDriver *DD)
{
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<QVector<QVector<uint32_t>>>("QVector<QVector<uint32_t>>");
    //connect(DD, SIGNAL(test()), this, SLOT(test()));
    //connect(coreThread, SIGNAL(test()), this, SLOT(test()));
    connect(DD, SIGNAL(setScreenByte(uint8_t, uint8_t, uint8_t)), this, SLOT(screenSetByte(uint8_t, uint8_t, uint8_t)));
    connect(this, SIGNAL(initCore(QString, DisplayDriver*)), coreThread, SLOT(init(QString, DisplayDriver*)));
    //connect(ui->actionPlay, SIGNAL(clicked()), coreThread, SLOT(play()));
    //connect(getButton("next_ee"), SIGNAL(clicked()), coreThread, SLOT(nextUntilExecError()));

    connect(coreThread, SIGNAL(log(QString,uint32_t)), this, SLOT(log(QString,uint32_t)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logError(QString,uint32_t)), this, SLOT(logError(QString,uint32_t)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logWarning(QString,uint32_t)), this, SLOT(logWarning(QString,uint32_t)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logInfo(QString,uint32_t)), this, SLOT(logInfo(QString,uint32_t)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logDebug(QString,uint32_t)), this, SLOT(logDebug(QString,uint32_t)),Qt::QueuedConnection);
    connect(instructions, SIGNAL(Write_Word(uint32_t,uint16_t)), coreThread, SLOT(Write_Word(uint32_t,uint16_t)),Qt::QueuedConnection);

    connect(coreThread, SIGNAL(stop()), this, SLOT(pause()));

    connect(ui->actionStep_into, SIGNAL(triggered()), coreThread, SLOT(step_into()));
    connect(ui->actionPlay, SIGNAL(triggered()), coreThread, SLOT(play()));
    connect(DD, SIGNAL(refresh()), ui->screen, SLOT(refreshOrder()));

}

void MainWindow::play()
{
    ui->actionPlay->setVisible(false);
    ui->actionPause->setVisible(true);

    //Disable log window because it will not be updated during emulation
    ui->emulatorLogText->setDisabled(true);
    instructions->setDisabled(true);
}

void MainWindow::pause()
{
    ui->actionPlay->setVisible(true);
    ui->actionPause->setVisible(false);
    coreThread->requestStop();

    //Reduce log size if too big
    if(Log.size() > 500000)
        Log = Log.right(500000);
    //Renable log and put all text inside
    ui->emulatorLogText->setUpdatesEnabled(false);
    ui->emulatorLogText->setPlainText(Log);
    ui->emulatorLogText->setUpdatesEnabled(true);
    ui->emulatorLogText->setDisabled(false);
    ui->emulatorLogText->verticalScrollBar()->setValue(ui->emulatorLogText->verticalScrollBar()->maximum());
}



void MainWindow::log(QString message, uint32_t offset)
{
    Log.append(message + " | " + QString::number(offset,16) + "\n");
    if(offset != 0xffffffff)
        instructions->setInformation(message, offset);
}

void MainWindow::logError(QString message, uint32_t offset)
{
    Log.append("(E) " + message + " | " + QString::number(offset,16) + "\n");
    if(offset != 0xffffffff)
        instructions->setInformation("(E) " + message, offset);
}

void MainWindow::logWarning(QString message, uint32_t offset)
{
    Log.append("(W) " + message + " | " + QString::number(offset,16) + "\n");
    if(offset != 0xffffffff)
        instructions->setInformation("(W) " + message, offset);
}

void MainWindow::logInfo(QString message, uint32_t offset)
{
    Log.append("(I) " + message + " | " + QString::number(offset,16) + "\n");
    if(offset != 0xffffffff)
        instructions->setInformation("(I) " + message, offset);
}

void MainWindow::logDebug(QString message, uint32_t offset)
{
    Log.append("(D) " + message + " | " + QString::number(offset,16) + "\n");
    if(offset != 0xffffffff)
        instructions->setInformation("(D) " + message, offset);
}
