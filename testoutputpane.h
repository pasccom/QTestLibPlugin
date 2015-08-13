#ifndef TESTOUTPUTPANE_H
#define TESTOUTPUTPANE_H

#include <coreplugin/ioutputpane.h>
#include <utils/outputformat.h>

class QTreeView;

namespace ProjectExplorer {
    class RunControl;
}

namespace QTestLibPlugin {
namespace Internal {

// TODO Change it into class QAbstractItemModel;
class TestSuiteModel;

// TODO some functions will be inlined
class TestOutputPane : public Core::IOutputPane
{
    Q_OBJECT
public:
    TestOutputPane(TestSuiteModel *model) : mModel(model), mOutputWidget(NULL) {}
    bool canFocus(void) const {return false;}
    bool canNavigate(void) const {return true;}
    bool canNext(void) const {return false;}
    bool canPrevious(void) const {return false;}
    void clearContents(void);
    QString displayName(void) const {return trUtf8("Test output");}
    void goToNext(void) {}
    void goToPrev(void) {}
    bool hasFocus(void) const {return false;}
    QWidget* outputWidget(QWidget * parent);
    int priorityInStatusBar(void) const {return 10;}
    void setFocus(void) {}
    QList<QWidget *> toolBarWidgets(void) const {return QList<QWidget *>();}
    void visibilityChanged(bool visible) {}
private:
    TestSuiteModel *mModel;
    QTreeView *mOutputWidget;
//    Core::MessageOutputWindow *mDebug;
};

} // namespace Internal
} // namespace QTestLibPlugin

#endif // TESTOUTPUTPANE_H
