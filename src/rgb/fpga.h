/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        fpga.h
 * \brief       彩色机型下位机远程升级界面头文件
 * \date        2015.01.14
 */
#ifndef FPGA_H
#define FPGA_H

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QStackedWidget>
#include "common/globalconfig.h"
#include "common/mypushbutton.h"
#include "common/mymessagebox.h"
#include "common/mymutex.h"
#include "myqextserialport.h"

/* 通用机型下远程升级部分宏定义 */
#define PACKET_COUNT_BIG    16384       // 发送4M rpd文件需要读取包的次数
#define PACKET_COUNT        8192        // 发送2M rpd文件需要读取包的次数
#define PACKET_COUNT_SMALL  2048        // 发送512K rpd文件需要读取包的次数
#define FILE_PACKET_SIZE    256         // 文件包的大小
#define PACKET_SIZE         24          // 命令包的大小

/* 玉米机型下远程升级部分宏定义 */
#define FILE_PACKET_SIZE_MAIZE      8192    // 高速串口每个包大小为8192字节
#define PACKET_COUNT_BIG_MAIZE      512     // 高速串口4M升级文件共512个包
#define PACKET_COUNT_MAIZE          256     // 高速串口2M升级文件共256个包
#define PACKET_COUNT_SMALL_MAIZE    64      // 高速串口512K升级文件共64个包

#define SNED_FILE_BY_USB    1           // 表征当前发送升级文件的形式
#define UPDATE_FILE_PATH    "/udisk/"   // 升级文件目录

#ifdef Q_OS_UNIX
#include "common/myusb/myusb.h"
#endif

/* 整机升级方案二接口板信息*/
typedef struct {
    int nIntAddr;
    int intBoradMask;
}IntBoardConfig;

/* 整机升级方案二相机板信息*/
typedef struct {
    int nUnitAddr;
    int masterBoardMask;
    int slaveBoardMask;
    int failedMask;
}ColorBoardConfig;

/* 整机升级方案二板级信息*/
typedef struct {
    IntBoardConfig IntConfig[2];
    ColorBoardConfig ColorConfig[20];
    int dateLen;
}UpdateConfig;

/*!
 * \brief 彩色机型下位机远程升级界面类
 */
class fpga : public QWidget
{
    Q_OBJECT
    
public:
    fpga(QWidget *parent = 0);
    ~fpga();

    void initUpdateParams(void);        // 初始化下位机升级相关参数

    void createMountUdiskPage(void);    // 创建挂载U盘界面
    void createFileListPage(void);      // 创建文件列表界面
    void createNoticePage(void);        // 创建提示界面
    void createBoardUpdatePage(void);   // 创建板级升级界面
    void createWholeUpdatePage(void);   // 创建整机升级界面
    void setUpdateDisplay(void);        // 根据当前状态设置板级升级界面的显示
    void setUpdateWholeDisplay(void);   // 根据当前状态设置整机升级界面的显示
    void clearFileStatus(void);         // 清空文件的下载状态

    int intBoardUpdate(int step);       // 接口板升级流程
    int colorBoardUpdate(int step);     // 彩色相机板升级流程
    int infBoardUpdate(int step);       // 红外相机板升级流程
    int ctrlBoardUpdate(int step);      // 控制板升级流程
    void wholeUpdate(void);             // 整机升级
    void wholeUpdateMasterParallel(void);  // 新整机主板升级

    bool checkHasColor(void);           //! 检查当前机型是否存在彩色相机板
    void checkIs115(QString fileName);  //! 检查升级文件是否针对115芯片
    void setBoardConfig(UpdateConfig &config); //! 设置板级参数
    void checkBoardStatus(UpdateConfig &config,char packetMask,int step); //!查询板级状态以及界面显示
signals:
    void sBackToFactorySetPage();

private slots:
    void onStackedWidgetIndexChangedSlt(int index);
    void goToFirstPageSlt();

    /* 挂载页面的槽函数 */
    void onMountUdiskBackBtnClicked();
    void onMountUdiskGoBtnClicked();

    /* 文件列表界面的槽函数 */
    void onFileListChangedSlt(int index);
    void onFileListNextBtnClicked();
    void onFileListLastBtnClicked();
    void onTransModeBtnClicked();
    void onModeMutexIndexChanged(int index);

    /* 提示页面的槽函数 */
    void onNoticeLastBtnClicked();
    void onNoticeNextBtnClicked();

    /* 板级升级页面的槽函数 */
    void onBoardUpdateBackBtnClicked();
    void onBoardUpdateGoBtnClicked();
    void onBoardUpdateWholeBtnClicked();
    void onBoardUpdateRetryBtnClicked();
    void onUserModeChangedSlt(int index);
    void onLayerChangedSlt(int index);
    void onlayerAddChangedSlt(int index);
    void onBoardNumPlusBtnClicked();
    void onBoardNumMinusBtnClicked();

    /* 整机升级页面的槽函数 */
    void onWholeUpdateBackBtnClicked();
    void onWholeUpdateReturnBtnClicked();

private:
    /* 远程升级页面索引号 */
    enum{
        MOUNT_PAGE,         // 挂载页面
        FILE_LIST_PAGE,     // 文件列表界面
        NOTICE_PAGE,        // 提示页面
        UPDATE_PAGE,        // 板级升级页面
        WHOLE_UPDATE_PAGE   // 整机升级页面
    };

    /* 板类型索引号 */
    typedef enum {
        INT_BOARD,          // 接口板
        COLOR_BOARD,        // 彩色相机板
        INF_BOARD,          // 红外相机板
        CTRL_BOARD,         // 控制板
        INT_CTRL_BOARD      // 接口控制板
    }boardMode;

    enum {
        MES_NUM = 7,
        MAX_FILE_NUM = 18,
        MAX_BOARD_PER_LEVEL = 10
    };

    /* 升级状态索引号 */
    typedef enum {
        UPDATE_NO_START,    // 升级未开始
        UPDATE_DOING,       // 升级进行中
        UPDATE_OK,          // 升级成功
        UPDATE_FAIL,        // 升级失败
        UPDATE_NO_EXIST     // 升级板不存在
    }updateStatus;

    /* FPGA反馈状态掩码 */
    typedef enum {
        MASK_BURN_FLASH_FILE	    = 0x01 << 0, // 烧写用户配置文件状态
        MASK_LOAD_CUSTOM_MODE	    = 0x01 << 1, // 载入用户模式成功状态
        MASK_LOAD_FACTORY_MODE	    = 0x01 << 2, // 载入工厂模式成功状态
        MASK_CRC_ERROR		    = 0x01 << 3, // CRC校验错误状态
        MASK_BURN_SDRAM_PAGE_FILE   = 0x01 << 4, // 页文件写入SDRAM成功状态
        MASK_BOARD_ADDRESS_ERROR    = 0x01 << 5, // 数据包板地址错误状态
        MASK_BURN_SDRAM_ALL_FILE    = 0x01 << 6, // 文件写入SDRAM成功状态
        MASK_RSV		    = 0x01 << 7, // RSV
    }fpgaStatus;

    void USBSendFile(int cmd, unsigned char module, int num);   // USB发送文件

    bool stepSwitchToFactory(boardMode module); // 切换下位机至工厂模式
    bool stepSendFile(void);                    // 向下位机发送升级文件
    bool stepSendFileByHighUART(void);          // 通过高速串口发送升级文件
    bool stepSendFileByUSB(void);               // 通过USB向下位机发送升级文件
    bool stepBurnFile(void);                    // 下位机烧写升级文件
    bool stepSwitchToUser(boardMode module);    // 切换下位机至用户模式

    bool ctrlSwitchToFactory(void);             // 切换控制板至工厂模式
    bool ctrlEraseFlash(void);                  // 擦除控制板的用户控件
    bool ctrlSendFile(void);                    // 向控制板发送文件
    bool ctrlSendFileImprove(void);             // 向控制板发送文件（改进版）
    bool ctrlSwitchToUser(void);                // 切换控制板至用户模式
    void switchAllIntBoardToUser();             // 将所有接口板切换至用户模式


    void setStatusLabelShow(int mesIndex, updateStatus status);     //根据某一步升级状态显示不同内容
    void setWholeStatusShow(int level, int mesIndex, updateStatus status, int guage = 0);  //根据单板的升级状态显示不同内容,guage为不带%的百分比

    g_Config *config;
    QStackedWidget *stackedWidget;
    int margin;
    QPalette pe;
    int updateModeIndex;            //0,旧协议远程升级方式，1,新协议远程升级方式

    /* 下位机升级相关参数 */
    quint8 defIntMode;              //缺省的接口板用户模式
    quint8 defColorMode;            //缺省的彩色相机板用户模式
    boardMode curBoardMode;         //当前升级板子的类型
    quint8 intUserMode;             //当前接口板的用户模式
    quint8 colorUserMode;           //当前彩色相机板的用户模式
    quint8 infUserMode;             //当前红外相机板的用户模式
    quint8 ctrlUserMode;            //当前控制板的用户模式
    bool bColorFileIsOk[MAX_LEVEL]; //表示彩色相机升级文件是否已下载成功
    bool bInfFileIsOk[MAX_LEVEL];   //表示红外相机升级文件是否已下载成功
    quint8 intAddr;                 //接口板地址
    quint8 colorAddr;               //彩色相机板地址
    quint8 infAddr;                 //红外相机板地址
    quint8 ctrlAddr;                //控制板地址
    bool bIntIsFactory;             //表示当前接口板是否在工厂模式
    bool bColorIsFactory;           //表示当前红外相机板是否在工厂模式
    char packetBuf[PACKET_SIZE];    //保存串口读取到的数据
    uchar fileBuf[PACKET_COUNT_BIG][FILE_PACKET_SIZE];      //保存升级文件中的数据
    quint8 curUpdateStep;           //记录当前升级的步骤，0表示升级未开始或已成功
    quint8 curLevel;                //记录当前升级的层数
    bool bTransByUSB;               //表示是否使用USB下传升级文件
    int iswholeUpdating;            //此变量紧供新版升级方式使用。0:当前所在页面非整机升级页面，1：整机升级页面
    bool isAllsent;                 //文件发送命令是否全部从主板发给从板
    bool isAllSwitchToFactory;      //相机板切换工厂模式命令是否全部发送
    bool isAllSwitchToUser;         //相机板切换用户模式命令是否全部发送
    bool m_bIs115;                  //! 115芯片升级标志位

    /* 挂载界面的控件 */
    QWidget *mountUdiskWidget;
    QLabel *mountUdiskLabel;
    QLabel *warningLabel;
    myPushButton *mountUdiskBackBtn;
    myPushButton *mountUdiskGoBtn;

    /* 升级文件列表界面的控件 */
    QWidget *fileListWidget;
    QListWidget *fileList;
    myPushButton *lastBtn;
    myPushButton *nextBtn;
    QStringList updateFileList;
    QListWidgetItem *fileItem[MAX_FILE_NUM];
    myPushButton *transModeBtn;

    /* 提示页面的控件 */
    QWidget *noticeWidget;
    QLabel *noticeLabel;
    myPushButton *noticeLastBtn;
    myPushButton *noticeNextBtn;

    /* 板级升级界面的控件 */
    QWidget *boardUpdateWidget;
    myMutex *userModeGroup;
    myMutex *layerGroup;
    myMutex *layerMaizeAddGroup;
    QGroupBox *boardNumGroup;
    QGroupBox *messagesGroup;
    myPushButton *boardNumPlusBtn;
    myPushButton *boardNumMinusBtn;
    myPushButton *boardUpdateBackBtn;
    myPushButton *boardUpdateGoBtn;
    myPushButton *boardUpdateWholeBtn;
    myPushButton *boardUpdateRetryBtn;
    QLabel *updateMesLabels[MES_NUM];
    QLabel *updateStatusLabels[MES_NUM];
    QLCDNumber *boardUpdateLcdNum;

    /* 整机升级界面的控件 */
    QWidget *updateWholeWidget;
    myPushButton *updateWholeBackBtn;
    myPushButton *updateWholeRebootBtn;
    QLabel *boardNumLabels[MAX_VIEW_LOCATION][MAX_BOARD_PER_LEVEL];
    QLabel *statusLabels[MAX_VIEW_LOCATION][MAX_BOARD_PER_LEVEL];
};

#endif // FPGA_H
