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

#include "qtestlibpluginconstants.h"
#include "testoutputpane.h"
#include "qtestlibmodel.h"
#include "testproxymodel.h"

#include <QtCore>
#include <QtWidgets>
#include <QtDebug>

// TODO remove it
#include <QLabel>

namespace QTestLibPlugin {
namespace Internal {

TestOutputPane::TestOutputPane(QAbstractItemModel *model) :
    mModel(model), mOutputWidget(NULL)
{
    mProxy = new TestProxyModel(this);
    mProxy->setSourceModel(model);
}

QWidget* TestOutputPane::outputWidget(QWidget * parent)
{
    if (mOutputWidget ==  NULL) {
        mOutputWidget = new QTreeView(parent);
        mOutputWidget->setModel(mProxy);
        mOutputWidget->setRootIndex(QModelIndex());
        mOutputWidget->setAllColumnsShowFocus(true);
        mOutputWidget->setExpandsOnDoubleClick(true);
        mOutputWidget->setIndentation(22);
        mOutputWidget->setRootIsDecorated(false);
    } else {
        mOutputWidget->setParent(parent);
    }

    if (mToolbarWidgets.isEmpty()) {
        int t = QTestLibModel::FirstMessageType;
        while (++t < QTestLibModel::LastMessageType)
            mToolbarWidgets.append(new TestProxyButton((QTestLibModel::MessageType) t, mProxy, parent));
    }

    return mOutputWidget;
}

QList<QWidget *> TestOutputPane::toolBarWidgets(void) const
{
    return mToolbarWidgets;
}

void TestOutputPane::clearContents(void)
{
    qDebug() << "User asked to clear test output pane";
    if (!QMetaObject::invokeMethod(mModel, "clear"))
        qWarning() << "Failed to clear model";

    // TODO add something more visible for the user (MessageBox?)
}

void TestOutputPane::saveSettings(QSettings* settings)
{
    settings->beginGroup(QTestLibPlugin::Constants::ViewGroup);
    settings->setValue(QTestLibPlugin::Constants::DescWidthKey, QVariant(mOutputWidget->columnWidth(0)));
    settings->setValue(QTestLibPlugin::Constants::FileWidthKey, QVariant(mOutputWidget->columnWidth(1)));
    settings->setValue(QTestLibPlugin::Constants::LineWidthKey, QVariant(mOutputWidget->columnWidth(2)));
    settings->endGroup();

    settings->beginGroup(QTestLibPlugin::Constants::FilterProxyGroup);
    int t = QTestLibModel::FirstMessageType;
    while (++t < QTestLibModel::LastMessageType)
        settings->setValue(QTestLibModel::resultString((QTestLibModel::MessageType) t),
                           QVariant(mProxy->isMessageTypeEnabled((QTestLibModel::MessageType) t)));
    settings->endGroup();
}

} // namespace Internal
} // namespace QTestLibPlugin

