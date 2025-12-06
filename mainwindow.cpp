#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <QTimer>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Mode
    connect(ui->radioButton_record, &QRadioButton::clicked, ui->groupBox_in, &QGroupBox::setEnabled);
    connect(ui->radioButton_record, &QRadioButton::clicked, ui->groupBox_operations, &QGroupBox::setEnabled);
    connect(ui->radioButton_operate, &QRadioButton::clicked, ui->groupBox_in, &QGroupBox::setDisabled);
    connect(ui->radioButton_operate, &QRadioButton::clicked, ui->groupBox_operations, &QGroupBox::setDisabled);
    connect(ui->radioButton_record, &QRadioButton::clicked, this, &MainWindow::activate);

    // Ops
    connect(ui->pushButton_plus, &QPushButton::clicked, [main=this] {main->triggered("+");});
    connect(ui->pushButton_minus, &QPushButton::clicked, [main=this] {main->triggered("-");});
    connect(ui->pushButton_times, &QPushButton::clicked, [main=this] {main->triggered("*");});
    connect(ui->pushButton_divide, &QPushButton::clicked, [main=this] {main->triggered("/");});
    connect(ui->pushButton_rest, &QPushButton::clicked, [main=this] {main->triggered("%");});

    connect(ui->pushButton_or, &QPushButton::clicked, [main=this] {main->triggered("||");});
    connect(ui->pushButton_and, &QPushButton::clicked, [main=this] {main->triggered("&&");});
    connect(ui->pushButton_not, &QPushButton::clicked, [main=this] {main->triggered("!");});
    connect(ui->pushButton_bitewise_and, &QPushButton::clicked, [main=this] {main->triggered("&");});
    connect(ui->pushButton_bitwise_or, &QPushButton::clicked, [main=this] {main->triggered("|");});
    connect(ui->pushButton_explusive_or, &QPushButton::clicked, [main=this] {main->triggered("^");});
    connect(ui->pushButton_sleep, &QPushButton::clicked, [main=this] {main->triggered("sleep");});

    connect(ui->pushButton_gt, &QPushButton::clicked, [main=this] {main->triggered(">");});
    connect(ui->pushButton_ge, &QPushButton::clicked, [main=this] {main->triggered(">=");});
    connect(ui->pushButton_eq, &QPushButton::clicked, [main=this] {main->triggered("==");});
    connect(ui->pushButton_le, &QPushButton::clicked, [main=this] {main->triggered("<=");});
    connect(ui->pushButton_lt, &QPushButton::clicked, [main=this] {main->triggered("<");});
    connect(ui->pushButton_ne, &QPushButton::clicked, [main=this] {main->triggered("!=");});
    connect(ui->pushButton_assign, &QPushButton::clicked, [main=this] {main->triggered("=>");});

    // Test
    connect(ui->pushButton_test, &QPushButton::clicked,this, &MainWindow::run);
}

void MainWindow::activate(bool on) {

    static std::deque<QMetaObject::Connection> connections;

    if(on && connections.size() == 0) {
        // Recording
        connections.push_front(connect(ui->checkBox_A, &QCheckBox::checkStateChanged, [main=this] {
            main->triggered("checkBox_A");
        }));
        connections.push_front(connect(ui->checkBox_B, &QCheckBox::checkStateChanged, [main=this] {
            main->triggered("checkBox_B");
        }));
        connections.push_front(connect(ui->checkBox_C, &QCheckBox::checkStateChanged, [main=this] {
            main->triggered("checkBox_C");
        }));
        connections.push_front(connect(ui->checkBox_F, &QCheckBox::checkStateChanged, [main=this] {
            main->triggered("checkBox_F");
        }));
        connections.push_front(connect(ui->checkBox_G, &QCheckBox::checkStateChanged, [main=this] {
            main->triggered("checkBox_G");
        }));
        connections.push_front(connect(ui->checkBox_H, &QCheckBox::checkStateChanged, [main=this] {
            main->triggered("checkBox_H");
        }));
        connections.push_front(connect(ui->spinBox_D, &QSpinBox::valueChanged, [main=this] {
            main->triggered("spinBox_D");
        }));
        connections.push_front(connect(ui->spinBox_E, &QSpinBox::valueChanged, [main=this] {
            main->triggered("spinBox_E");
        }));
        connections.push_front(connect(ui->spinBox_I, &QSpinBox::valueChanged, [main=this] {
            main->triggered("spinBox_I");
        }));
        connections.push_front(connect(ui->spinBox_J, &QSpinBox::valueChanged, [main=this] {
            main->triggered("spinBox_J");
        }));

    } else {
        for(;connections.size() > 0;connections.pop_front()) {
            disconnect(connections.at(0));
        }
    }
}

void MainWindow::run(bool runit)
{
    static bool running = false;
    this->activate(false);
    if(!running) {
        running = true;
        auto rules = ui->plainTextEdit_rules->toPlainText();
        auto lines = rules.split("\n");
        for(auto line: lines) {
            //std::cout << "line: " << line.toStdString() << std::endl;
            handleLine(line);
        }
        running = false;
    }
    this->activate(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleLine(QString line) {
    try {
        if(line.startsWith("=>")) {
            int state;
            auto target = stack.at(0);
            stack.pop_front();
            auto source = stack.at(0);
            stack.pop_front();
            std::cout << source.toStdString() << line.toStdString() << target.toStdString() << std::endl;

            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            auto targetWidget = findFirstByName(ui->centralwidget, target);
            if(targetWidget != nullptr) {
                setValue(targetWidget, state);
            }
        } else if(line.startsWith("sleep")) {
            int state;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            QThread::msleep(state);
        } else if(line.startsWith("&&")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result && state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("||")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result || state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith(">=")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result >= state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("==")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result == state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("<=")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result <= state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("!=")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result != state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("&")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result & state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("|")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result | state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("^")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result ^ state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("+")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result + state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("-")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result - state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("*")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result * state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("/")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            if(state != 0) {
                result = result / state;
                stack.push_front(QString::number(result));
            } else {
                stack.push_front(QString("division by null"));
            }
        } else if(line.startsWith("%")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            if(state != 0) {
                result = result % state;
                stack.push_front(QString::number(result));
            } else {
                stack.push_front(QString("division by null"));
            }
        } else if(line.startsWith("<")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result < state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith(">")) {
            int state, result;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = state;
            source = stack.at(0);
            stack.pop_front();
            sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            result = result > state;
            stack.push_front(QString::number(result));
        } else if(line.startsWith("!")) {
            int state;
            auto source = stack.at(0);
            stack.pop_front();
            auto sourceWidget = findFirstByName(ui->centralwidget, source);
            if(sourceWidget != nullptr) {
                state = getValue(sourceWidget);
            } else {
                state = source.toInt();
            }
            state = !state;
            stack.push_front(QString::number(state));
        } else if(line.trimmed().size() < 1) {
            // NOP
        } else {
            stack.push_front(line);
            std::cout << "push " << line.toStdString() << " on stack" << std::endl;
            return;
        }
        if(stack.empty()) {
            std::cout << "stack is empty!" << std::endl;
        } else {
            std::cout << "first on stack: " << stack.at(0).toStdString() << std::endl;
        }
    } catch (...) {
        std::cerr << "error in rule" << std::endl;
    }
}

int MainWindow::getValue(QObject* object) {
    if(object->objectName().startsWith("checkBox_")) {
        auto widget = dynamic_cast<QCheckBox*>(object);
        auto state = widget->checkState();
        if(state == Qt::Checked) {
            return 1;
        } else {
            return 0;
        }
    } else if(object->objectName().startsWith("spinBox_")) {
        auto widget = dynamic_cast<QSpinBox*>(object);
        return widget->value();
    } else {
        std::cerr << "getValue: " << object->objectName().toStdString() << " not supported!" << std::endl;
    }

    return 0;
}
void MainWindow::setValue(QObject* object, int state) {
    if(object->objectName().startsWith("checkBox_")) {
        auto widget = dynamic_cast<QCheckBox*>(object);
        widget->setChecked(state != 0);
    } else if(object->objectName().startsWith("spinBox_")) {
        auto widget = dynamic_cast<QSpinBox*>(object);
        widget->setValue(state);
    } else {
        std::cerr << "setValue: " << object->objectName().toStdString() << " not supported!" << std::endl;
    }
}

QObject *MainWindow::findFirstByName(QObject* parent, QString postfix) {
    for(auto child: parent->children()) {
        auto name = child->objectName();
        //std::cout << parent->objectName().toStdString() << ": " << name.toStdString() << std::endl;
        if(name.compare(postfix) == 0) {
            return child;
        } else {
            auto found = findFirstByName(child, postfix);
            if(found != nullptr) {
                return found;
            }
        }
    }
    return nullptr;
}

void MainWindow::triggered(QString line)
{
    if(ui->radioButton_record->isChecked()) {
        ui->plainTextEdit_rules->appendPlainText(line);
    } else if(line.contains("Box_")) {
        run();
    }
}
