#include "refresh.h"

Refresh::Refresh(int millis)
{
    this->needRefresh = false;
    connect(this, SIGNAL(timeout()), this, SLOT(refreshTimer()));
    this->setInterval(millis);
    this->start();
}

void Refresh::refreshOrder()
{
    this->needRefresh = true;
}

void Refresh::refreshTimer()
{
    if(this->needRefresh)
    {
        emit refresh();
    }
    this->needRefresh = false;
}
