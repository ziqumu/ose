#ifndef SCREEN_H
#define SCREEN_H

#include <QLabel>
#include <QTableWidgetItem>
#include <cstdint>
#include "refresh.h"

namespace Ui {
class Screen;
class MainWindow;
}
class Screen : public QLabel
{
    Q_OBJECT
public:
    explicit Screen(QWidget *parent = 0);
    uint8_t getByte(int x, int y);
    void setByte(uint8_t x, uint8_t y, uint8_t byte);

protected:
    void paintEvent(QPaintEvent *);
    Refresh *refreshTimer;



private:
    unsigned char vram[1024];
	unsigned char display[1024];
	void drawByte(QPainter *painter, char byte, short coord);
    Ui::MainWindow *ui;
public slots:
    void refresh();
    void refreshOrder();
};

#endif // SCREEN_H
