#ifndef MYMACHINE_ENGINEER_BOX_H
#define MYMACHINE_ENGINEER_BOX_H
#include <QDialog>
#include <QWidget>
#include "mypushbutton.h"
#include "mylabel.h"
#include "globalparams.h"
#include "common/mymessagebox.h"
#include "common/myinputpanel.h"
#include "common/myinputmethod.h"

class MyContact : public QDialog
{
    Q_OBJECT
public:
    explicit MyContact(QString text,QWidget *parent = 0);
    void setText(QString text);

signals:

public slots:
   void backtohomeBtnClicked();

private:
    myPushButton    *backtohome;
    myLabel         *textLabel;
    myLabel         *machineLabel;
    myLabel         *machineLbinfo;
    myLabel         *engineerLabel;
    myLabel         *engineerLbinfo;
    QVBoxLayout     *Machine_Engineer_Layout;
    QVBoxLayout     *Machine_Layout;
    QVBoxLayout     *Engineer_Layout;
    QHBoxLayout     *Info_Layout;
    QHBoxLayout     *Tip_Layout;
};

#endif // MYMACHINE_ENGINEER_BOX_H
