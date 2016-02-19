#include "filetypevalidatinglineedit.h"

#include <QtCore>

namespace QTestLibPlugin {
namespace Utils {

FileTypeValidatingLineEdit::FileTypeValidatingLineEdit(QWidget* parent) :
    QtcUtils::FancyLineEdit(parent), mAccepted(0)
{
    mAccepted |= AcceptsFiles;
    mAccepted |= RequireReadable;
}

bool FileTypeValidatingLineEdit::validate(const QString& value, QString *errorMessage) const
{
    if ((mAccepted & AcceptEmpty) && value.isEmpty())
        return true;

    if ((mAccepted & AcceptNew) && !QFileInfo::exists(value))
        return validateName(value, errorMessage);

    if (!QFileInfo::exists(value)) {
        if (errorMessage != NULL)
            *errorMessage = tr("File \"%1\" does not exist").arg(value);
        return false;
    }

    return (validateName(value, errorMessage)
         && validateType(value, errorMessage)
         && validatePermissions(value, errorMessage));
}

void FileTypeValidatingLineEdit::manageAcceptFlags(Accept flag, bool enable)
{
    if (enable)
        mAccepted |= flag;
    else
        mAccepted &= ~Accepts(flag);
}

bool FileTypeValidatingLineEdit::validateName(const QString& value, QString* errorMessage) const
{
    if (mRequiredExtensions.isEmpty())
        return true;

    foreach (QString ext, mRequiredExtensions) {
        if (value.endsWith(QLatin1Char('.') + ext))
            return true;
    }

    if (errorMessage != NULL)
        *errorMessage = tr("File does not have one of the required extensions.");
    return false;
}

bool FileTypeValidatingLineEdit::validateType(const QString& value, QString* errorMessage) const
{
    QFileInfo info(value);

    if (info.isFile() && !(mAccepted & AcceptsFiles)) {
        if (errorMessage != NULL)
            *errorMessage = tr("Regular files are not accepted");
        return false;
    }

    if (info.isDir() && !(mAccepted & AcceptsDirectories)) {
        if (errorMessage != NULL)
            *errorMessage = tr("Directories are not accepted");
        return false;
    }

    return true;
}

bool FileTypeValidatingLineEdit::validatePermissions(const QString& value, QString* errorMessage) const
{
    QFileInfo info(value);

    if ((mAccepted & RequireReadable) && !info.isReadable()) {
        if (errorMessage != NULL)
            *errorMessage = tr("File must be readable");
        return false;
    }
    if ((mAccepted & RequireWritable) && !info.isWritable()) {
        if (errorMessage != NULL)
            *errorMessage = tr("File must be writable");
        return false;
    }
    if ((mAccepted & RequireExecutable) && !info.isExecutable()) {
        if (errorMessage != NULL)
            *errorMessage = tr("File must be executable");
        return false;
    }

    return true;
}

} // Utils
} // QTestLibPlugin
