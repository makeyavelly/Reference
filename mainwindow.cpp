#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QUuid>
#include <QSettings>
#include "sql/lastsql.h"

QString genId()
{
    return QUuid::createUuid().toString();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    connectIntoDb();
    loadReferences();
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
        if (ui->checkDb->isChecked()) {
            selectFromDb(reference, index, value);
        } else {
            selectFromReference(reference, index, value);
        }
    }
}

void MainWindow::on_btGenerate_clicked()
{
    generateReferences();
    loadReferences();
}

void MainWindow::connectIntoDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("test");
    db.setUserName("postgres");
    db.setPassword("1");
    if (!db.open()) {
        throw QString("Нет соединения с БД");
    }
    sql::Connect(db);
}

void MainWindow::loadSettings()
{
    QSettings ini("settings.ini", QSettings::IniFormat);
    ini.beginGroup("Count");
    countReference = ini.value("Reference", 0).toInt();
    countRecord = ini.value("Record", 0).toInt();
    ui->spinCountReference->setValue(countReference);
    ui->spinCountRecord->setValue(countRecord);
    ini.endGroup();
}

void MainWindow::saveSettings()
{
    QSettings ini("settings.ini", QSettings::IniFormat);
    ini.beginGroup("Count");
    ini.setValue("Reference", countReference);
    ini.setValue("Record", countRecord);
    ini.endGroup();
}

void MainWindow::generateReferences()
{
    countReference = ui->spinCountReference->value();
    countRecord = ui->spinCountRecord->value();
    for (int i = 1; i <= countReference; ++i) {
        QStringList transact;
        transact.push_back(QString("DROP TABLE IF EXISTS ref_%1; "
                                   "CREATE TABLE ref_%1 ("
                                   "id UUID, "
                                   "number INTEGER, "
                                   "name TEXT);").arg(i));
        for (int j = 0; j < countRecord; ++j) {
            transact.push_back(QString("INSERT INTO ref_%1 (id, number, name) "
                                       "VALUES ('%2', %3, '%4');")
                               .arg(i)
                               .arg(genId())
                               .arg(j % 20 + 1)
                               .arg(QString("Объект из справочника №%1 %2").arg(i).arg(j + 1)));
        }
        sql::exec(transact);
    }
    saveSettings();
    ui->editResult->setTextColor(QColor("#000000"));
    ui->editResult->setText("Генерация справочников прошла успешно!");
}

void MainWindow::loadReferences()
{
    references.clear();
    for (int i = 1; i <= countReference; ++i) {
        references.appendReference(QString("ref_%1").arg(i),
                                   ReferenceIndexFields {"id", "number", "name", "id,number"});
    }
}

void MainWindow::selectFromReference(const QString &reference, const QString &index, const QString &value)
{
    try {
        QTime t;
        int count = ui->spinCountSelect->value();
        t.start();
        const QVector<ReferenceRecord*> records = references.getAll(reference, index, value);
        for (int i = 1; i < count; ++i) {
            references.getAll(reference, index, value);
        }
        int time = t.elapsed();
        ui->editResult->append(QString("%1 запросов за %2 ms").arg(count).arg(time));
        if (records.isEmpty()) {
            ui->editResult->setTextColor(QColor("#4F0000"));
            ui->editResult->append("Искомый объект не найден");
        } else {
            ui->editResult->setTextColor(QColor("#000000"));
            for (ReferenceRecord *record : records) {
                QStringList text;
                for (int i = 0; i < record->count(); ++i) {
                    text.push_back(record->at(i));
                }
                ui->editResult->append(text.join(" | "));
            }
        }
    } catch (const QString &error) {
        ui->editResult->setTextColor(QColor("#4F0000"));
        ui->editResult->append(error);
    }
}

void MainWindow::selectFromDb(const QString &reference, const QString &index, const QString &value)
{
    try {
        QTime t;
        int count = ui->spinCountSelect->value();
        QString select = QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3';")
                .arg(reference)
                .arg(index)
                .arg(value);
        t.start();
        sql::Table table = sql::Select(select);
        for (int i = 1; i < count; ++i) {
            sql::Select(select);
        }
        int time = t.elapsed();
        ui->editResult->append(QString("%1 запросов за %2 ms").arg(count).arg(time));
        if (table.isEmpty()) {
            ui->editResult->setTextColor(QColor("#4F0000"));
            ui->editResult->append("Искомый объект не найден");
        } else {
            ui->editResult->setTextColor(QColor("#000000"));
            for (sql::Record &record : table) {
                QStringList text;
                for (int i = 0; i < record.count(); ++i) {
                    text.push_back(record.get(i).toString());
                }
                ui->editResult->append(text.join(" | "));
            }
        }
    } catch (const QString &error) {
        ui->editResult->setTextColor(QColor("#4F0000"));
        ui->editResult->append(error);
    }
}
