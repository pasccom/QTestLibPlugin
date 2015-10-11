#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class QFileInfo;
class QDir;
class QTextStream;
class QDebug;

namespace Utils {

class HostOsInfo {
public:
    static Qt::CaseSensitivity fileNameCaseSensitivity();
};

class FileName : private QString
{
public:
    FileName();
    explicit FileName(const QFileInfo &info);
    QFileInfo toFileInfo() const;
    static FileName fromString(const QString &filename);
    static FileName fromString(const QString &filename, const QString &defaultExtension);
    static FileName fromLatin1(const QByteArray &filename);
    static FileName fromUserInput(const QString &filename);
    static FileName fromUtf8(const char *filename, int filenameSize = -1);
    const QString &toString() const;
    QString toUserOutput() const;
    QString fileName(int pathComponents = 0) const;
    bool exists() const;

    FileName parentDir() const;

    bool operator==(const FileName &other) const;
    bool operator!=(const FileName &other) const;
    bool operator<(const FileName &other) const;
    bool operator<=(const FileName &other) const;
    bool operator>(const FileName &other) const;
    bool operator>=(const FileName &other) const;

    bool isChildOf(const FileName &s) const;
    bool isChildOf(const QDir &dir) const;
    bool endsWith(const QString &s) const;

    FileName relativeChildPath(const FileName &parent) const;
    FileName &appendPath(const QString &s);
    FileName &appendString(const QString &str);
    FileName &appendString(QChar str);

    using QString::size;
    using QString::count;
    using QString::length;
    using QString::isEmpty;
    using QString::isNull;
    using QString::clear;
private:
    FileName(const QString &string);
};

QTextStream &operator<<(QTextStream &s, const FileName &fn);

} // Utils

QDebug operator<<(QDebug dbg, const Utils::FileName &c);

#endif // FILEUTILS_H
