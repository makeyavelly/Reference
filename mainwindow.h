#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Reference/reference.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btSelect_clicked();

private:
    Ui::MainWindow *ui;
    ListReference references;

    void generateReferences();
};

#endif // MAINWINDOW_H
