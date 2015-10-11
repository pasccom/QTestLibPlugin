#include "fileutils.h"

#include <QtCore>
#include <QtDebug>

QDebug operator<<(QDebug dbg, const Utils::FileName &c)
{
    return dbg << c.toString();
}

namespace Utils {

Qt::CaseSensitivity HostOsInfo::fileNameCaseSensitivity()
{
#ifdef Q_OS_UNIX
    return Qt::CaseSensitive;
#else
    return Qt::CaseInsensitive;
#endif
}

FileName::FileName()
    : QString()
{}

FileName::FileName(const QFileInfo &info)
    : QString(info.absoluteFilePath())
{}

QFileInfo FileName::toFileInfo() const
{
    return QFileInfo(*this);
}

const QString &FileName::toString() const
{
    return *this;
}

QString FileName::toUserOutput() const
{
    return QDir::toNativeSeparators(toString());
}

QString FileName::fileName(int pathComponents) const
{
    if (pathComponents < 0)
        return *this;
    const QChar slash = QLatin1Char('/');
    Q_ASSERT(!endsWith(slash));
    int i = lastIndexOf(slash);
    if (pathComponents == 0 || i == -1)
        return mid(i + 1);
    int component = i + 1;
    // skip adjacent slashes
    while (i > 0 && at(--i) == slash);
    while (i >= 0 && --pathComponents >= 0) {
        i = lastIndexOf(slash, i);
        component = i + 1;
        while (i > 0 && at(--i) == slash);
    }

    // If there are no more slashes before the found one, return the entire string
    if (i > 0 && lastIndexOf(slash, i) != -1)
        return mid(component);
    return *this;
}

bool FileName::exists() const
{
    return QFileInfo::exists(*this);
}

FileName FileName::parentDir() const
{
    const QString basePath = toString();
    if (basePath.isEmpty())
        return FileName();

    const QDir base(basePath);
    if (base.isRoot())
        return FileName();

    const QString path = basePath + QLatin1String("/..");
    const QString parent = QDir::cleanPath(path);

    return FileName::fromString(parent);
}

FileName FileName::fromString(const QString &filename)
{
    return FileName(filename);
}

FileName FileName::fromString(const QString &filename, const QString &defaultExtension)
{
    if (filename.isEmpty() || defaultExtension.isEmpty())
        return filename;

    QString rc = filename;
    QFileInfo fi(filename);
    // Add extension unless user specified something else
    const QChar dot = QLatin1Char('.');
    if (!fi.fileName().contains(dot)) {
        if (!defaultExtension.startsWith(dot))
            rc += dot;
        rc += defaultExtension;
    }
    return rc;
}

FileName FileName::fromLatin1(const QByteArray &filename)
{
    return FileName(QString::fromLatin1(filename));
}

FileName FileName::fromUserInput(const QString &filename)
{
    QString clean = QDir::cleanPath(filename);
    if (clean.startsWith(QLatin1String("~/")))
        clean = QDir::homePath() + clean.mid(1);
    return FileName(clean);
}

FileName FileName::fromUtf8(const char *filename, int filenameSize)
{
    return FileName(QString::fromUtf8(filename, filenameSize));
}

FileName::FileName(const QString &string)
    : QString(string)
{}

bool FileName::operator==(const FileName &other) const
{
    return QString::compare(*this, other, HostOsInfo::fileNameCaseSensitivity()) == 0;
}

bool FileName::operator!=(const FileName &other) const
{
    return !(*this == other);
}

bool FileName::operator<(const FileName &other) const
{
    return QString::compare(*this, other, HostOsInfo::fileNameCaseSensitivity()) < 0;
}

bool FileName::operator<=(const FileName &other) const
{
    return QString::compare(*this, other, HostOsInfo::fileNameCaseSensitivity()) <= 0;
}

bool FileName::operator>(const FileName &other) const
{
    return other < *this;
}

bool FileName::operator>=(const FileName &other) const
{
    return other <= *this;
}

bool FileName::isChildOf(const FileName &s) const
{
    if (s.isEmpty())
        return false;
    if (!QString::startsWith(s, HostOsInfo::fileNameCaseSensitivity()))
        return false;
    if (size() <= s.size())
        return false;
    // s is root, '/' was already tested in startsWith
    if (s.QString::endsWith(QLatin1Char('/')))
        return true;
    // s is a directory, next character should be '/' (/tmpdir is NOT a child of /tmp)
    return at(s.size()) == QLatin1Char('/');
}

bool FileName::isChildOf(const QDir &dir) const
{
    return isChildOf(FileName::fromString(dir.absolutePath()));
}

bool FileName::endsWith(const QString &s) const
{
    return QString::endsWith(s, HostOsInfo::fileNameCaseSensitivity());
}

FileName FileName::relativeChildPath(const FileName &parent) const
{
    if (!isChildOf(parent))
        return FileName();
    return FileName(QString::mid(parent.size() + 1, -1));
}

FileName &FileName::appendPath(const QString &s)
{
    if (s.isEmpty())
        return *this;
    if (!isEmpty() && !QString::endsWith(QLatin1Char('/')))
        appendString(QLatin1Char('/'));
    appendString(s);
    return *this;
}

FileName &FileName::appendString(const QString &str)
{
    QString::append(str);
    return *this;
}

FileName &FileName::appendString(QChar str)
{
    QString::append(str);
    return *this;
}

QTextStream &operator<<(QTextStream &s, const FileName &fn)
{
    return s << fn.toString();
}

} // Utils
