/* Copyright 2015 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QTestLibPlugin.
 * 
 * QTestLibPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QTestLibPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QTestLibPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#ifndef RUNCONFIGURATION_H
#define RUNCONFIGURATION_H

#include <qtcreatorfake_global.h>

#include <QWidget>
#include <QVariant>
#include <QLinkedList>
#include <QProcess>

#include <utils/outputformat.h>
#include "target.h"

namespace ProjectExplorer {

class IRunConfigurationAspect;
    
class RunConfiguration : public QObject
{
    Q_OBJECT
public:
    inline RunConfiguration(ProjectExplorer::Target *target) :
        QObject(target), mTarget(target) {}
    inline virtual QString displayName(void) const {return QString::null;}
    inline Target *target(void) const {return mTarget;}
    
    inline void addExtraAspect(IRunConfigurationAspect *aspect) {mAspects.append(aspect);}
    template <typename T> T *extraAspect() const
    {
        foreach (IRunConfigurationAspect *aspect, mAspects)
            if (T *result = qobject_cast<T *>(aspect))
                return result;
        return NULL;
    }
    
    inline void setDefaultDisplayName(const QString& name) {mDefaultDisplayName = name;}
    inline QString defaultDisplayName(void) const {return mDefaultDisplayName;}
    
    inline bool fromMap(const QVariantMap& map) {Q_UNUSED(map); return true;}
    inline QVariantMap toMap(void) const {return QVariantMap();}
    
private:
    QString mDefaultDisplayName;
    Target *mTarget;
    QLinkedList<IRunConfigurationAspect*> mAspects;
};

class IRunConfigurationAspect : public QObject
{
    Q_OBJECT
public:
    inline IRunConfigurationAspect(RunConfiguration* parent) :
        QObject(parent) {}

    inline void setId(const Core::Id& id) {mId = id;}
    inline Core::Id id(void) const {return mId;}

    inline void setDisplayName(const QString& name) {mDisplayName = name;}
    inline QString displayName(void) const {return mDisplayName;}
private:
    Core::Id mId;
    QString mDisplayName;
};

class RunControl : public QObject
{
    Q_OBJECT
public:
    RunControl(RunConfiguration *runConfig, const QString& name = QString::null, QObject *parent = NULL);
    inline QString displayName(void) const {return mName;}
    inline RunConfiguration* runConfiguration() const {return mConfig;}

    inline Utils::OutputFormat outputFormat(void) const {return mOutputFormat;}
    inline Utils::OutputFormat errorFormat(void) const {return mErrorFormat;}
    inline void setOutputFormat(Utils::OutputFormat format) {mOutputFormat = format;}
    inline void setErrorFormat(Utils::OutputFormat format) {mErrorFormat = format;}
    inline void setFormats(Utils::OutputFormat outputFormat, Utils::OutputFormat errorFormat) {mOutputFormat = outputFormat; mErrorFormat = errorFormat;}
signals:
    void appendMessage(ProjectExplorer::RunControl *runControl, const QString& msg, Utils::OutputFormat format);
    void finished(void);
    void started(void);
public slots:
    virtual void start(void) = 0;
private:
    QProcess* mTestProc;
    Utils::OutputFormat mOutputFormat;
    Utils::OutputFormat mErrorFormat;
    RunConfiguration* mConfig;
    QString mName;
};

class RunConfigWidget : public QWidget
{
    Q_OBJECT
public:
    RunConfigWidget()
        : QWidget(0)
    {}

    virtual QString displayName() const = 0;

signals:
    void displayNameChanged(const QString &);
};


} // ProjectExplorer

#endif // RUNCONFIGURATION_H
