#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <vector>
#include <QTableWidgetItem>
#include <QStatusBar>
#include <qDebug>
#include <QFile>
#include "screen.h"
#include <String>
#include "displaydriver.h"
#include <QPushButton>
#include "corethread.h"
#include <cstdint>
#include "instructions_widget.h"
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

	class QStatusBar;
public:
    explicit MainWindow(CoreThread *coreThread,QWidget *parent = 0);
    Screen* getScreen();
    ~MainWindow();
    void initCoreSignal(QString flashLink, DisplayDriver* DD);
    void setConnexions(CoreThread* coreThread, DisplayDriver *DD);

private:
    Ui::MainWindow *ui;
    CoreThread *coreThread;
    QString Log;
    InstructionsWidget *instructions;
public slots:
    void screenSetByte(uint8_t x, uint8_t y, uint8_t byte);
    void play();
    void pause();
    void log(QString message);
    void logError(QString message);
    void logWarning(QString message);
    void logInfo(QString message);
    void logDebug(QString message);
signals:
    void initCore(QString flashLink, DisplayDriver* D);

};

#endif // MAINWINDOW_H
