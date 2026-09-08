#include "mycontact.h"

MyContact::MyContact(QString text,QWidget *parent) :
    QDialog(parent)
{
    int margin = 25;
    int nLabelHei;

    switch(LCD_WIDTH) {
    case 1024:
        margin = 25;
        nLabelHei = 60;
        break;
    case 640:
        margin = 5;
        nLabelHei = 50;
        break;
    }

    this->setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(LCD_WIDTH/4,LCD_HEIGHT/4,LCD_WIDTH/2,LCD_HEIGHT/2);
    setFixedSize(LCD_WIDTH/2,LCD_HEIGHT/2);
    setStyleSheet(g_style2);

    textLabel   = new myLabel(text);
    backtohome = new myPushButton(myLan.ok ,myIcon.Action_Apply);
    backtohome->setFixedSize(QSize(BTN_WIDTH/1.5,BTN_HEIGHT));

    QString strMachine,strEngineer;
    QString strMachineLbe,strEngineerLbe;

    strMachine = QString ("%1%2").arg("销售员").arg(":");
    strEngineer = QString ("%1%2").arg("电话").arg(":");
    machineLabel = new myLabel(strMachine);
    engineerLabel = new myLabel(strEngineer);

    strMachineLbe = struCnfg.MachineInfo ;
    strEngineerLbe =struCnfg.EngineerInfo;

    if(strMachineLbe.length()>=26){
        strMachineLbe.insert(26,"\n");
        if(strMachineLbe.length()>=53){
            strMachineLbe.insert(53,"\n");
        }
    }

    if(strEngineerLbe.length()>=26){
        strEngineerLbe.insert(26,"\n");
        if(strEngineerLbe.length()>=53){
            strEngineerLbe.insert(53,"\n");
        }
    }

    machineLbinfo = new myLabel(strMachineLbe);
    engineerLbinfo = new myLabel(strEngineerLbe);

    Machine_Engineer_Layout = new QVBoxLayout(this);
    Machine_Engineer_Layout->setContentsMargins(0,0,0,0);
    Machine_Layout = new QVBoxLayout();
    Engineer_Layout = new QVBoxLayout();
    Info_Layout = new QHBoxLayout();
    Tip_Layout = new QHBoxLayout();

    Info_Layout->addWidget(textLabel ,1 ,Qt::AlignCenter);
    Info_Layout->setContentsMargins(0,40,0,0);

    Machine_Layout->addWidget(machineLabel,0,Qt::AlignLeft);
    Machine_Layout->addWidget(machineLbinfo,0,Qt::AlignLeft);
    Machine_Layout->setContentsMargins(50,0,10,0);

    Engineer_Layout->addWidget(engineerLabel,0,Qt::AlignLeft);
    Engineer_Layout->addWidget(engineerLbinfo,0,Qt::AlignLeft);
    Engineer_Layout->setContentsMargins(50,0,10,0);

    Tip_Layout->addWidget(backtohome ,1 ,Qt::AlignRight);
    Tip_Layout->setContentsMargins(0,0,10,10);

    Machine_Engineer_Layout->addLayout(Info_Layout);
    Machine_Engineer_Layout->addLayout(Machine_Layout);
    Machine_Engineer_Layout->addLayout(Engineer_Layout);
    Machine_Engineer_Layout->addLayout(Tip_Layout);

    connect(backtohome, SIGNAL(pressed()) , this,SLOT(backtohomeBtnClicked()));
}

void MyContact::setText(QString text)
{
    textLabel->setText(text);
}

void MyContact::backtohomeBtnClicked()
{
    accept();
}
