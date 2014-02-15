#ifndef SIGNALEMITTER_H
#define SIGNALEMITTER_H

#include <QObject>
#include <QApplication>
#include <qDebug>
#include <String>
#include "displaydriver.h"
#include <QFile>
#include "screen.h"

class signalEmitter : public QObject
{
    Q_OBJECT

private:
    Screen* screen;

public:
    void setScreen(Screen* screen);
    void initCoreSignal(QString flashLink, DisplayDriver* DD);
    void testSignal();

signals:
    void initCore(QString flashLink, DisplayDriver* DD);
    void test();

public slots:
    void ScreenSetByte(uint8_t x, uint8_t y, uint8_t byte);

};

#endif // SIGNALEMITTER_H
