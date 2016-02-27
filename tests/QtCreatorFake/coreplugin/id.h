#ifndef CORE_ID_H
#define CORE_ID_H

//#include "core_global.h"

#include <QMetaType>
#include <QString>

QT_BEGIN_NAMESPACE
class QDataStream;
class QVariant;
QT_END_NAMESPACE

namespace Core {

class Id
{
public:
    enum { IdsPerPlugin = 10000, ReservedPlugins = 1000 };

    Id() : m_id(0) {}
    Id(const char *name);

    Id withSuffix(int suffix) const;
    Id withSuffix(const char *suffix) const;
    Id withSuffix(const QString &suffix) const;
    Id withPrefix(const char *prefix) const;

    QByteArray name() const;
    QString toString() const; // Avoid.
    QVariant toSetting() const; // Good to use.
    QString suffixAfter(Id baseId) const;
    bool isValid() const { return m_id; }
    bool operator==(Id id) const { return m_id == id.m_id; }
    bool operator==(const char *name) const;
    bool operator!=(Id id) const { return m_id != id.m_id; }
    bool operator!=(const char *name) const { return !operator==(name); }
    bool operator<(Id id) const { return m_id < id.m_id; }
    bool operator>(Id id) const { return m_id > id.m_id; }
    bool alphabeticallyBefore(Id other) const;
    int uniqueIdentifier() const { return m_id; }
    static Id fromUniqueIdentifier(int uid) { return Id(uid); }
    static Id fromString(const QString &str); // FIXME: avoid.
    static Id fromName(const QByteArray &ba); // FIXME: avoid.
    static Id fromSetting(const QVariant &variant); // Good to use.

private:
    // Intentionally unimplemented
    Id(const QLatin1String &);
    // Force explicit use of fromUniqueIdentifier().
    explicit Id(int uid) : m_id(uid) {}

    int m_id;
};

inline uint qHash(Id id) { return id.uniqueIdentifier(); }

} // namespace Core

Q_DECLARE_METATYPE(Core::Id)
Q_DECLARE_METATYPE(QList<Core::Id>)

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &ds, Core::Id id);
QDataStream &operator>>(QDataStream &ds, Core::Id &id);
QDebug operator<<(QDebug dbg, const Core::Id &id);
QT_END_NAMESPACE

#endif // CORE_ID_H
