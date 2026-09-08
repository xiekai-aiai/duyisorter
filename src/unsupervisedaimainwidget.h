/*!
 * \brief  unsupervised Learning Main Widgets
 * \date   20161021
 */

#ifndef UNSUPERVISEDAIMAINWIDGET_H
#define UNSUPERVISEDAIMAINWIDGET_H

#include <QWidget>
#include <QLCDNumber>
#include <QScrollArea>
#include <QScrollBar>
#include "globalparams.h"
#include "common/mypushbutton.h"
#include "common/mygroupbox.h"
#include "common/mylabel.h"
#include "common/mygroupbox.h"
#include "common/globalconfig.h"
#include "common/mylineedit.h"
#include "common/myinputpanel.h"
#include "src/rgb/aimainwidget.h"
#ifdef Q_OS_UNIX
#include "common/myusb/myusb.h"
#endif

class UnsupervisedAIMainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UnsupervisedAIMainWidget(QWidget *parent = 0);

    void updateAIMainWidget();
private:
    enum{
        SM_AI_MAIN_PAGE,        /*!< 主界面 */
        SM_AI_SIM_PAGE          /*!< 仿真页面 */
    };

    QWidget        *pageAIMain;
    QWidget        *pageSimMain;
    QStackedWidget  *m_stackedWidget;

    //! pageAIMain
    myPushButton    *m_AIStartBtn;
    myPushButton    *m_AISimBtn;
    myPushButton    *m_AIBackBtn;

    //! pageSimMain
    myLabel         *m_showImgLbe;                  /*!< 图片显示 */
    myPushButton    *m_simBackBtn;                  /*!< 返回主界面 */
    QScrollArea     *m_imgScrollArea;               /*!< 滚动条 */
    myGroupBox      *m_chuteGroup;
    myLabel         *m_chuteLbe;
    myPushButton    *m_plusBtn;
    myPushButton    *m_minusBtn;
    QLCDNumber      *m_lcdNum;
    myLabel         *m_feedSetLbe;
    myLineEdit      *m_feedLne;
    myLabel         *m_materPerLbe;
    myLineEdit      *m_materPerSetLne;
    myGroupBox      *m_imgListGbx;
    myPushButton    *m_imgListPlusBtn;
    QLCDNumber      *m_imgListLcdNum;
    myPushButton    *m_imgListMinusBtn;
    myPushButton    *m_imgSimBtn;
    myLabel         *m_imgAISensLbe;
    myLineEdit      *m_imgAISensLne;

    struct classQuo m_struClassQuo[MAX_VIEW_LOCATION];
    int             m_nLayer;
    int             m_nNumOfImg;
    bool            m_bFlgSim;
    int             m_nChuteFeed;
    int             m_nSens[MAX_VIEW_LOCATION][24];
    int             m_nImgTotal;
    bool            m_bLessPoint;   // true - 物料点不够
    int             m_nFrontBkgLamp;
    int             m_nRearBkgLamp;

    //! function
    void            createAIMainWidge();
    void            createSimMainWidget();
    void updateImage();                                                 /*!< 更新页面 */
    bool getImage();                                                                            /*!< 获取图片 */
    char *getHead(const char *mem, int size, const char *str);                                  /*!< 检查包头 */
    bool unsupervisedLearning();                                                                /*!< 私有函数：无监督自学习 */
    int IsImageOk(int nUnit,int nImgNum);                                                                /*!< 私有函数：图片是否满足要求 */
    bool saveAIConfig(int nMaterPer, int nImgNum, int nFlgView, int nResMode, int nResSens);    /*!< 私有函数：保存智能设置 */
    int getGlobalAIParam(int classMode,int levelNum, int groupNum, int indexOfTrain);          /*!< 私有函数：获取全局智能参数 */
    bool pixelIsEdgeImage(QImage *image,int x, int y);
    bool pixelIsBad(int r, int g, int b);                                                       /*!< 多分类智能算法下判断像素点是否为坏点 */
    int  getUnitGroup(int level, int unit);
    void saveBMPFile(QString name, int width, int height, uchar *data); /*!< 私有函数：将图片数据保存为BMP文件 */
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateInitImgSlt();                        /*!< 如果默认有图片，默认更新*/
    void updateSimImg();
    void adjustBackground(int nLevelId, bool bIsFront);
    void setBkgValueAuto(bool bAdjust);     /*!< 自动设置背景值 */
    void resetBkgLampIndex(void);               // 重置背景灯索引
    void getBkgValue(int nUnit, int &nVal, int &nPer);  /*!< 获取单相机背景值 */
signals:
    void goToHomePage();                            /*!< 信号函数：返回智能主界面 */

public slots:
    void onAIBackPressedBtnSlt();
    void onAIStartPressedBtnSlt();                  /*!< 无监督智能开始 */
    void onSimBackPressedBtnSlt();                  /*!< 返回主界面 */
    void onAISimPressedBtnSlt();                    /*!< 仿真 */
    void onImgSImPressedBtnSlt();
    void onPlusPressedBtnSlt();
    void onMinusPressedBtnSlt();
    void onFeedPressedLneSlt();
    void onMaterPerSetLneSlt();
    void onImgListPlusBtnSlt();
    void onImgListMinusBtnSlt();
    void onImgAISensLneSlt();
};

#endif // UNSUPERVISEDAIMAINWIDGET_H
