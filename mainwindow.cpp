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
        connect(this->instructions, SIGNAL(updateInstructionsList(unsigned int)), coreThread, SLOT(updateInstructionsList(unsigned int)));
        connect(this->instructions, SIGNAL(updateInstructionsListTo(uint32_t, unsigned int)), coreThread, SLOT(updateInstructionsListTo(uint32_t,unsigned int)));
        connect(coreThread, SIGNAL(setInstructionsList(QVector<InstructionRow>)), this->instructions, SLOT(setInstructionsList(QVector<InstructionRow>)));
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
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<QVector<QVector<uint32_t>>>("QVector<QVector<uint32_t>>");
    //connect(DD, SIGNAL(test()), this, SLOT(test()));
    //connect(coreThread, SIGNAL(test()), this, SLOT(test()));
    connect(DD, SIGNAL(setScreenByte(uint8_t, uint8_t, uint8_t)), this, SLOT(screenSetByte(uint8_t, uint8_t, uint8_t)));
    connect(this, SIGNAL(initCore(QString, DisplayDriver*)), coreThread, SLOT(init(QString, DisplayDriver*)));
    //connect(ui->actionPlay, SIGNAL(clicked()), coreThread, SLOT(play()));
    //connect(getButton("next_ee"), SIGNAL(clicked()), coreThread, SLOT(nextUntilExecError()));

    connect(coreThread, SIGNAL(log(QString)), this, SLOT(log(QString)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logError(QString)), this, SLOT(logError(QString)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logWarning(QString)), this, SLOT(logWarning(QString)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logInfo(QString)), this, SLOT(logInfo(QString)),Qt::QueuedConnection);
    connect(coreThread, SIGNAL(logDebug(QString)), this, SLOT(logDebug(QString)),Qt::QueuedConnection);


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
    //information ask signal
    this->instructions->updateInstructionsList();
}



void MainWindow::log(QString message)
{
    Log.append(message + "\n");
}

void MainWindow::logError(QString message)
{
    Log.append("(E) " + message + "\n");
}

void MainWindow::logWarning(QString message)
{
    Log.append("(W) " + message + "\n");
}

void MainWindow::logInfo(QString message)
{
    Log.append("(I) " + message + "\n");
}

void MainWindow::logDebug(QString message)
{
    Log.append("(D) " + message + "\n");
}
