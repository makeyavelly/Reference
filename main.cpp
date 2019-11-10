#include <QDebug>
#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

void initCodec()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#if QT_VERSION_MAJOR < 5
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = nullptr;
    try {
        initCodec();
        w = new MainWindow();
        w->show();
    } catch (const QString &error) {
        qCritical() << error;
        return 1;
    }
    return a.exec();
}
