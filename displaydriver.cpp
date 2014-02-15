#include "displaydriver.h"


void DisplayDriver::setRegister(int8_t reg)
{
    this->reg = reg;
    //qDebug() << "Registre " << reg;
}


void DisplayDriver::putData(uint8_t value)
{
    //qDebug() << "Value " << value;
    if(this->reg == 1) //Couter mode
    {
        counterMode = (value&0x3);
        //qDebug() << "--------- Counter mode : " << counterMode;
    }
    else if(this->reg == 4) //Set the x/y position
    {
        if(((value>>6)&3) == 3) // y address
        {
            value = (value & 0x3F);
            if(value <= 63)
                this->y = value;
            else
                qDebug() << "Y value out of range 0->63 in display driver";

            //qDebug() << "----Y : " << y;
        }
        else if(((value>>7)&1) == 0) // x address
        {
            value = value & 0x1F;
            if(value <= 19)
                this->x = value;
            else
                qDebug() << "X value out of range 0->19 in display driver";
           // qDebug() << "----x : " << x;
        }
        else
        {
            qDebug() << "Unsuported value in display driver register 4";
        }
    }
    else if(this->reg == 7) //data write
    {
        emit setScreenByte(this->x,this->y,value);
        emit refresh();
        //Counter
        if(counterMode == 0 && x > 0)//X Down
            this->x--;
        else if(counterMode == 1 && x < 19)//X Up
            this->x++;
        else if(counterMode == 2 && y > 0)//Y Down
            this->y--;
       else  if(counterMode == 3 && y < 63)//Y Up
            this->y++;
    }
    else
    {
        //qDebug() << "Unknown register";
    }
}
