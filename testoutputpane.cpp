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

#include "testoutputpane.h"
#include "testsuitemodel.h"

#include <QtCore>
#include <QtWidgets>
#include <QtDebug>

// TODO remove it
#include <QLabel>

namespace QTestLibPlugin {
namespace Internal {

QWidget* TestOutputPane::outputWidget(QWidget * parent)
{
    if (mOutputWidget ==  NULL) {
        mOutputWidget = new QTreeView(parent);
        mOutputWidget->setModel(mModel);
        mOutputWidget->setRootIndex(QModelIndex());
        mOutputWidget->setAllColumnsShowFocus(true);
        mOutputWidget->setExpandsOnDoubleClick(true);
        mOutputWidget->setIndentation(22);
        mOutputWidget->setRootIsDecorated(false);
    } else {
        mOutputWidget->setParent(parent);
    }

    return mOutputWidget;
}

void TestOutputPane::clearContents(void)
{
    qDebug() << "User asked to clear test output pane";
    mModel->clear();
}


} // namespace Internal
} // namespace QTestLibPlugin

