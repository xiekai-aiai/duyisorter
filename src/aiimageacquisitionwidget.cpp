/*
 * @Description:
 * @version:
 * @Author: xiekai
 * @Date: 2026-09-17 15:50:40
 * @LastEditors: xiekai
 * @LastEditTime: 2026-09-17 18:19:02
 */
#include "aiimageacquisitionwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "mylanguage.h"
#include "mylabel.h"
#include "configmgr.h"
#include "unilog.h"

AiImageAcquisitionWidget::AiImageAcquisitionWidget(QWidget* parent) : QWidget(parent)
{
    // 使能AI图片采集
    enabelAcquComBox = new myCustomCheckBox(myLan.enable + myLan.image_acquisition, false);

    // 图片张数
    QHBoxLayout* hLayout = new QHBoxLayout();
    myLabel* picNumLbl = new myLabel(myLan.ai_pic_num);
    acqPicNumLineEdt = new myLineEdit();
    acqPicNumLineEdt->setFixedSize(BTN_WIDTH + 20, BTN_HEIGHT);
    quint16 pic_num = ConfigMgr::Instance().GetCollPicNum();
    acqPicNumLineEdt->setText(QString::number(pic_num));

    // 水平布局
    hLayout->addWidget(picNumLbl);
    hLayout->addWidget(acqPicNumLineEdt);
    hLayout->addSpacing(300);

    // 主界面采用垂直布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(enabelAcquComBox);
    mainLayout->addLayout(hLayout);

    connect(enabelAcquComBox, SIGNAL(pressed()), this, SLOT(onEnableAcquComBoxClicked()));
}

void AiImageAcquisitionWidget::onRefresh()
{
    // 读取配置赋值
    quint16 pic_num = ConfigMgr::Instance().GetCollPicNum();
    acqPicNumLineEdt->setText(QString::number(pic_num));
    bool enable_acq = ConfigMgr::Instance().GetEnableAcquisition();
    enabelAcquComBox->setChecked(enable_acq);
    LOG_INFO_STM("ai acquisition num:" << pic_num);
}

void AiImageAcquisitionWidget::onEnableAcquComBoxClicked()
{
    LOG_INFO_STM("enable acquisition checkbox clicked, enable acquisition:" << enabelAcquComBox->getChecked());
    ConfigMgr::Instance().SetEnableAcquisition(enabelAcquComBox->getChecked());
}


void AiImageAcquisitionWidget::onSave()
{
    // 保存配置
    ConfigMgr::Instance().SetEnableAcquisition(enabelAcquComBox->getChecked());
    QString pic_num = acqPicNumLineEdt->text();
    auto ret = ConfigMgr::Instance().SetCollPicNum(pic_num.toUShort());
    LOG_INFO_STM("save ai acquistion num:" << pic_num.toStdString() << " to db:" << ret << ", enable acquisition:" << enabelAcquComBox->getChecked());
}
