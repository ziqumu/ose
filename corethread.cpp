#include "corethread.h"

void CoreThread::init(QString flashLink, DisplayDriver* DD)
{
     qDebug() << "hello from worker thread " << thread()->currentThreadId();

    //Lancement de l'emulateur
        this->core = new Core(flashLink, DD);
    //Config "bidouille" afin de lancer l'emulateur
        this->core->setPC(0xA0000004);


     DD->setRegister(7);
     DD->putData(0xFF);
}

int CoreThread::step_into(bool loopmode)
{
    //Another little hack..
        this->core->Write_Byte(0x8800713c,1);
    try
    {
        core->Next();
    }
    catch (emulator_exception & e )
    {
        // Some "hacks" to go out of some loop due to hardware wait.. Todo, really emulate this hardware
            if(e.getPC() == 0xa000003c)
            {
                core->setPC(core->getPC()+4);
                core->setT(1);
            }
            else if(e.getPC() == 0x80002a08)
            {
                core->setPC(core->getPC()+6);
                core->setT(0);
            }
            else if(e.getPC() == 0xa00100bc)
            {
                core->setPC(core->getPC()+4);
                core->setT(1);
            }
            else if(e.getPC() == 0x8003e4d4)
            {
                core->setPC(core->getPC()+8);
                core->setT(1);
            }
            else
                core->setPC(core->getPC()+2);
        //emulator log
            if(e.getAction() == EMULATOR_ACTION_EXECUTE)
                emit logError(QString(e.what()));
            else
                emit logWarning(QString(e.what()));

        return e.getAction();
    }
    if(!loopmode)
        updateInstructionsList();
    return 0;
}

void CoreThread::play()
{
    while(true)
    {
        if(step_into(true) == EMULATOR_ACTION_EXECUTE)
            break;
        if(isStopRequested())
        {
            requestStop(false);
            break;
        }
    }
}

void CoreThread::updateInstructionsList(unsigned int nbrLine)
{
    uint32_t offset = core->getPC()-4;
    updateInstructionsListTo(offset, nbrLine);
}

void CoreThread::updateInstructionsListTo(uint32_t offset, unsigned int nbrLine)
{
    offset = offset-(nbrLine*2);
    QVector<InstructionRow> rows;
    rows.resize((nbrLine*2)+1);
    for(unsigned int i = 0 ; i < (nbrLine*2)+1; i++)
    {
        //Disassemble
        Disassembler disassembler(core);
        rows[i] = disassembler.read(offset+i*2);

    }
    emit setInstructionsList(rows);
}


//Stop loop system
    void CoreThread::requestStop(bool state)
    {
        QMutexLocker locker(&mutex);
        stopRequested = state;
    }

    bool CoreThread::isStopRequested()
    {
        QMutexLocker locker(&mutex);
        return stopRequested;
    }

