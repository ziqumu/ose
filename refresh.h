#ifndef REFRESH_H
#define REFRESH_H

#include <QTimer>

class Refresh : public QTimer
{
    Q_OBJECT
private:
    bool needRefresh;
public:
    explicit Refresh(int millis = 25);
    void refreshOrder();

signals:
    void refresh();

private slots:
    void refreshTimer();

};

#endif // REFRESH_H
