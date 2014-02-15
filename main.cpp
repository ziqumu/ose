#include "mainwindow.h"
#include "displaydriver.h"
#include "core.h"
#include "emulator_exception.hpp"
#include <QApplication>
#include <qDebug>
#include <QString>
#include <String>
#include <QThread>
#include "corethread.h"
#include "signalemitter.h"
#include <QMetaType>
#include <QFile>
#include <QAction>
#include <QDockWidget>
#include "disassembler.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CoreThread *coreThread = new CoreThread;
    //Start window
        MainWindow w(coreThread);
        w.show();
    //init core system
        QThread *thread = new QThread;
        DisplayDriver *DD = new DisplayDriver;
        w.setConnexions(coreThread, DD);
        coreThread->moveToThread(thread);
        DD->moveToThread(thread);
        thread->start();
    //Debug
        qDebug() << "hello from GUI thread : " << a.thread()->currentThreadId();
    //If flash file in parameter
        if (argc > 1)
        {
            QString flashLink;
           //Get OS
                flashLink = argv[1];
           //Start core thread
                w.initCoreSignal(flashLink, DD);
        }
    //Main loop
        a.exec();
    //End
        delete thread;
        delete DD;
        delete coreThread;
        return 0;
}
