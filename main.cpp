#include <QApplication>
#include <QTranslator>
#include <QLabel>
#include <QRectF>
#include <QWidget>
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include "paint.h"

bool isdigit(char ch) {
    return std::isdigit(static_cast<unsigned char>(ch));
}

int get_num(const QString& text, int l, int r) {
    int ans = 0;
    for (int i = l; i < r; i++) {
        ans *= 10;
        ans += text.at(i).toLatin1() - '0';
    }
    return ans;
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Панель управления");

    auto* c = new QWidget(nullptr);

    Colours window;
    window.resize(600, 600);
    window.setWindowTitle("Поле муравья");
    window.set_new_step(10, std::make_pair(300, 300));
    auto* label_command = new QLabel(c);
    QString command = "L 1; R 1;";
    window.command = "LR";
    QString pref = "Текущая команда: ";
    label_command->setText(pref + command);
    label_command->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    label_command->setGeometry(10, 60, label_command->text().size() * 8, 40);
    int one_char_len = 9;
    auto read_command = [&]() {
        bool ok;
        QString text = QInputDialog::getText(c, "Новая команда",
                                             "Новая команда:", QLineEdit::Normal,
                                             command, &ok);
        if (ok && !text.isEmpty()) {
            QString tot_text = pref + text;
            // check that command is valid
            int st = -1;
            QString long_c = "";
            QChar curr_command;
            for (int j = 0; j < (int)text.size(); j++) {
                QChar i = text[j];
                if (i == ' ') {
                    if (st != -1 && isdigit(text.at(st).toLatin1())) {
                        tot_text = "Некорректный ввод.\n" + pref + command;
                        label_command->resize(
                                (tot_text.size() - 14) * one_char_len,
                                label_command->height());
                        label_command->setText(tot_text);
                        c->resize(std::max(c->width(), label_command->width() + 10), c->height());
                        return;
                    }
                    continue;
                }
                if (i == 'L' || i == 'R' || i == 'F' || i == 'B') {
                    if (st != -1) {
                        tot_text = "Некорректный ввод.\n" + pref + command;
                        label_command->resize(
                                (tot_text.size() - 14) * one_char_len,
                                label_command->height());
                        label_command->setText(tot_text);
                        c->resize(std::max(c->width(), label_command->width() + 10), c->height());
                        return;
                    }
                    curr_command = i;
                    st = j;
                }
                if (isdigit(i.toLatin1()))
                    st = j;
                if (i == ';') {
                    if (st == -1) {
                        tot_text = "Некорректный ввод.\n" + pref + command;
                        label_command->resize(
                                (tot_text.size() - 14) * one_char_len,
                                label_command->height());
                        label_command->setText(tot_text);
                        c->resize(std::max(c->width(), label_command->width() + 10), c->height());
                        return;
                    } else {
                        int count = get_num(text, st, j);
                        for (int _ = 0; _ < count; _++) {
                            long_c += curr_command;
                        }
                        st = -1;
                    }
                }
            }
            if (st != -1) {
                tot_text = "Некорректный ввод.\n" + pref + command;
                label_command->resize(
                        (tot_text.size() - 14) * one_char_len,
                        label_command->height());

                label_command->setText(tot_text);
                c->resize(std::max(c->width(), label_command->width() + 10), c->height());
                return;
            }
            // submit  a command
            command = text;
            label_command->setText(tot_text);
            label_command->resize(tot_text.size() * one_char_len, label_command->height());
            c->resize(std::max(c->width(), label_command->width() + 10), c->height());
            window.reset();
            window.command = long_c;
            window.set_new_step(10, std::make_pair(300, 300));
        }
    };
    auto* button_read_new_command = new QPushButton("Новая команда", c);
    button_read_new_command->resize(200, 50);
    QWidget::connect(button_read_new_command, &QPushButton::pressed, c, read_command);
    button_read_new_command->show();
    label_command->show();

    // Ant simulation
    // buttons of next turns
    auto next_turn = [&]() {
        window.next(1);
    };
    auto* button_next = new QPushButton("Сделать шаг", c);
    button_next->setGeometry(200, 0, 100, 50);
    QWidget::connect(button_next, &QPushButton::pressed, c, next_turn);
    button_next->show();
    QString last_custom_number = "1000000";
    auto next_last_custom_turns = [&]() {
        window.next(last_custom_number.toInt());
    };
    auto* button_next_last_custom = new QPushButton("Сделать " + last_custom_number + " шагов", c);
    button_next_last_custom->setGeometry(300, 0, 200, 50);
    QWidget::connect(button_next_last_custom, &QPushButton::pressed, c, next_last_custom_turns);
    button_next_last_custom->show();
    auto next_custom_turns = [&]() {
        bool ok;
        QString text = QInputDialog::getText(c, "Новое количество шагов",
                                             "Количество шагов:", QLineEdit::Normal,
                                             last_custom_number, &ok);
        bool isNum;
        int value = text.toInt(&isNum);
        if (ok && !text.isEmpty()) {
            if (isNum) {
                last_custom_number = text;
                int lcn = last_custom_number.toInt() % 100;
                if ((5 <= lcn && lcn <= 20) || lcn % 10 == 0 || (lcn % 10 > 4 && lcn % 10 <= 9))
                button_next_last_custom->setText("Сделать " + last_custom_number + " шагов");
                else if (lcn % 10 == 1)
                    button_next_last_custom->setText("Сделать " + last_custom_number + " шаг");
                else
                    button_next_last_custom->setText("Сделать " + last_custom_number + " шагa");
                window.next(value);
            } else {
                QMessageBox msgBox;
                msgBox.setText("Некорректное число(" + text + ")");
                msgBox.exec();
            }
        }
    };
    auto* button_next_custom = new QPushButton("Новое количество шагов", c);
    button_next_custom->setGeometry(500, 0, 200, 50);
    QWidget::connect(button_next_custom, &QPushButton::pressed, c, next_custom_turns);
    // Clearing the field and quiting of all windows buttons
    auto change_step = [&]() {
        bool ok;
        QString text = QInputDialog::getText(c, "Размер квадрата",
                                             "Размер[пк]:", QLineEdit::Normal,
                                             (QString::number(window.step)), &ok);
        bool isNum;
        int value = text.toInt(&isNum);
        if (ok && !text.isEmpty()) {
            if (isNum) {
                bool ok_;
                QString text_ = QInputDialog::getText(c, "Центр гомотетии",
                                                      "Напишите координату по вертикали, и через пробел горизонтальную координату[пк]:", QLineEdit::Normal,
                                                      (QString("300 300")), &ok_);
                if (ok_ && !text_.isEmpty()) {
                    bool was = false;
                    QString dh, dw;
                    for (auto i: text_) {
                        if (i == ' ' && !dh.isEmpty()) {
                            was = true;
                            continue;
                        }
                        if (!(('0' <= i && i <= '9') || i == '-')) {
                            QMessageBox msgBox;
                            msgBox.setText("Некорректные два числа(" + text + ")");
                            msgBox.exec();
                            return;
                        }
                        if (was) {
                            dw += i;
                        } else {
                            dh += i;
                        }
                    }
                    if (dh.isEmpty())
                        dh = "0";
                    if (dw.isEmpty())
                        dw = "0";
                    int int_dh = dh.toInt(), int_dw = dw.toInt();
                    window.set_new_step(value, std::make_pair(int_dh, int_dw));
                }
            } else {
                QMessageBox msgBox;
                msgBox.setText("Некорректное число(" + text + ")");
                msgBox.exec();
            }
        }
    };
    auto* button_step = new QPushButton("Новый размер квадрата", c);
    button_step->setGeometry(700, 0, 200, 50);
    QWidget::connect(button_step, &QPushButton::pressed, c, change_step);
    button_step->show();
    auto move_field = [&]() {
        bool ok;
        QString text = QInputDialog::getText(c, "Сдвинуть поле",
                                             "Напишите длину вертикального перемещения, и через пробел длину горизонтального перемещения[пк]:", QLineEdit::Normal,
                                             (QString("10 10")), &ok);
        if (ok && !text.isEmpty()) {
            bool was = false;
            QString dh, dw;
            for (auto i : text) {
                if (i == ' ' && !dh.isEmpty()) {
                    was = true;
                    continue;
                }
                if (!(('0' <= i && i <= '9') || i == '-')) {
                    QMessageBox msgBox;
                    msgBox.setText("Некорректные два числа(" + text + ")");
                    msgBox.exec();
                    return;
                }
                if (was) {
                    dw += i;
                } else {
                    dh += i;
                }
            }
            if (dh.isEmpty())
                dh = "0";
            if (dw.isEmpty())
                dw = "0";
            int int_dh = dh.toInt(), int_dw = dw.toInt();
            window.move_screen(int_dh, int_dw);
            window.redraw();
        }
    };
    auto* button_move = new QPushButton("Сдвинуть поле", c);
    button_move->setGeometry(900, 0, 200, 50);
    QWidget::connect(button_move, &QPushButton::pressed, c, move_field);
    button_move->show();
    auto clear_field = [&]() {
        window.reset();
    };

    auto measure = [&]() {
        QMessageBox msgBox1;
        msgBox1.setText("У Вас секунда для наводки мышкой на нужную точку");
        msgBox1.setStandardButtons(QMessageBox::NoButton);
        QTimer::singleShot(1000, &msgBox1, &QMessageBox::accept);
        msgBox1.exec();
        QMessageBox msgBox;
        msgBox.setText("Координаты мыши на поле: " + QString::number(window.cursor().pos().x() - window.x()) + "; " + QString::number(window.cursor().pos().y() - window.y()) + "");
        msgBox.exec();
    };
    auto* button_measure = new QPushButton("Координаты мыши", c);
    button_measure->setGeometry(1100, 0, 200, 50);
    QWidget::connect(button_measure, &QPushButton::pressed, c, measure);
    button_measure->show();

    auto* button_clear = new QPushButton("Очистить", c);
    button_clear->setGeometry(1300, 0, 100, 50);
    QWidget::connect(button_clear, &QPushButton::pressed, c, clear_field);
    button_clear->show();
    auto quit_all = [&]() {
        window.quit();
        c->close();
    };
    auto* button_quit = new QPushButton("Выход", c);
    button_quit->setGeometry(1400, 0, 100, 50);
    QWidget::connect(button_quit, &QPushButton::pressed, c, quit_all);
    button_quit->show();
    window.show();
    c->show();
    return QApplication::exec();
}