#ifndef LASTSQL_H
#define LASTSQL_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QStringList>

namespace sql
{

    enum Order
    {
        AscOrder  = 0,
        DescOrder = 1
    };

    class Field
    {
    public:
        QString name;
        QString type;
        QString comment;
        QVariant value;
        bool isNull;
        bool isKey;

        Field(QString name = QString(), QVariant value = QVariant(), QString type = QString(),
              QString comment = QString(), bool isNull = true, bool isKey = false);

        QString getValue() const;

        QString getSetValueString() const;
    };

    class Record : public QList<Field>
    {
    public:
        bool isValidIndex(int index);
        QVariant get(int index);
        QVariant get(QString name);
        QString getString(QString name);

        QString getSetValueString();
    };

    class Table : public QList<Record>
    {
    public:
        int countRow();
        int countCol();
        bool isValidIndex(int row, int col);
        QVariant cell(int row, int col);

        QStringList header();
    };

    static QSqlDatabase m_db;
    static QString m_lastError;
    QSqlDatabase &db();
    QString GetDatabaseName();
    QString User();
    QString Password();
    QString Host();
    bool isError();
    QString lastError();

    void setDatabaseName(QString dbName);

    void Connect(const QSqlDatabase &db);
    QSqlQuery CreateQuery();

    void exec(QString script);
    void exec(QStringList transact);
    void exec(QString script, const char * error);
    QSqlQuery Exec(QString script);
    sql::Table Select(QString script);
    sql::Table Select(QString table, QString fields, QString where = QString(),
                      QString order = QString(), Order orderType = AscOrder);
    sql::Table Select(QString table, QStringList fields, QStringList where = QStringList(),
                      QStringList order = QStringList(), Order orderType = AscOrder);

    void Delete(QString table, QString where = QString());
    void Delete(QString table, QStringList where);

    void insert(QString table, sql::Record record);

    void Update(QString table, sql::Record record, QString where = QString());
    void Update(QString table, sql::Field field, QString where = QString());

    sql::Record GetRecord(QString script);
    QVariant GetOneValue(QString script);
    int GetInt(QString script);
    long long GetLong(QString script);
    double GetDouble(QString script);
    QString GetString(QString script);
    QByteArray GetByteArray(QString script);
    bool GetBool(QString script);

    void ClearTable(QString tableName, QString schemaName = "public");

    bool isTableExists(QString tableName, QString schemaName = "public");
    bool isViewExists(QString viewName, QString schemaName = "public");
    bool isTableEmpty(QString tableName, QString schemaName = "public");

    bool isFieldExists(const QString &tableName, const QString &fieldName, const QString &schemaName = "public");

    QString quote(QString text);
    QString quote2(QString text);

    QString getTableName(QString tableName, QString schemaName = "public");
}

#endif // LASTSQL_H
