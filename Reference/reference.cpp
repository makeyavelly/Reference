#include "reference.h"
#include "sql/lastsql.h"


Reference::Reference(const ReferenceHeader &header, const ReferenceTable &values,
                     const ReferenceIndexFields &indexes) :
    _values(values)
{
    setHeader(header);
    appendIndex(indexes);
}

Reference *Reference::create(const QString &script, const ReferenceIndexFields &indexes)
{
    Reference *reference = new Reference;
    reference->_script = script;
    reference->loadData();
    reference->appendIndex(indexes);
    return reference;
}

const QVector<ReferenceRecord *> Reference::getAll(const QString &index, const QString &value)
{
    ReferenceIndex &refIndex = getIndex(index);
    return refIndex.value(value, QVector<ReferenceRecord*>());
}

QVector<QString> Reference::getAll(const QString &index, const QString &value, const QString &fieldName)
{
    QVector<QString> res;
    const QVector<ReferenceRecord*> records = getAll(index, value);
    int iField = getField(fieldName);
    if (iField >= 0) {
        for (const ReferenceRecord *record : records) {
            res.push_back(record->at(iField));
        }
    }
    return res;
}

const ReferenceRecord *Reference::get(const QString &index, const QString &value)
{
    QVector<ReferenceRecord*> records = getAll(index, value);
    return records.isEmpty() ? nullptr : records.first();
}

QString Reference::get(const QString &index, const QString &value, const QString &fieldName)
{
    const ReferenceRecord *record = get(index, value);
    int iField = getField(fieldName);
    if (!record || iField < 0) {
        return QString();
    }
    return record->at(iField);
}

void Reference::reload()
{
    loadData();
    updateIndex();
}

Reference::Reference()
{}

void Reference::setHeader(const ReferenceHeader &header)
{
    _fields.clear();
    for (int i = 0; i < header.count(); ++i) {
        _fields.insert(header.at(i), i);
    }
}

void Reference::loadData()
{
    sql::Table table = sql::Select(_script);
    setHeader(table.header());
    _values.reserve(table.countRow());
    for (sql::Record record : table) {
        ReferenceRecord newRecord;
        newRecord.reserve(record.count());
        for (int i = 0; i < record.count(); ++i) {
            newRecord.push_back(record.get(i).toString());
        }
        _values.push_back(newRecord);
    }
}

void Reference::appendIndex(const QString &name)
{
    ReferenceIndex index;
    updateIndex(name, index);
    _indexes.insert(name, index);
}

void Reference::appendIndex(const ReferenceIndexFields &indexes)
{
    for (int i = 0; i < indexes.count(); ++i) {
        appendIndex(indexes.at(i));
    }
}

void Reference::updateIndex()
{
    for (const QString &key : _indexes.keys()) {
        updateIndex(key, _indexes[key]);
    }
}

void Reference::updateIndex(const QString &key, ReferenceIndex &index)
{
    index.clear();
    const QStringList names = key.split(",");
    const QVector<int> fields = getFields(names);
    if (fields.isEmpty()) {
        throw QString("Задан неверный индекс");
    }
    for (int i = 0; i < _values.count(); ++i) {
        ReferenceRecord &record =_values[i];
        QStringList keys;
        for (int i = 0; i < fields.count(); ++i) {
            keys.push_back(record.at(fields.at(i)));
        }
        QString key = keys.join(",");
        if (index.contains(key)) {
            index[key].push_back(&record);
        } else {
            QVector<ReferenceRecord*> values;
            values.push_back(&record);
            index.insert(key, values);
        }
    }
}

int Reference::getField(const QString &name) const
{
    return _fields.value(name, -1);
}

QVector<int> Reference::getFields(const QStringList &names) const
{
    QVector<int> res;
    for (int i = 0; i < names.count(); ++i) {
        res.push_back(getField(names.at(i)));
    }
    return res;
}

ReferenceIndex &Reference::getIndex(const QString &name)
{
    if (!_indexes.contains(name)) {
        throw QString("Неверный индекс запроса");
    }
    return _indexes[name];
}



ListReference::ListReference()
{}

void ListReference::appendReference(const QString &name, Reference *reference)
{
    _references.insert(name, reference);
}

void ListReference::appendReference(const QString &name, const ReferenceIndexFields &indexes)
{
    appendReference(name, indexes, QString("SELECT * FROM \"%1\";").arg(name));
}

void ListReference::appendReference(const QString &name, const ReferenceIndexFields &indexes, const QString &script)
{
    appendReference(name, Reference::create(script, indexes));
}

const ReferenceRecord *ListReference::get(const QString &reference, const QString &index,
                                          const QString &value)
{
    return getReference(reference)->get(index, value);
}

QString ListReference::get(const QString &reference, const QString &index,
                           const QString &value, const QString &fieldName)
{
    return getReference(reference)->get(index, value, fieldName);
}

const QVector<ReferenceRecord *> ListReference::getAll(const QString &reference, const QString &index, const QString &value)
{
    return getReference(reference)->getAll(index, value);
}

QVector<QString> ListReference::getAll(const QString &reference, const QString &index, const QString &value, const QString &fieldName)
{
    return getReference(reference)->getAll(index, value, fieldName);
}

void ListReference::clear()
{
    _references.clear();
}

void ListReference::reload(const QString &nameTable)
{
    getReference(nameTable)->reload();
}

void ListReference::reload()
{
    foreach (const QString &key, _references.keys()) {
        reload(key);
    }
}

Reference *ListReference::getReference(const QString &name)
{
    Reference *ref = _references.value(name, nullptr);
    if (!ref) {
        throw QString("Не найден указанный справочник");
    }
    return ref;
}
