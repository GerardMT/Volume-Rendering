
#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>
#include <ui_main_window.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    Ui::MainWindow ui;

private slots:
    void on_actionOpen_triggered();

     void on_actionQuit_triggered();
};

#endif  //  MAIN_WINDOW_H_
