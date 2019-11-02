#include "Reference.h"


Reference::Reference(const ReferenceHeader &names, const ReferenceTable &values,
                     const ReferenceIndexFields &indexes) :
    _values(values)
{
    for (int i = 0; i < names.count(); ++i) {
        _fields.insert(names.at(i), i);
    }
    foreach (const QString &index, indexes) {
        appendIndex(index);
    }
}

Reference *Reference::create(const QString &nameTable, const ReferenceIndexFields &indexes)
{
    Reference *reference = new Reference;
    // TODO-AFTER Загрузка справочника из БД
    Q_UNUSED(nameTable);
    Q_UNUSED(indexes);
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

Reference *ListReference::getReference(const QString &name)
{
    Reference *ref = _references.value(name, nullptr);
    if (!ref) {
        throw QString("Не найден указанный справочник");
    }
    return ref;
}
