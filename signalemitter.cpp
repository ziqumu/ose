#include "signalemitter.h"

void signalEmitter::setScreen(Screen* screen)
{
    this->screen = screen;
}

void signalEmitter::initCoreSignal(QString flashLink, DisplayDriver* DD)
{
   emit initCore(flashLink, DD);
}

void signalEmitter::testSignal()
{
   emit test();
}

void signalEmitter::ScreenSetByte(uint8_t x, uint8_t y, uint8_t byte)
{
    qDebug() << "Signal reçu" ;
    this->screen->setByte(x, y, byte);
}
