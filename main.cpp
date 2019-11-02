#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

void initCodec()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initCodec();
    MainWindow w;
    w.show();

    return a.exec();
}
