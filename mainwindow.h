#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <deque>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void handleLine(QString line);
    QObject* findFirstByName(QObject* parent, QString postfix);
    int getValue(QObject* object);
    void setValue(QObject* object, int state);
    void activate(bool);

public slots:
    void run(bool runit=true);
    void triggered(QString line);

private:
    Ui::MainWindow *ui;
    std::deque<QString> stack;
};
#endif // MAINWINDOW_H
