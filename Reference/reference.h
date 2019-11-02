#ifndef REFERENCE_H
#define REFERENCE_H

#include <QString>
#include <QVector>
#include <QHash>


typedef QVector<QString> ReferenceRecord;
typedef QVector<QString> ReferenceHeader;
typedef QVector<ReferenceRecord> ReferenceTable;
typedef QVector<QString> ReferenceIndexFields;
typedef QHash<QString, ReferenceRecord*> ReferenceIndex;


class Reference
{
    QHash<QString, int> _fields;
    ReferenceTable _values;
    QHash<QString, ReferenceIndex> _indexes;

public:
    Reference(const ReferenceHeader &names, const ReferenceTable &values,
              const ReferenceIndexFields &indexes);
    static Reference *create(const QString &nameTable, const ReferenceIndexFields &indexes);

    const ReferenceRecord *get(const QString &index, const QString &value);
    QString get(const QString &index, const QString &value, const QString &fieldName);

private:
    Reference();

    void appendIndex(const QString &name);

    int getField(const QString &name) const;
    ReferenceIndex &getIndex(const QString &name);
};


class ListReference
{
    QHash<QString, Reference*> _references;
public:
    ListReference();

    void appendReference(const QString &name, Reference *reference);
    void appendReference(const QString &name, const ReferenceIndexFields &indexes);

    const ReferenceRecord *get(const QString &reference, const QString &index,
                               const QString &value);
    QString get(const QString &reference, const QString &index,
                const QString &value, const QString &fieldName);

private:
    Reference *getReference(const QString &name);
};


#endif // REFERENCE_H
