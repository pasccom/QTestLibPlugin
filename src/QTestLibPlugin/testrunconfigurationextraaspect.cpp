#include "testrunconfigurationextraaspect.h"

#include <utils/filenamevalidatinglineedit.h>

#include <QtWidgets>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigWidget::TestRunConfigWidget(TestRunConfigurationExtraAspect* aspect) :
    ProjectExplorer::RunConfigWidget(), mAspect(aspect)
{
    mFormatCombo = new QComboBox(this);
    mFormatLabel = new QLabel(tr("Test output format:"), this);
    mFormatLabel->setBuddy(mFormatCombo);
    mVerbosityCombo = new QComboBox(this);
    mVerbosityLabel = new QLabel(tr("Test output verbosity:"), this);
    mVerbosityLabel->setBuddy(mVerbosityCombo);
    mOutFileEdit = new Utils::FileNameValidatingLineEdit(this);
    mOutFileEdit->setAllowDirectories(false);
    mOutFileLabel = new QLabel(tr("Test output file:"), this);
    mOutFileLabel->setBuddy(mOutFileEdit);
    mOutFileButton =  new QPushButton(tr("Browse..."), this);
    mWarningCheck = new QCheckBox(tr("Maximum number of messages"), this);
    mWarningSpin = new QSpinBox(this);
    mWarningSpin->setEnabled(false);
    mEventDelayCheck = new QCheckBox(tr("Event simulation delay"), this);
    mEventDelaySpin = new QSpinBox(this);
    mEventDelaySpin->setEnabled(false);
    mKeyDelayCheck = new QCheckBox(tr("Keyboard simulation delay"),this);
    mKeyDelaySpin = new QSpinBox(this);
    mKeyDelaySpin->setEnabled(false);
    mMouseDelayCheck = new QCheckBox(tr("Mouse simulation delay"),this);
    mMouseDelaySpin = new QSpinBox(this);
    mMouseDelaySpin->setEnabled(false);

    QBoxLayout* outFileLayout = new QHBoxLayout;
    outFileLayout->setContentsMargins(0, 0, 0, 0);
    outFileLayout->addWidget(mOutFileLabel, 0, Qt::AlignLeft);
    outFileLayout->addWidget(mOutFileEdit, 1);
    outFileLayout->addWidget(mOutFileButton, 0, Qt::AlignCenter);

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mFormatLabel, 0, 0, Qt::AlignLeft);
    mainLayout->addWidget(mFormatCombo, 0, 1, Qt::AlignRight);
    mainLayout->addWidget(mVerbosityLabel, 0, 3, Qt::AlignLeft);
    mainLayout->addWidget(mVerbosityCombo, 0, 4, Qt::AlignRight);
    mainLayout->addLayout(outFileLayout, 1, 0, 1, 5);
    mainLayout->addWidget(mWarningCheck, 2, 0, Qt::AlignLeft);
    mainLayout->addWidget(mWarningSpin, 2, 1, Qt::AlignRight);
    mainLayout->addWidget(mEventDelayCheck, 2, 3, Qt::AlignLeft);
    mainLayout->addWidget(mEventDelaySpin, 2, 4, Qt::AlignRight);
    mainLayout->addWidget(mKeyDelayCheck, 3, 0, Qt::AlignLeft);
    mainLayout->addWidget(mKeyDelaySpin, 3, 1, Qt::AlignRight);
    mainLayout->addWidget(mMouseDelayCheck, 3, 3, Qt::AlignLeft);
    mainLayout->addWidget(mMouseDelaySpin, 3, 4, Qt::AlignRight);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 1);
    mainLayout->setColumnStretch(3, 1);

    setLayout(mainLayout);

    connect(mWarningCheck, SIGNAL(toggled(bool)),
            mWarningSpin, SLOT(setEnabled(bool)));
    connect(mEventDelayCheck, SIGNAL(toggled(bool)),
            mEventDelaySpin, SLOT(setEnabled(bool)));
    connect(mKeyDelayCheck, SIGNAL(toggled(bool)),
            mKeyDelaySpin, SLOT(setEnabled(bool)));
    connect(mMouseDelayCheck, SIGNAL(toggled(bool)),
            mMouseDelaySpin, SLOT(setEnabled(bool)));
}

TestRunConfigurationExtraAspect::TestRunConfigurationExtraAspect(ProjectExplorer::RunConfiguration* parent) :
  ProjectExplorer::IRunConfigurationAspect(parent)
{
    setId(Core::Id());
    setDisplayName(tr("Test settings"));
}

TestRunConfigurationExtraAspect::~TestRunConfigurationExtraAspect()
{

}

} // Internal
} // QTestLibPlugin
