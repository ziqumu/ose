#ifndef CORETHREAD_H
#define CORETHREAD_H

#include <QThread>
#include <QDebug>
#include <QObject>
#include <QMutex>
#include <QVector>
#include <cstdint>
#include "displaydriver.h"
#include "emulator_exception.hpp"
#include "core.h"
#include <exception>
#include "instructions_widget.h"
#include "disassembler.h"
#include "register_widget.h"

class CoreThread : public QObject
{
    Q_OBJECT
public:
     void requestStop(bool state = true);
private:
    Core *core;
    bool stopRequested = false;
    QMutex mutex;
    bool isStopRequested();
public slots:
    void init(QString flashLink, DisplayDriver* DD);
    //Prepare data for each lines
    void initInstructionsList(bool forceCenter = false, unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);
    void initInstructionsListTo(uint32_t offset, bool forceCenter = false, unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);
    void initRegistersList();
    int step_into(bool loopmode = false);
    void Write_Word(uint32_t offset, uint16_t value);
    void play();

signals:
    void stop();
    void setRegistersList(Registers newReg);
    void setInitInstructionsList(QVector<InstructionRow> rows, bool forceCenter = false);
    void log(QString message, uint32_t offset = 0xffffffff);
    void logError(QString message, uint32_t offset = 0xffffffff);
    void logWarning(QString message, uint32_t offset = 0xffffffff);
    void logInfo(QString message, uint32_t offset = 0xffffffff);
    void logDebug(QString message, uint32_t offset = 0xffffffff);
};

#endif // CORETHREAD_H
