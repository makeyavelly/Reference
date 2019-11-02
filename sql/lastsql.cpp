#include "lastsql.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>

sql::Field::Field(QString name, QVariant value, QString type, QString comment, bool isNull, bool isKey)
{
    this->name = name;
    this->value = value;
    this->type = type;
    this->comment = comment;
    this->isNull = isNull;
    this->isKey = isKey;
}

QString sql::Field::getValue() const {
    QString res = value.toString();
    if (value.type() == QVariant::Bool) {
        res = value.toBool() ? "true" : "false";
    }
    if (value.type() == QVariant::DateTime) {
        res = value.toDateTime().toString(Qt::ISODate);
    }
    return res;
}

QString sql::Field::getSetValueString() const
{
    return QString("\"%1\"=%2").arg(name).arg(getValue());
}


bool sql::Record::isValidIndex(int index)
{
    return ((index < count()) && (index >= 0));
}

QVariant sql::Record::get(int index)
{
    QVariant res = QVariant();
    if (isValidIndex(index))
    {
        res = at(index).value;
    }
    return res;
}

QVariant sql::Record::get(QString name)
{
    QVariant res = QVariant();
    for (int iField = 0; iField < count(); ++iField)
    {
        if (name == at(iField).name)
        {
            res = get(iField);
            break;
        }
    }
    return res;
}

QString sql::Record::getString(QString name) {
    return get(name).toString().trimmed();
}

QString sql::Record::getSetValueString()
{
    QStringList fields;
    for (int i = 0; i < count(); ++i) {
        fields.append(at(i).getSetValueString());
    }
    return fields.join(",");
}


int sql::Table::countRow()
{
    return count();
}

int sql::Table::countCol()
{
    int res = 0;
    if (countRow() > 0)
    {
        res = first().count();
    }
    return res;
}

bool sql::Table::isValidIndex(int row, int col)
{
    return ((row < countRow()) && (col < countCol()) && (row >= 0) && (col >= 0));
}

QVariant sql::Table::cell(int row, int col)
{
    QVariant res = QVariant();
    if (isValidIndex(row, col))
    {
        Record record = this->at(row);
        res = record.get(col);
    }
    return res;
}

QStringList sql::Table::header()
{
    QStringList res;
    if (countRow() > 0) {
        for (int i = 0; i < countCol(); ++i) {
            res << at(0).at(i).name;
        }
    }
    return res;
}

void sql::Connect(const QSqlDatabase &db)
{
    sql::m_db = db;
    sql::m_db.open();
}

QSqlQuery sql::CreateQuery()
{
    QSqlQuery query(sql::db());
    query.setForwardOnly(true);
    return query;
}

void sql::exec(QString script) {
    QSqlQuery query = sql::CreateQuery();
    if (!query.exec(script)) {
        throw query.lastError().text();
    }
}

void sql::exec(QStringList transact) {
    sql::exec(transact.join(";\n"));
}

void sql::exec(QString script, const char *error) {
    QSqlQuery query = sql::CreateQuery();
    if (!query.exec(script)) {
        throw error;
    }
}

QSqlQuery sql::Exec(QString script)
{
    QSqlQuery query = sql::CreateQuery();
    if (!query.exec(script))
    {
        sql::m_lastError = query.lastError().text();
    } else {
        sql::m_lastError.clear();
    }
    return query;
}

sql::Table sql::Select(QString script)
{
    Table res;
    QSqlQuery query = sql::Exec(script);
    while (query.next())
    {
        Record record;
        for (int iField = 0; iField < query.record().count(); ++iField)
        {
            record.append(Field(query.record().fieldName(iField), query.value(iField)));
        }
        res.append(record);
    }
    return res;
}

sql::Table sql::Select(QString table, QStringList fields, QStringList where, QStringList order, Order orderType) {
    return sql::Select(table, fields.join(","), where.join(" AND "), order.join(","), orderType);
}

sql::Table sql::Select(QString table, QString fields, QString where, QString order, Order orderType) {
    QString select = QString("SELECT %1 FROM %2")
            .arg(fields)
            .arg(table);
    select += (where.isEmpty() ? QString() : QString(" WHERE %1").arg(where));
    if (!order.isEmpty()) {
        select += (order.isEmpty() ? QString() : QString(" ORDER BY %1 %2")
                                     .arg(order)
                                     .arg(orderType == AscOrder ? "ASC" : "DESC"));
    }
    select += ";";
    return sql::Select(select);
}

sql::Record sql::GetRecord(QString script)
{
    sql::Record res;
    sql::Table table = sql::Select(script);
    if (!table.empty()) res = table.first();
    return res;
}

QVariant sql::GetOneValue(QString script)
{
    QVariant res = QVariant();
    Table table = sql::Select(script);
    if (!table.isEmpty())
    {
        res = table.cell(0,0);
    }
    return res;
}

int sql::GetInt(QString script)
{
    return sql::GetOneValue(script).toInt();
}

long long sql::GetLong(QString script)
{
    return sql::GetOneValue(script).toLongLong();
}

double sql::GetDouble(QString script)
{
    return sql::GetOneValue(script).toDouble();
}

QString sql::GetString(QString script)
{
    return sql::GetOneValue(script).toString();
}

QByteArray sql::GetByteArray(QString script)
{
    return sql::GetOneValue(script).toByteArray();
}

bool sql::GetBool(QString script)
{
    return sql::GetOneValue(script).toBool();
}

QSqlDatabase &sql::db()
{
    return m_db;
}

QString sql::GetDatabaseName()
{
    return sql::db().databaseName();
}

QString sql::User()
{
    return sql::db().userName();
}

QString sql::Password()
{
    return sql::db().password();
}

QString sql::Host()
{
    return sql::db().hostName();
}

void sql::setDatabaseName(QString dbName) {
    sql::db().close();
    sql::db().setDatabaseName(dbName);
    sql::db().open();
}

void sql::ClearTable(QString tableName, QString schemaName)
{
    sql::Exec(QString("DELETE FROM \"%1\".\"%2\";")
              .arg(schemaName)
              .arg(tableName));
}

bool sql::isTableExists(QString tableName, QString schemaName) {
    int count = sql::GetInt(QString("SELECT COUNT(*) FROM pg_tables WHERE schemaname = '%1' AND tablename = '%2';")
                            .arg(schemaName)
                            .arg(tableName));
    return (count > 0);
}

bool sql::isViewExists(QString viewName, QString schemaName) {
    int count = sql::GetInt(QString("SELECT COUNT(*) FROM pg_views WHERE schemaname = '%1' AND viewname = '%2';")
                            .arg(schemaName)
                            .arg(viewName));
    return (count > 0);
}

bool sql::isTableEmpty(QString tableName, QString schemaName)
{
    int count = sql::GetInt(QString("SELECT COUNT(*) FROM \"%1\".\"%2\";")
                            .arg(schemaName)
                            .arg(tableName));
    return (count == 0);
}

bool sql::isFieldExists(const QString &tableName, const QString &fieldName, const QString &schemaName)
{
    return sql::GetInt(QString("SELECT COUNT(*) FROM information_schema.columns "
                               "WHERE table_schema='%1' AND table_name='%2' AND column_name='%3';")
                       .arg(schemaName)
                       .arg(tableName)
                       .arg(fieldName)) > 0;
}

void sql::insert(QString table, sql::Record record) {
    QString variable = QString();
    QString value = QString();
    for (int iValue = 0; iValue < record.count(); ++iValue) {
        if (!variable.isEmpty()) {
            variable += ",";
            value += ",";
        }
        variable += QString("\"%1\"").arg(record.at(iValue).name);
        QVariant val = record.at(iValue).value;
        if (val.type() == QVariant::Bool) {
            value += val.toBool() ? "TRUE" : "FALSE";
        }
        else {
            value += QString("'%1'").arg(val.toString());
        }
    }
    sql::Exec(QString("INSERT INTO \"%1\" (%2) VALUES (%3);")
              .arg(table)
              .arg(variable)
              .arg(value));
}

QString sql::quote(QString text) {
    return QString("'%1'").arg(text);
}

QString sql::quote2(QString text) {
    return QString("\"%1\"").arg(text);
}

QString sql::getTableName(QString tableName, QString schemaName) {
    return QString("\"%1\".\"%2\"").arg(schemaName).arg(tableName);
}

bool sql::isError()
{
    return !sql::m_lastError.isEmpty();
}

QString sql::lastError()
{
    return sql::m_lastError;
}

void sql::Delete(QString table, QString where)
{
    sql::Exec(QString("DELETE FROM %1%2;")
              .arg(table)
              .arg(where.isEmpty() ? "" : QString(" WHERE %1").arg(where)));
}

void sql::Delete(QString table, QStringList where)
{
    sql::Delete(table, where.join(" AND "));
}

void sql::Update(QString table, sql::Record record, QString where)
{
    sql::Exec(QString("UPDATE %1 SET %2%3;")
              .arg(table)
              .arg(record.getSetValueString())
              .arg(where.isEmpty() ? "" : QString(" WHERE %1").arg(where)));
}

void sql::Update(QString table, sql::Field field, QString where)
{
    sql::Exec(QString("UPDATE %1 SET %2%3;")
              .arg(table)
              .arg(field.getSetValueString())
              .arg(where.isEmpty() ? "" : QString(" WHERE %1").arg(where)));
}
