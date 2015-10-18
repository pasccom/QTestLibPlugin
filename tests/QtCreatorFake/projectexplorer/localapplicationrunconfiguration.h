#ifndef LOCALAPPLICATIONRUNCONFIGURATION_H
#define LOCALAPPLICATIONRUNCONFIGURATION_H

#include "runconfiguration.h"

namespace ProjectExplorer {

class LocalApplicationRunConfiguration : public RunConfiguration
{
    Q_OBJECT
public:
    inline LocalApplicationRunConfiguration(ProjectExplorer::Target *target) :
        RunConfiguration(target) {}

    inline QString executable() const {return mExe;}
    inline void setExecutable(const QString& exe) {mExe = exe;}
    inline QString workingDirectory() const {return mDir;}
    inline void setWorkingDirectory(const QString& dir) {mDir = dir;}
    inline QString commandLineArguments() const {return mArgs;}
    inline void setCommandLineArguments(const QString& args) {mArgs = args;}

    inline QString displayName(void) const {return mExe.section('.', 0, 0, QString::SectionSkipEmpty);}
private:
    QString mExe;
    QString mArgs;
    QString mDir;
};

} // ProjectExplorer

#endif // LOCALAPPLICATIONRUNCONFIGURATION_H
