#ifndef DISPLAYDRIVER_H
#define DISPLAYDRIVER_H

#include <QObject>
#include <cstdint>
#include <QWidget>
#include <QTimer>
#include <QDebug>
#include "screen.h"

class DisplayDriver : public QObject
{
    Q_OBJECT
private:
    uint8_t reg = 0;
    uint8_t x = 0; //0->19 : used 0->15 (note : in the Toshiba T6K11 doc, X and Y are inverted)
    uint8_t y = 0; //0->63
    uint8_t counterMode = 0;
public:
    void setRegister(int8_t reg);
    void putData(uint8_t value);
    void sendTest();

signals:
    void test();
    void refresh();
    void setScreenByte(uint8_t x, uint8_t y, uint8_t value);
};

#endif // DISPLAYDRIVER_H
