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

