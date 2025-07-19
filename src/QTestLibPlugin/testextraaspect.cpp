/* Copyright 2016-2020 Pascal COMBES <pascom@orange.fr>
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

#include "testextraaspect.h"
#include "qtestlibpluginconstants.h"

#include "Widgets/filetypevalidatinglineedit.h"

#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/runcontrol.h>

#include <qmakeprojectmanager/qmakeproject.h>

#include <utils/detailswidget.h>
#include <utils/processinterface.h>

#include <QtWidgets>

namespace QTestLibPlugin {
namespace Internal {

TestRunConfigWidget::TestRunConfigWidget(TestExtraAspect* aspect) :
    QWidget(), mAspect(aspect)
{
    Utils::Store map;
    mAspect->mTestArgsParser->toMap(map);

    mDetailWidget = new Utils::DetailsWidget(this);
    QWidget* mainWidget = new QWidget(mDetailWidget);
    mDetailWidget->setSummaryText(tr("Use default settings"));
    mDetailWidget->setWidget(mainWidget);

    mFormatCombo = new QComboBox(mainWidget);
    mFormatCombo->addItem(tr("Text format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::TxtFormat));
    //mFormatCombo->addItem(tr("CSV format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::CsvFormat));
    mFormatCombo->addItem(tr("XML format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::XmlFormat));
    mFormatCombo->addItem(tr("Light XML format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::LightXmlFormat));
    mFormatCombo->addItem(tr("XUnit XML format"), QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(QTestLibArgsParser::XUnitXmlFormat));
    mFormatCombo->setCurrentIndex(mFormatCombo->findData(QVariant::fromValue<QTestLibArgsParser::TestOutputFormat>(mAspect->mTestArgsParser->outputFormat())));
    mFormatLabel = new QLabel(tr("Test output format:"), mainWidget);
    mFormatLabel->setBuddy(mFormatCombo);

    mVerbosityCombo = new QComboBox(mainWidget);
    mVerbosityCombo->addItem(tr("Silent"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::Silent));
    mVerbosityCombo->addItem(tr("Normal"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::NormalVerbosity));
    mVerbosityCombo->addItem(tr("Verbose"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::Verbose1));
    mVerbosityCombo->addItem(tr("Very verbose"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::Verbose2));
    mVerbosityCombo->addItem(tr("Signals"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::VerboseSignal));
    //mVerbosityCombo->addItem(tr("Silent"), QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(QTestLibArgsParser::VerboseBenchmark));
    mVerbosityCombo->setCurrentIndex(mVerbosityCombo->findData(QVariant::fromValue<QTestLibArgsParser::TestVerbosity>(mAspect->mTestArgsParser->verbosity())));
    mVerbosityLabel = new QLabel(tr("Test output verbosity:"), mainWidget);
    mVerbosityLabel->setBuddy(mVerbosityCombo);

    mOutFileEdit = new Widgets::FileTypeValidatingLineEdit(mainWidget);
    mOutFileEdit->setAcceptNew(true);
    mOutFileEdit->setRequireWritable(true);
    mOutFileEdit->setText(map.value(Constants::OutputFileKey).toString());
    mOutFileLabel = new QLabel(tr("Test output file:"), mainWidget);
    mOutFileLabel->setBuddy(mOutFileEdit);
    mOutFileButton =  new QPushButton(tr("Browse..."), mainWidget);

    mWarningCheck = new QCheckBox(tr("Maximum number of messages"), mainWidget);
    mWarningSpin = new QSpinBox(mainWidget);
    mWarningSpin->setMinimum(0);
    mWarningSpin->setMaximum(INT_MAX);
    mWarningSpin->setSingleStep(100);
    mWarningSpin->setValue(mAspect->mTestArgsParser->maxWarnings());
    if (map.contains(Constants::MaxWarningKey))
        mWarningCheck->setChecked(true);
    else
        mWarningSpin->setEnabled(false);

    mEventDelayCheck = new QCheckBox(tr("Event simulation delay"), mainWidget);
    mEventDelaySpin = new QSpinBox(mainWidget);
    mEventDelaySpin->setMinimum(0);
    mEventDelaySpin->setMaximum(10000);
    mEventDelaySpin->setSingleStep(10);
    mEventDelaySpin->setSuffix(tr(" ms"));
    mEventDelaySpin->setValue(mAspect->mTestArgsParser->eventDelay());
    if (map.contains(Constants::EventDelayKey))
        mEventDelayCheck->setChecked(true);
    else
        mEventDelaySpin->setEnabled(false);

    mKeyDelayCheck = new QCheckBox(tr("Keyboard simulation delay"),mainWidget);
    mKeyDelaySpin = new QSpinBox(mainWidget);
    mKeyDelaySpin->setMinimum(0);
    mKeyDelaySpin->setMaximum(10000);
    mKeyDelaySpin->setSingleStep(10);
    mKeyDelaySpin->setSuffix(tr(" ms"));
    mKeyDelaySpin->setValue(mAspect->mTestArgsParser->keyDelay());
    if (map.contains(Constants::KeyDelayKey))
        mKeyDelayCheck->setChecked(true);
    else
        mKeyDelaySpin->setEnabled(false);

    mMouseDelayCheck = new QCheckBox(tr("Mouse simulation delay"),mainWidget);
    mMouseDelaySpin = new QSpinBox(mainWidget);
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
    mainWidget->setLayout(mainLayout);

    QBoxLayout *extLayout = new QHBoxLayout;
    extLayout->setContentsMargins(0, 0, 0, 0);
    extLayout->addWidget(mDetailWidget, 1);
    setLayout(extLayout);

    /*connect(mAspect, SIGNAL(testArgumentsChanged()),
            this, SLOT(handleTestArgumentsChange()));*/

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

    updateSummary();
}

void TestRunConfigWidget::updateSummary(void)
{
    QStringList summaryList;

    QString oldTestArgs(mDetailWidget->additionalSummaryText());

    switch (mFormatCombo->currentData().value<QTestLibArgsParser::TestOutputFormat>()) {
    case QTestLibArgsParser::TxtFormat:
        break;
    case QTestLibArgsParser::XmlFormat:
        summaryList << tr("format set to <b>XML</b>");
        break;
    /*case QTestLibArgsParser::CsvFormat:
        summary << tr("format set to <b>CSV</b>");
        break;*/
    case QTestLibArgsParser::LightXmlFormat:
        summaryList << tr("format set to <b>LightXML</b>");
        break;
    case QTestLibArgsParser::XUnitXmlFormat:
        summaryList << tr("format set to <b>XUnitXML</b>");
        break;
    default:
        Q_ASSERT(false); // The program should not reach this point.
        break;
    }

    switch (mVerbosityCombo->currentData().value<QTestLibArgsParser::TestVerbosity>()) {
    case QTestLibArgsParser::NormalVerbosity:
        break;
    case QTestLibArgsParser::Silent:
        summaryList << tr("verbosity set to <b>Silent</b>");
        break;
    case QTestLibArgsParser::Verbose1:
        summaryList << tr("verbosity set to <b>Verbose</b>");
        break;
    case QTestLibArgsParser::Verbose2:
        summaryList << tr("verbosity set to <b>Very verbose</b>");
        break;
    case QTestLibArgsParser::VerboseSignal:
        summaryList << tr("verbosity set to <b>Verbose signal</b>");
        break;
    /*case QTestLibArgsParser::VerboseBenchmark:
        summary << tr("verbosity set to <b>Verbose benchmark</b>");
        break;*/
    default:
        Q_ASSERT(false); // The program should not reach this point.
        break;
    }

    if (!mOutFileEdit->text().isEmpty())
        summaryList << tr("output logged in file");

    if (mWarningCheck->isChecked())
        summaryList << tr("max warnings set to <b>%1</b>").arg(mWarningSpin->value());
    if (mEventDelayCheck->isChecked())
        summaryList << tr("event delay set to <b>%1ms</b>").arg(mEventDelaySpin->value());
    if (mKeyDelayCheck->isChecked())
        summaryList << tr("key delay set to <b>%1ms</b>").arg(mKeyDelaySpin->value());
    if (mMouseDelayCheck->isChecked())
        summaryList << tr("mouse delay set to <b>%1ms</b>").arg(mMouseDelaySpin->value());

    QString summary;
    for (int i = 0; i < qMin(3, summaryList.size() - 1); i++)
        summary.append(summaryList.at(i)).append(QLatin1String(", "));
    if (!summary.isEmpty() && summaryList.size() <= 4)
        summary.chop(2);
    if (summaryList.size() > 4)
        summary.append("...");
    if (!summaryList.isEmpty() && !summary.isEmpty())
        summary.append(tr(" and "));
    if (!summaryList.isEmpty())
        summary.append(summaryList.last());
    if (!summary.isEmpty())
        summary = summary.at(0).toUpper() + summary.mid(1);
    else
        summary = tr("Use default settings");

    mDetailWidget->setSummaryText(summary);
    mDetailWidget->setAdditionalSummaryText(mAspect->mTestArgsParser->toString());

    if (oldTestArgs != mDetailWidget->additionalSummaryText())
        emit testArgumentsChanged(mDetailWidget->additionalSummaryText());
}

/*void TestRunConfigWidget::handleTestArgumentsChange(void)
{
    int index;

    // Output format
    index = 0;
    while ((index < mFormatCombo->count()) &&
           (mFormatCombo->itemData(index).canConvert<QTestLibArgsParser::TestOutputFormat>()) &&
           (mFormatCombo->itemData(index).value<QTestLibArgsParser::TestOutputFormat>() != mAspect->mTestArgsParser->outputFormat()))
        index++;

    QTC_ASSERT(index < mFormatCombo->count(), return);
    QTC_ASSERT(mFormatCombo->itemData(index).canConvert<QTestLibArgsParser::TestOutputFormat>(), return);
    mFormatCombo->setCurrentIndex(index);

    // Out file name
    mOutFileEdit->setText(mAspect->mTestArgsParser->outFileName().toString());

    // Verbosity
    index = 0;
    while ((index < mVerbosityCombo->count()) &&
           (mVerbosityCombo->itemData(index).canConvert<QTestLibArgsParser::TestVerbosity>()) &&
           (mVerbosityCombo->itemData(index).value<QTestLibArgsParser::TestVerbosity>() != mAspect->mTestArgsParser->verbosity()))
        index++;

    QTC_ASSERT(index < mVerbosityCombo->count(), return);
    QTC_ASSERT(mVerbosityCombo->itemData(index).canConvert<QTestLibArgsParser::TestVerbosity>(), return);
    mVerbosityCombo->setCurrentIndex(index);

    // Max warnings
    mWarningCheck->setChecked(mAspect->mTestArgsParser->maxWarnings() != 2000);
    mWarningSpin->setEnabled(mAspect->mTestArgsParser->maxWarnings() != 2000);
    mWarningSpin->setValue(mAspect->mTestArgsParser->maxWarnings());

    // Event delay
    mEventDelayCheck->setChecked(mAspect->mTestArgsParser->eventDelay() >= 0);
    mEventDelaySpin->setEnabled(mAspect->mTestArgsParser->eventDelay() >= 0);
    mEventDelaySpin->setValue(mAspect->mTestArgsParser->eventDelay());

    // Key delay
    mKeyDelayCheck->setChecked(mAspect->mTestArgsParser->keyDelay() >= 0);
    mKeyDelaySpin->setEnabled(mAspect->mTestArgsParser->keyDelay() >= 0);
    mKeyDelaySpin->setValue(mAspect->mTestArgsParser->keyDelay());

    // Mouse delay
    mMouseDelayCheck->setChecked(mAspect->mTestArgsParser->mouseDelay() >= 0);
    mMouseDelaySpin->setEnabled(mAspect->mTestArgsParser->mouseDelay() >= 0);
    mMouseDelaySpin->setValue(mAspect->mTestArgsParser->mouseDelay());

    mDetailWidget->setAdditionalSummaryText(mAspect->mTestArgsParser->toString());
    updateSummary();
}*/

void TestRunConfigWidget::updateFormat(int index)
{
    QTC_ASSERT(mFormatCombo->itemData(index).canConvert<QTestLibArgsParser::TestOutputFormat>(), return);
    mAspect->mTestArgsParser->setOutputFormat(mFormatCombo->itemData(index).value<QTestLibArgsParser::TestOutputFormat>());
    updateSummary();
}

void TestRunConfigWidget::updateVerbosity(int index)
{
    QTC_ASSERT(mVerbosityCombo->itemData(index).canConvert<QTestLibArgsParser::TestVerbosity>(), return);
    mAspect->mTestArgsParser->setVerbosity(mVerbosityCombo->itemData(index).value<QTestLibArgsParser::TestVerbosity>());
    updateSummary();
}

void TestRunConfigWidget::updateOutFile(bool valid)
{
    if (!valid)
        return;

    mAspect->mTestArgsParser->setOutFileName(Utils::FilePath::fromString(mOutFileEdit->text()));
    updateSummary();
}

void TestRunConfigWidget::updateOutFile(void)
{
    if (mOutFileEdit->isValid())
        mAspect->mTestArgsParser->setOutFileName(Utils::FilePath::fromString(mOutFileEdit->text()));
    else
        mOutFileEdit->setText(mAspect->mTestArgsParser->outFileName().toFSPathString());
    updateSummary();
}

void TestRunConfigWidget::browseOutFile(void)
{
    QString outFile = QFileDialog::getSaveFileName(this, tr("Choose test output file"), mAspect->mTestArgsParser->outFileName().toFSPathString());
    if (!outFile.isNull())
        mAspect->mTestArgsParser->setOutFileName(Utils::FilePath::fromString(outFile));
    updateOutFile();
}

void TestRunConfigWidget::updateMaxWarnings(int value)
{
    mAspect->mTestArgsParser->setMaxWarnings(value);
    updateSummary();
}

void TestRunConfigWidget::updateMaxWarnings(bool enabled)
{
    mWarningSpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setMaxWarnings(mWarningSpin->value());
    else
        mAspect->mTestArgsParser->setMaxWarnings();
    updateSummary();
}

void TestRunConfigWidget::updateEventDelay(int value)
{
    mAspect->mTestArgsParser->setEventDelay(value);
    updateSummary();
}

void TestRunConfigWidget::updateEventDelay(bool enabled)
{
    mEventDelaySpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setEventDelay(mEventDelaySpin->value());
    else
        mAspect->mTestArgsParser->setEventDelay();
    updateSummary();
}

void TestRunConfigWidget::updateKeyDelay(int value)
{
    mAspect->mTestArgsParser->setKeyDelay(value);
    updateSummary();
}

void TestRunConfigWidget::updateKeyDelay(bool enabled)
{
    mKeyDelaySpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setKeyDelay(mKeyDelaySpin->value());
    else
        mAspect->mTestArgsParser->setKeyDelay();
    updateSummary();
}

void TestRunConfigWidget::updateMouseDelay(int value)
{
    mAspect->mTestArgsParser->setMouseDelay(value);
    updateSummary();
}

void TestRunConfigWidget::updateMouseDelay(bool enabled)
{
    mMouseDelaySpin->setEnabled(enabled);
    if (enabled)
        mAspect->mTestArgsParser->setMouseDelay(mMouseDelaySpin->value());
    else
        mAspect->mTestArgsParser->setMouseDelay();
    updateSummary();
}

void TestExtraAspect::initArgParser(QTestLibArgsParser* argParser)
{
    if (argParser != nullptr)
        mTestArgsParser = new QTestLibArgsParser(*argParser);
    else
        mTestArgsParser = new QTestLibArgsParser;

    setId(Utils::Id(Constants::TestRunConfigurationExtraAspectId));
    setDisplayName(tr("Test settings"));
    setConfigWidgetCreator([this] {
        TestRunConfigWidget* runConfigWidget = new TestRunConfigWidget(this);
        /*connect(runConfigWidget, SIGNAL(testArgumentsChanged(const QString&)),
                this, SLOT(handleTestArgumentsChange(const QString&)));*/
        return runConfigWidget;
    });

    /*ProjectExplorer::ArgumentsAspect* argAspect = runConfiguration()->extraAspect<ProjectExplorer::ArgumentsAspect>();
    if (argAspect != NULL) {
        connect(argAspect, SIGNAL(argumentsChanged(const QString&)),
                this, SLOT(handleArgumentsChange(const QString&)));
        handleArgumentsChange(argAspect->arguments());
    }*/
}

TestExtraAspect::~TestExtraAspect()
{
    delete mTestArgsParser;
}

/*void TestExtraAspect::handleTestArgumentsChange(const QString& newArgs)
{
    ProjectExplorer::ArgumentsAspect* argAspect = runConfiguration()->extraAspect<ProjectExplorer::ArgumentsAspect>();
    if (argAspect != NULL) {
        QTestLibArgsParser parser(argAspect->arguments());
        QString testArgs = parser.unknownArgs().join(QLatin1Char(' '));
        testArgs.prepend(" ").prepend(newArgs);
        testArgs.append(" ").append(parser.selectedTestCases());
        argAspect->setArguments(testArgs.trimmed());
    }
}

void TestExtraAspect::handleArgumentsChange(const QString& newArgs)
{
    mTestArgsParser->setArgs(newArgs);
    emit testArgumentsChanged();
}*/

bool TestExtraAspect::isUseful(ProjectExplorer::RunConfiguration* runConfiguration)
{
    Q_ASSERT(runConfiguration != NULL);

    Utils::ProcessRunData runnable = runConfiguration->runnable();

    // Only accept qMake projects:
    QmakeProjectManager::QmakePriFileNode* qMakeRootNode = dynamic_cast<QmakeProjectManager::QmakePriFileNode*>(runConfiguration->target()->project()->rootProjectNode());
    if (qMakeRootNode == nullptr)
        return false;

    foreach(QmakeProjectManager::QmakeProFile *pro, qMakeRootNode->proFileNode()->proFile()->allProFiles()) {
        qDebug() << "Project name:" << pro->displayName();

        if (!pro->validParse())
            continue;

        // Check the executable matches the target:
        Utils::FilePath destDir = pro->targetInformation().destDir;
        if (!destDir.isAbsolutePath())
            destDir = pro->targetInformation().buildDir.pathAppended(pro->targetInformation().destDir.toFSPathString()).cleanPath();
        qDebug() << "TARGET:" << destDir.pathAppended(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target));
        qDebug() << "Executable:" << runnable.command.executable();
        if (destDir.pathAppended(Utils::HostOsInfo::withExecutableSuffix(pro->targetInformation().target)) != runnable.command.executable())
            continue;
        // Check the testlib is included:
        qDebug() << "QT variable:" << pro->variableValue(QmakeProjectManager::Variable::Qt);
        if (pro->variableValue(QmakeProjectManager::Variable::Qt).contains(QLatin1String("testlib"), Qt::CaseSensitive))
            return true;
    }

    return false;
}

} // Internal
} // QTestLibPlugin
