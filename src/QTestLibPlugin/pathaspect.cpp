/* Copyright 2020 Pascal COMBES <pascom@orange.fr>
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

#include "pathaspect.h"
#include "Widgets/filetypevalidatinglineedit.h"

#include <utils/variablechooser.h>
#include <utils/layoutbuilder.h>
#include <utils/qtcassert.h>

#include <QtWidgets>

namespace QTestLibPlugin {
namespace Internal {

void PathAspect::fromMap(const Utils::Store& map)
{
    QTC_ASSERT(!settingsKey().isEmpty(), return);
    mValue = Utils::FilePath::fromSettings(map.value(settingsKey(), mDefaultValue.toSettings()));
    qDebug() << __func__ << settingsKey().toByteArray() << "value:" << mValue;
}

void PathAspect::toMap(Utils::Store& map) const
{
    QTC_ASSERT(!settingsKey().isEmpty(), return);
    if (mValue != mDefaultValue)
        map.insert(settingsKey(), mValue.toSettings());
}

void PathAspect::setDefaultValue(const Utils::FilePath& defaultValue)
{
    bool isDefault = (mValue == mDefaultValue);
    mDefaultValue = defaultValue;

    if (isDefault)
        mValue = defaultValue;
}

void PathAspect::setValue(const Utils::FilePath& value)
{
    bool same = (mValue == value);
    mValue = value;
    qDebug() << __func__ << settingsKey().toByteArray() << "value:" << mValue;

    if (!same)
        emit changed();
}

void PathAspect::addToLayoutImpl(Layouting::Layout& builder)
{
    mEdit = createSubWidget<Widgets::FileTypeValidatingLineEdit>();
    mEdit->setMacroExpander(mMacroExpanderProvider());
    mEdit->setAcceptFlags(mAccepted);
    mEdit->setText(mValue.nativePath());

    if (mMacroExpanderProvider != nullptr) {
        Utils::VariableChooser* variableChooser = new Utils::VariableChooser(builder.emerge());
        variableChooser->addSupportedWidget(mEdit);
        variableChooser->addMacroExpanderProvider(mMacroExpanderProvider);
    }

    mButton = new QPushButton(tr("Browse..."));

    connect(mEdit, SIGNAL(validChanged(bool)), this, SLOT(update(bool)));
    connect(mEdit, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(mButton, SIGNAL(released()), this, SLOT(browse()));

    QLayout* fieldLayout = new QHBoxLayout;
    fieldLayout->addWidget(mEdit);
    fieldLayout->addWidget(mButton);

    if (mCheckable) {
        mCheckbox = createSubWidget<QCheckBox>(displayName() + ":");
        mCheckbox->setChecked(mValue != mDefaultValue);
        mEdit->setEnabled(mCheckbox->isChecked());
        mButton->setEnabled(mCheckbox->isChecked());

        connect(mCheckbox, SIGNAL(toggled(bool)), this, SLOT(updateState(bool)));

        builder.addItem(mCheckbox);
        builder.addItem(fieldLayout);
    } else {
        mLabel = createSubWidget<QLabel>(displayName() + ":");
        mLabel->setBuddy(mEdit);

        builder.addItem(mLabel);
        builder.addItem(fieldLayout);
    }
}

void PathAspect::manageAcceptFlags(Widgets::FileTypeValidatingLineEdit::Accept flag, bool enable)
{
    if (enable)
        mAccepted |= flag;
    else
        mAccepted &= ~Widgets::FileTypeValidatingLineEdit::Accepts(flag);
}

void PathAspect::update(void)
{
    if (mEdit->isValid())
        setValue(Utils::FilePath::fromUserInput(mEdit->text()));
    mEdit->setText(mValue.nativePath());
}

void PathAspect::update(bool valid)
{
    if (valid)
        setValue(Utils::FilePath::fromUserInput(mEdit->text()));
}

void PathAspect::updateState(bool checked)
{
    mEdit->setEnabled(checked);
    mButton->setEnabled(checked);
    setValue(checked ? Utils::FilePath::fromUserInput(mEdit->text()) : Utils::FilePath());
}

void PathAspect::browse(void)
{
    QString path;
    if (mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptsDirectories) {
        path = QFileDialog::getExistingDirectory(nullptr, displayName(), mValue.nativePath());
    } else if (mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptsFiles) {
        if (mAccepted & Widgets::FileTypeValidatingLineEdit::AcceptNew)
            path = QFileDialog::getSaveFileName(nullptr, displayName(), mValue.nativePath());
        else
            path = QFileDialog::getOpenFileName(nullptr, displayName(), mValue.nativePath());
    }

    if (!path.isNull())
        setValue(Utils::FilePath::fromString(path));
    mEdit->setText(mValue.nativePath());
}

} // Internal
} // QTestLibPlugin
