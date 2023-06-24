#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLabel>
#include <QRectF>
#include <QWidget>
#include <QInputDialog>
#include <QDir>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>
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

    auto* c = new QWidget(nullptr);

    Colours window;
    window.resize(480, 360);
    window.setWindowTitle("Ant field");

    auto* label_command = new QLabel(c);
    QString command = "R 1; L 1;";
    window.command = "RL";
    QString pref = "Current command is: ";
    label_command->setText(pref + command);
    label_command->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    label_command->setGeometry(10, 60, label_command->text().size() * 8, 40);
    int one_char_len = 9;
    auto read_command = [&]() {
        bool ok;
        QString text = QInputDialog::getText(c, "Langton ant command line",
                                             "Command to the ant:", QLineEdit::Normal,
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
                        tot_text = "Invalid input.\n" + pref + command;
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
                        tot_text = "Invalid input.\n" + pref + command;
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
                        tot_text = "Invalid input.\n" + pref + command;
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
                tot_text = "Invalid input.\n" + pref + command;
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
        }
    };
    auto* button_read_new_command = new QPushButton("New command", c);
    button_read_new_command->resize(200, 50);
    QWidget::connect(button_read_new_command, &QPushButton::pressed, c, read_command);
    button_read_new_command->show();
    label_command->show();

    // Ant simulation
    // buttons of next turns
    auto next_turn = [&]() {
        window.next(1);
    };
    auto* button_next = new QPushButton("Next", c);
    button_next->setGeometry(200, 0, 100, 50);
    QWidget::connect(button_next, &QPushButton::pressed, c, next_turn);
    button_next->show();
    QString last_custom_number = "1000000";
    auto next_last_custom_turns = [&]() {
        window.next(last_custom_number.toInt());
    };
    auto* button_next_last_custom = new QPushButton("Next " + last_custom_number, c);
    button_next_last_custom->setGeometry(300, 0, 100, 50);
    QWidget::connect(button_next_last_custom, &QPushButton::pressed, c, next_last_custom_turns);
    button_next_last_custom->show();
    auto next_custom_turns = [&]() {
        bool ok;
        QString text = QInputDialog::getText(c, "Custom number of turns",
                                             "Number of turns:", QLineEdit::Normal,
                                             last_custom_number, &ok);
        bool isNum;
        int value = text.toInt(&isNum);
        if (ok && !text.isEmpty()) {
            if (isNum) {
                last_custom_number = text;
                button_next_last_custom->setText("Next " + last_custom_number);
                window.next(value);
            } else {
                QMessageBox msgBox;
                msgBox.setText("Invalid number(" + text + ")");
                msgBox.exec();
            }
        }
    };
    auto* button_next_custom = new QPushButton("Next custom", c);
    button_next_custom->setGeometry(400, 0, 100, 50);
    QWidget::connect(button_next_custom, &QPushButton::pressed, c, next_custom_turns);
    // Clearing the field and quiting of all windows buttons
    auto change_step = [&]() {
        bool ok;
        QString text = QInputDialog::getText(c, "The size of square",
                                             "The size[px]:", QLineEdit::Normal,
                                             (QString::number(window.step)), &ok);
        bool isNum;
        int value = text.toInt(&isNum);
        if (ok && !text.isEmpty()) {
            if (isNum) {
                window.step = value;
                window.reset();
            } else {
                QMessageBox msgBox;
                msgBox.setText("Invalid number(" + text + ")");
                msgBox.exec();
            }
        }
    };
    auto* button_step = new QPushButton("Change the size of square", c);
    button_step->setGeometry(500, 0, 200, 50);
    QWidget::connect(button_step, &QPushButton::pressed, c, change_step);
    button_step->show();
    auto clear_field = [&]() {
        window.reset();
    };
    auto* button_clear = new QPushButton("Clear", c);
    button_clear->setGeometry(700, 0, 100, 50);
    QWidget::connect(button_clear, &QPushButton::pressed, c, clear_field);
    button_clear->show();
    auto quit_all = [&]() {
        window.quit();
        c->close();
    };
    auto* button_quit = new QPushButton("Quit", c);
    button_quit->setGeometry(800, 0, 100, 50);
    QWidget::connect(button_quit, &QPushButton::pressed, c, quit_all);
    button_quit->show();
    window.show();
    c->show();
    return QApplication::exec();
}