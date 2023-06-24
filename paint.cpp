#include <QPainter>
#include "paint.h"

Colours::Colours(QWidget* parent) : QWidget(parent), pm(10000, 10000) {
    pm.fill();
}

void Colours::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);
    doPainting();
}

void Colours::doPainting() {
    QPainter painter(this);
    painter.setPen(Qt::black);

    painter.drawPixmap(0, 0, w, h, pm);
}

void Colours::next(int t) {
    QPainter painter(&pm);
    for (int _ = 0; _ < t; _++) {
        int it = this->cols[std::make_pair(this->cur_x, this->cur_y)];
        QChar c = this->command[it];
        int col = (it + 1) % command.size();
        this->cols[{cur_x, cur_y}] = col;
        int cc = 255 - col * 240 / (this->command.size()/* / 2*/ - 1) - 15;
        painter.fillRect(this->added_w + this->cur_x, this->added_h + this->cur_y, this->step, this->step, QColor(cc, cc, cc));
        if (c == 'L') {
            this->dir--;
        } else if (c == 'R') {
            this->dir++;
        } else if (c == 'B') {
            this->dir += 2;
        }
        this->dir = (this->dir + 4) % 4;
        if (this->dir == 0) {
            this->cur_y -= this->step;
        } else if (this->dir == 2) {
            this->cur_y += this->step;
        } else if (this->dir == 1) {
            this->cur_x += this->step;
        } else {
            this->cur_x -= this->step;
        }
    }
    update();
}

void Colours::reset() {
    this->cur_x = 300;
    this->cur_y = 300;
    this->dir = 0;
    this->added_h = 0;
    this->added_w = 0;
    this->cols = {};
    pm.fill();
    update();
}

void Colours::quit() {
    this->close();
}

void Colours::move_screen(int dx, int dy) {
    this->added_h += dx;
    this->added_w += dy;
//    QWidget::move(dx, dy);
}

void Colours::redraw() {
    pm.fill();
    update();
    QPainter painter(&pm);
    for (auto i : this->cols) {
        int col = i.second;
        int cc = 255 - col * 240 / (this->command.size()/* / 2*/ - 1) - 15;
        painter.fillRect(this->added_w + i.first.first, this->added_h + i.first.second, this->step, this->step, QColor(cc, cc, cc));
    }
    update();
}
