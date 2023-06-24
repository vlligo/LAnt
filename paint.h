#pragma once

#include <QWidget>
#include <QPainter>
#include <unordered_map>

class Colours : public QWidget {

public:
    QString command;
    int step = 1;
    explicit Colours(QWidget *parent = nullptr);
    void next(int t);
    void reset();
    void quit();
protected:
    void paintEvent(QPaintEvent *e) override;

private:
    int added_w = 0, added_h = 0;
    std::map<std::pair<int, int>, int> cols;
    int w = 10000, h = 10000;
    int cur_x = 300, cur_y = 300;
    int dir = 0;
    QPixmap pm;
    void doPainting();
};