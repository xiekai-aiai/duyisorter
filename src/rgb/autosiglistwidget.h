#ifndef AUTOSIGLISTWIDGET_H
#define AUTOSIGLISTWIDGET_H

#include <QWidget>
#include "common/mypushbutton.h"
#include "common/mylabel.h"
#include "common/mylineedit.h"
#include "common/mylcdnumber.h"
#include "common/myslider.h"
#include "common/mylistwidget.h"
#include "common/mylistwidgetitem.h"
#include "common/mymessagebox.h"
#include "common/mygroupbox.h"
#include "common/myinputpanel.h"
#include "common/mycustomcheckbox.h"
#include "common/myinputmethod.h"
#include "globalparams.h"
#include "globalflow.h"


class autoSigListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit autoSigListWidget(QWidget *parent = 0);
    
    myLabel    *m_unitLabel[MAX_UNIT/2];
    myLineEdit *m_autoSigRValLbe[MAX_UNIT/2];
    myLineEdit *m_autoSigGValLbe[MAX_UNIT/2];
    myLineEdit *m_autoSigBValLbe[MAX_UNIT/2];

    myPushButton *m_viewChaneBtn;
    myCustomCheckBox  *m_autoSigInitEnableCbx;
    myPushButton *m_autoSigListBackBtn;

    QSignalMapper *m_autoSigListSigMapper;

    bool m_bView;

    void updateAutoSigListWidget();
signals:
    void sBackToFactorySetPage();
public slots:
    void autoSigListValueChange(int index);
    void onViewChangePressedBtnSlt();
    void onSigInitEnbalePressedCbxSlt();
    void onAutoSigListBackPressedBtnSlt();
};

#endif // AUTOSIGLISTWIDGET_H
