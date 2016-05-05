/* Copyright 2016 Pascal COMBES <pascom@orange.fr>
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

#include "testproxymodel.h"

#include <utils/qtcassert.h>

#include <QtWidgets>
#include <QtDebug>

namespace QTestLibPlugin {
namespace Internal {

TestProxyModel::TestProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    mFilters.resize((int) QTestLibModel::LastMessageType);

    for (int i = 0; i < mFilters.size(); i++)
        mFilters[i] = true;
}

bool TestProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QTC_ASSERT(sourceIndex.isValid(), return false);

    QVariant messageType = sourceIndex.data(QTestLibModel::ResultRole);
    Q_ASSERT(messageType.canConvert<QTestLibModel::MessageType>());

    return mFilters.at((int) messageType.value<QTestLibModel::MessageType>());
}

TestProxyButton::TestProxyButton(QTestLibModel::MessageType messageType, TestProxyModel *proxy, QWidget *parent)
    :QToolButton(parent), mType(messageType), mProxy(proxy)
{
    setIcon(QTestLibModel::messageIcon(messageType));
    setToolTip(QTestLibModel::resultStringTr(messageType));
    setCheckable(true);
    if (mProxy != NULL)
        setChecked(mProxy->isMessageTypeEnabled(messageType));

    connect(this, SIGNAL(clicked(bool)),
            this, SLOT(handleClick(bool)));
}

void TestProxyButton::handleClick(bool checked)
{
    if (mProxy == NULL)
        return;

    if (checked)
        mProxy->enableMessageType(mType);
    else
        mProxy->disableMessageType(mType);
}

void TestProxyButton::paintEvent(QPaintEvent *pe)
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QColor color;
    if (isChecked())
        color = QColor(0, 255, 0);
    else
        color = QColor(255, 0, 0);

    QPainter p(this);
    if (opt.state & QStyle::State_MouseOver)
        color.setAlphaF(0.3);
    else
        color.setAlphaF(0.15);
    p.fillRect(pe->rect(), color);

    if (opt.state & QStyle::State_HasFocus)
        color.setAlphaF(0.3);
    else
        color.setAlphaF(0.);
    p.setBrush(Qt::NoBrush);
    p.setPen(color);
    p.drawRect(pe->rect().adjusted(0, 0, -1, -1));

    style()->drawControl(QStyle::CE_ToolButtonLabel, &opt, &p);
}

} // namespace Internal
} // namespace QTestLibPlugin


