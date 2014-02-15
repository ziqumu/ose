#include "screen.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>

Screen::Screen(QWidget *parent) : QLabel(parent)
{
	for(int i=0;i<1024;i++)
        this->display[i]=0xFF;

    /*this->display[440]=60;
    this->display[456]=126;
    this->display[472]=251;
    this->display[488]=253;
    this->display[504]=253;
    this->display[520]=255;
    this->display[536]=126;
    this->display[552]=60;

    this->setByte(10,10,0xaa);*/

    //Screen refresh class, to be sure the screen refresh every 25ms max (without this class, a lot of refresh signals are send in the same time and the emulator gui crash)
    refreshTimer = new Refresh;
    connect(refreshTimer, SIGNAL(refresh()), this, SLOT(refresh()));
}

void Screen::drawByte(QPainter* painter,char byte,short coord)
{
	unsigned short l=(int)this->width()/128;
	unsigned short h=(int)this->height()/64;
//Dessin en noir
	painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
	painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
    for(int i=7; i>=0 ; i--)
	{
		if(byte>>i&1)
            painter->drawRect((coord%16*8+7-i)*this->width()/128,coord/16*this->height()/64,l,h);
	}
 //Dessin en blanc
	painter->setPen(QPen(Qt::white, 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
	painter->setBrush(QBrush(Qt::white, Qt::SolidPattern));
    for(int i=7; i>=0 ; i--)
	{
		if((byte>>i&1) == 0)
            painter->drawRect((coord%16*8+7-i)*this->width()/128,coord/16*this->height()/64,l,h);
	}
}


void Screen::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for(int i=0;i<1024;i++)
        drawByte(&painter,this->display[i],i);
}

void Screen::setByte(uint8_t x, uint8_t y, uint8_t byte) //Begin in top left
{
    this->display[16*y+x] = byte;
    //this->repaint();
}

void Screen::refresh()
{
    this->repaint();
}

void Screen::refreshOrder()
{
    this->refreshTimer->refreshOrder();
}

uint8_t Screen::getByte(int x, int y)
{
    return this->display[16*y+x];
}

