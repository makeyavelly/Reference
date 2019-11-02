#include "reference.h"
#include "sql/lastsql.h"


Reference::Reference(const ReferenceHeader &header, const ReferenceTable &values,
                     const ReferenceIndexFields &indexes) :
    _values(values)
{
    setHeader(header);
    appendIndex(indexes);
}

Reference *Reference::create(const QString &nameTable, const ReferenceIndexFields &indexes)
{
    Reference *reference = new Reference;
    sql::Table table = sql::Select(QString("SELECT * FROM \"%1\";").arg(nameTable));
    reference->setHeader(table.header());
    foreach (sql::Record record, table) {
        ReferenceRecord newRecord;
        for (int i = 0; i < record.count(); ++i) {
            newRecord.push_back(record.get(i).toString());
        }
        reference->_values.push_back(newRecord);
    }
    reference->appendIndex(indexes);
    return reference;
}

const ReferenceRecord *Reference::get(const QString &index, const QString &value)
{
    ReferenceIndex &refIndex = getIndex(index);
    return refIndex.value(value, nullptr);
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

Reference::Reference()
{}

void Reference::setHeader(const ReferenceHeader &header)
{
    _fields.clear();
    for (int i = 0; i < header.count(); ++i) {
        _fields.insert(header.at(i), i);
    }
}

void Reference::appendIndex(const QString &name)
{
    ReferenceIndex index;
    int iField = getField(name);
    if (iField < 0) {
        throw QString("Задан неверный индекс");
    }
    for (int i = 0; i < _values.count(); ++i) {
        ReferenceRecord &record =_values[i];
        index.insert(record.at(iField), &record);
    }
    _indexes.insert(name, index);
}

void Reference::appendIndex(const ReferenceIndexFields &indexes)
{
    for (int i = 0; i < indexes.count(); ++i) {
        appendIndex(indexes.at(i));
    }
}

int Reference::getField(const QString &name) const
{
    return _fields.value(name, -1);
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
    appendReference(name, Reference::create(name, indexes));
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

void ListReference::clear()
{
    _references.clear();
}

Reference *ListReference::getReference(const QString &name)
{
    Reference *ref = _references.value(name, nullptr);
    if (!ref) {
        throw QString("Не найден указанный справочник");
    }
    return ref;
}
