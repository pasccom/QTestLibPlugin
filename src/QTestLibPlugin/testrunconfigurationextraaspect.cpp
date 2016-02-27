#include "testrunconfigurationextraaspect.h"
#include "qtestlibpluginconstants.h"

#include "Widgets/filetypevalidatinglineedit.h"

#include <QtWidgets>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigWidget::TestRunConfigWidget(TestRunConfigurationExtraAspect* aspect) :
    ProjectExplorer::RunConfigWidget(), mAspect(aspect)
{
    QVariantMap map;
    mAspect->mTestArgsParser->toMap(map);

    mFormatCombo = new QComboBox(this);
    mFormatCombo->addItem(tr("Text format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::TxtFormat));
    //mFormatCombo->addItem(tr("CSV format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::CsvFormat));
    mFormatCombo->addItem(tr("XML format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::XmlFormat));
    mFormatCombo->addItem(tr("Light XML format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::LightXmlFormat));
    mFormatCombo->addItem(tr("XUnit XML format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::XUnitXmlFormat));
    mFormatCombo->setCurrentIndex(mFormatCombo->findData(QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(mAspect->mTestArgsParser->outputFormat())));
    mFormatLabel = new QLabel(tr("Test output format:"), this);
    mFormatLabel->setBuddy(mFormatCombo);

    mVerbosityCombo = new QComboBox(this);
    mVerbosityCombo->addItem(tr("Silent"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::Silent));
    mVerbosityCombo->addItem(tr("Normal"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::NormalVerbosity));
    mVerbosityCombo->addItem(tr("Verbose"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::Verbose1));
    mVerbosityCombo->addItem(tr("Very verbose"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::Verbose2));
    mVerbosityCombo->addItem(tr("Signals"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::VerboseSignal));
    //mVerbosityCombo->addItem(tr("Silent"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::VerboseBenchmark));
    mVerbosityCombo->setCurrentIndex(mVerbosityCombo->findData(QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(mAspect->mTestArgsParser->verbosity())));
    mVerbosityLabel = new QLabel(tr("Test output verbosity:"), this);
    mVerbosityLabel->setBuddy(mVerbosityCombo);

    mOutFileEdit = new Widgets::FileTypeValidatingLineEdit(this);
    mOutFileEdit->setAcceptNew(true);
    mOutFileEdit->setRequireWritable(true);
    mOutFileEdit->setText(map.value(Constants::OutputFileKey).toString());
    mOutFileLabel = new QLabel(tr("Test output file:"), this);
    mOutFileLabel->setBuddy(mOutFileEdit);
    mOutFileButton =  new QPushButton(tr("Browse..."), this);

    mWarningCheck = new QCheckBox(tr("Maximum number of messages"), this);
    mWarningSpin = new QSpinBox(this);
    mWarningSpin->setMinimum(0);
    mWarningSpin->setMaximum(INT_MAX);
    mWarningSpin->setSingleStep(100);
    mWarningSpin->setValue(mAspect->mTestArgsParser->maxWarnings());
    if (map.contains(Constants::MaxWarningKey))
        mWarningCheck->setChecked(true);
    else
        mWarningSpin->setEnabled(false);

    mEventDelayCheck = new QCheckBox(tr("Event simulation delay"), this);
    mEventDelaySpin = new QSpinBox(this);
    mEventDelaySpin->setMinimum(0);
    mEventDelaySpin->setMaximum(10000);
    mEventDelaySpin->setSingleStep(10);
    mEventDelaySpin->setSuffix(tr(" ms"));
    mEventDelaySpin->setValue(mAspect->mTestArgsParser->eventDelay());
    if (map.contains(Constants::EventDelayKey))
        mEventDelayCheck->setChecked(true);
    else
        mEventDelaySpin->setEnabled(false);

    mKeyDelayCheck = new QCheckBox(tr("Keyboard simulation delay"),this);
    mKeyDelaySpin = new QSpinBox(this);
    mKeyDelaySpin->setMinimum(0);
    mKeyDelaySpin->setMaximum(10000);
    mKeyDelaySpin->setSingleStep(10);
    mKeyDelaySpin->setSuffix(tr(" ms"));
    mKeyDelaySpin->setValue(mAspect->mTestArgsParser->keyDelay());
    if (map.contains(Constants::KeyDelayKey))
        mKeyDelayCheck->setChecked(true);
    else
        mKeyDelaySpin->setEnabled(false);

    mMouseDelayCheck = new QCheckBox(tr("Mouse simulation delay"),this);
    mMouseDelaySpin = new QSpinBox(this);
    mMouseDelaySpin->setMinimum(0);
    mMouseDelaySpin->setMaximum(10000);
    mMouseDelaySpin->setSingleStep(10);
    mMouseDelaySpin->setSuffix(tr(" ms"));
    mMouseDelaySpin->setValue(mAspect->mTestArgsParser->mouseDelay());
    if (map.contains(Constants::MouseDelayKey))
        mMouseDelayCheck->setChecked(true);
    else
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

    connect(mFormatCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFormat(int)));
    connect(mVerbosityCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateVerbosity(int)));

    connect(mOutFileEdit, SIGNAL(validChanged(bool)),
            this, SLOT(updateOutFile(bool)));
    connect(mOutFileEdit, SIGNAL(editingFinished()),
            this, SLOT(updateOutFile()));
    connect(mOutFileButton, SIGNAL(released()),
            this, SLOT(browseOutFile()));

    connect(mWarningCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateMaxWarnings(bool)));
    connect(mEventDelayCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateEventDelay(bool)));
    connect(mKeyDelayCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateKeyDelay(bool)));
    connect(mMouseDelayCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateMouseDelay(bool)));
    connect(mWarningSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateMaxWarnings(int)));
    connect(mEventDelaySpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateEventDelay(int)));
    connect(mKeyDelaySpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateKeyDelay(int)));
    connect(mMouseDelaySpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateMouseDelay(int)));
}

void TestRunConfigWidget::updateFormat(int index)
{
    QTC_ASSERT(mFormatCombo->itemData(index).canConvert<QTestLibArgsParser::TestOutputFormat>(), return);
    mAspect->mTestArgsParser->setOutputFormat(mFormatCombo->itemData(index).value<QTestLibArgsParser::TestOutputFormat>());
}

void TestRunConfigWidget::updateVerbosity(int index)
{
    QTC_ASSERT(mVerbosityCombo->itemData(index).canConvert<QTestLibArgsParser::TestVerbosity>(), return);
    mAspect->mTestArgsParser->setVerbosity(mVerbosityCombo->itemData(index).value<QTestLibArgsParser::TestVerbosity>());
}

void TestRunConfigWidget::updateOutFile(bool valid)
{
    if (valid)
        mAspect->mTestArgsParser->setOutFileName(Utils::FileName::fromString(mOutFileEdit->text()));
}

void TestRunConfigWidget::updateOutFile(void)
{
    if (mOutFileEdit->isValid())
        mAspect->mTestArgsParser->setOutFileName(Utils::FileName::fromString(mOutFileEdit->text()));
    else
        mOutFileEdit->setText(mAspect->mTestArgsParser->outFileName().toString());
}

void TestRunConfigWidget::browseOutFile(void)
{
    QString outFile = QFileDialog::getSaveFileName(this, tr("Choose test output file"), mAspect->mTestArgsParser->outFileName().toString());
    if (!outFile.isNull())
        mAspect->mTestArgsParser->setOutFileName(Utils::FileName::fromString(outFile));
    updateOutFile();
}

void TestRunConfigWidget::updateMaxWarnings(int value)
{
    mAspect->mTestArgsParser->setMaxWarnings(value);
}

void TestRunConfigWidget::updateMaxWarnings(bool enabled)
{
    mWarningSpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setMaxWarnings(mWarningSpin->value());
    else
        mAspect->mTestArgsParser->setMaxWarnings();
}

void TestRunConfigWidget::updateEventDelay(int value)
{
    mAspect->mTestArgsParser->setEventDelay(value);
}

void TestRunConfigWidget::updateEventDelay(bool enabled)
{
    mEventDelaySpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setEventDelay(mEventDelaySpin->value());
    else
        mAspect->mTestArgsParser->setEventDelay();
}

void TestRunConfigWidget::updateKeyDelay(int value)
{
    mAspect->mTestArgsParser->setKeyDelay(value);
}

void TestRunConfigWidget::updateKeyDelay(bool enabled)
{
    mKeyDelaySpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setKeyDelay(mKeyDelaySpin->value());
    else
        mAspect->mTestArgsParser->setKeyDelay();
}

void TestRunConfigWidget::updateMouseDelay(int value)
{
    mAspect->mTestArgsParser->setMouseDelay(value);
}

void TestRunConfigWidget::updateMouseDelay(bool enabled)
{
    mMouseDelaySpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setMouseDelay(mMouseDelaySpin->value());
    else
        mAspect->mTestArgsParser->setMouseDelay();
}

TestRunConfigurationExtraAspect::TestRunConfigurationExtraAspect(ProjectExplorer::RunConfiguration* parent) :
  ProjectExplorer::IRunConfigurationAspect(parent)
{
    mTestArgsParser = new QTestLibArgsParser;

    setId(Core::Id(Constants::TestRunConfigurationExtraAspectId));
    setDisplayName(tr("Test settings"));
}

TestRunConfigurationExtraAspect::~TestRunConfigurationExtraAspect()
{

}

} // Internal
} // QTestLibPlugin
