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
    void updateInstructionsListTo(uint32_t offset, unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);
    void updateInstructionsList(unsigned int nbrLine = INSTRUCTIONWIDGET_ROWNBR);

    int step_into(bool loopmode = false);
    void play();

signals:
    void test();
    void setInstructionsList(QVector<InstructionRow> rows);
    void log(QString message);
    void logError(QString message);
    void logWarning(QString message);
    void logInfo(QString message);
    void logDebug(QString message);
};

#endif // CORETHREAD_H
