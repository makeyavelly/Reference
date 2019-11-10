#ifndef REFERENCE_H
#define REFERENCE_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QHash>


typedef QVector<QString> ReferenceRecord;
typedef QStringList ReferenceHeader;
typedef QVector<ReferenceRecord> ReferenceTable;
typedef QStringList ReferenceIndexFields;
typedef QHash<QString, QVector<ReferenceRecord*> > ReferenceIndex;


class Reference
{
    QString _script;
    QHash<QString, int> _fields;
    ReferenceTable _values;
    QHash<QString, ReferenceIndex> _indexes;

public:
    Reference(const ReferenceHeader &header, const ReferenceTable &values,
              const ReferenceIndexFields &indexes);
    static Reference *create(const QString &script, const ReferenceIndexFields &indexes);

    const QVector<ReferenceRecord*> getAll(const QString &index, const QString &value);
    QVector<QString> getAll(const QString &index, const QString &value, const QString &fieldName);

    const ReferenceRecord *get(const QString &index, const QString &value);
    QString get(const QString &index, const QString &value, const QString &fieldName);

    void reload();

private:
    Reference();

    void setHeader(const ReferenceHeader &header);
    void loadData();

    void appendIndex(const QString &name);
    void appendIndex(const ReferenceIndexFields &indexes);
    void updateIndex();
    void updateIndex(const QString &key, ReferenceIndex &index);

    int getField(const QString &name) const;
    QVector<int> getFields(const QStringList &names) const;
    ReferenceIndex &getIndex(const QString &name);
};


class ListReference
{
    QHash<QString, Reference*> _references;
public:
    ListReference();

    void appendReference(const QString &name, Reference *reference);
    void appendReference(const QString &name, const ReferenceIndexFields &indexes);
    void appendReference(const QString &name, const ReferenceIndexFields &indexes, const QString &script);

    const ReferenceRecord *get(const QString &reference, const QString &index,
                               const QString &value);
    QString get(const QString &reference, const QString &index,
                const QString &value, const QString &fieldName);

    const QVector<ReferenceRecord*> getAll(const QString &reference, const QString &index, const QString &value);
    QVector<QString> getAll(const QString &reference, const QString &index, const QString &value, const QString &fieldName);

    void clear();

    void reload(const QString &nameTable);
    void reload();

private:
    Reference *getReference(const QString &name);
};


#endif // REFERENCE_H
