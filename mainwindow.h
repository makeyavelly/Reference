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
    void on_btGenerate_clicked();

private:
    Ui::MainWindow *ui;
    int countReference;
    int countRecord;
    ListReference references;

    void connectIntoDb();
    void loadSettings();
    void saveSettings();
    void generateReferences();
    void loadReferences();

    void selectFromReference(const QString &reference, const QString &index, const QString &value);
    void selectFromDb(const QString &reference, const QString &index, const QString &value);
};

#endif // MAINWINDOW_H
