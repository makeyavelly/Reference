#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>

QString genId(long long number)
{
    QString res = QString::number(number);
    while (res.length() < 36) {
        res.push_front("0");
    }
    res.push_front("id-");
    return res;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    generateReferences();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btSelect_clicked()
{
    ui->editResult->clear();
    const QString reference = ui->editReference->text();
    const QString index = ui->editIndex->text();
    const QString value = ui->editValue->text();
    if (reference.isEmpty() || index.isEmpty() || value.isEmpty()) {
        ui->editResult->setTextColor(QColor("#4F0000"));
        ui->editResult->append("Необходимо заполнить все параметры запроса");
    } else {
        try {
            QTime t;
            int count = ui->spinCountSelect->value();
            t.start();
            const ReferenceRecord *record = references.get(reference, index, value);
            for (int i = 1; i < count; ++i) {
                record = references.get(reference, index, value);
            }
            int time = t.elapsed();
            ui->editResult->append(QString("%1 запросов за %2 ms").arg(count).arg(time));
            if (record) {
                ui->editResult->setTextColor(QColor("#000000"));
                QStringList text;
                for (int i = 0; i < record->count(); ++i) {
                    text.push_back(record->at(i));
                }
                ui->editResult->append(text.join(" | "));
            } else {
                ui->editResult->setTextColor(QColor("#4F0000"));
                ui->editResult->append("Искомый объект не найден");
            }
        } catch (const QString &error) {
            ui->editResult->setTextColor(QColor("#4F0000"));
            ui->editResult->append(error);
        }
    }
}

void MainWindow::generateReferences()
{
    for (int i = 1; i <= 200; ++i) {
        ReferenceHeader header {"id", "number", "name"};
        ReferenceTable values;
        for (int j = 0; j < 1000; ++j) {
            values.push_back(ReferenceRecord {genId(100000000LL * i + j),
                                              QString::number(j),
                                              QString("Объект из справочника №%1 %2").arg(i).arg(j)});
        }
        ReferenceIndexFields indexs {"id", "number", "name"};
        references.appendReference(QString("ref_%1").arg(i),
                                   new Reference(header, values, indexs));
    }
}
