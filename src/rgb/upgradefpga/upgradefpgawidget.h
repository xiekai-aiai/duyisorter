#ifndef UPGRADEFPGAWIDGET_H
#define UPGRADEFPGAWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include "globalparams.h"
#include "globalflow.h"
#include "common/mypushbutton.h"
#include "common/mylineedit.h"
#include "common/mylabel.h"
#include "common/mycombobox.h"
#include "selectfiledialog.h"
#include "mywid.h"

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

#define FILE_SIZE_4M    4*1024*1024
#define FILE_SIZE_2M    2*1024*1024
#define FILE_SIZE_512K  512*1024

/*!
 * \brief The UpgradeFPGAWidget class
 */
class UpgradeFPGAWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpgradeFPGAWidget(QWidget *parent = 0);
    void initLocalParams(void);     /*!< 初始化本地参数 */
    void initUpdateParams(void);    /*!< 初始化升级参数*/
    void initTextParams(void);    /*!< 初始化文本参数 */
    void createPage(void);          /*!< 创建页面 */
    void connectSigAndSlt(void);    /*!< 连接信号与槽 */
    void refreshDisplay(void);      /*!< 刷新界面显示 */
    void refreshBoardTotal();       //更新各类型板数量
    void refreshSelBtnsDisplay();   /*!< 刷新板级使能的显示 */
    void refreshBoardComboBoxDisplay(void);     /*!< 刷新组合框的显示 */
    void refreshModeComboBoxDisplay(void);      /*!< 刷新组合框的显示 */
    void refreshFileNameDisplay();  /*!< 刷新升级文件名的显示 */
    void refreshInitDisplay(void);        /*!< 刷新升级状态 */
    void refreshCurrentDisplay(int nType);      /*!< 刷新当前板升级结果显示 */
    void refreshResultDisplay(void);        /*!< 刷新升级结果显示 */
    bool checkUpgradeBoardEn(void);         /*!< 检查要升级板级使能 */
    void clearUpgradeStatus(int nType);     /*!< 清空对应下位机的升级状态 */
    QString getTextByStatus(int nType, int nStat, int nBoard);  /*!< 根据状态获取文本 */
    QString getTextUpgradeInfo(int nType,int step);

    bool stepSwitchToFactory(int nType);    /*!< 升级步骤1: 切换至工厂模式 */
    bool stepSendFileByUSB(int nType);      /*!< 升级步骤2: 发送升级文件 */
    bool stepBurnFile(int nType);           /*!< 升级步骤3: 烧写文件 */
    bool stepSwtichToUser(int nType);       /*!< 升级步骤4: 切换至用户模式 */

    void USBSendFile(int cmd, unsigned char module, int num,int nType); /*!< USB发送文件 */

    bool checkBoardStatus(int nType, int nUnit);      /*!< 检查对应下位机的升级状态 */
    bool checkUpdateStatus(int nBoardType, int nUnit, int nStatType);    /*!< 检查下位机状态 */
    bool checkStatus(int nBoardType,int nStatType);     /*!< 检查板级升级状态 */

    bool upgradeIntBoard();             /*!< 接口板升级*/
    bool upgradeMajorBoard(int nType);  /*!< 彩色相机板升级*/
    void upgradeAssistBoard(int nType); /*!< 红外相机板升级*/

signals:
    void sBackToFactorySetPage();

public slots:
    void onSelBtnsPressed(int nIndex);          /*!< 槽函数: 使能按钮 */
    void onBoardComboBoxesPressed(int nIndex);  /*!< 槽函数: 组合框 */
    void onModeComboBoxesPressed(int nIndex);   /*!< 槽函数: 组合框 */
    void onFileLineEditsPressed(int nIndex);    /*!< 槽函数: 文件选择 */
    void onUpgradeBtnPressed(void);             /*!< 槽函数: 升级 */
    void onBackBtnPressed(void);                /*!< 槽函数: 返回 */

private:
    //! 枚举板类型
    enum {
        BOARD_INT = 0,          /*!< 接口相机板 */
        BOARD_MAJOR_FRONT,      /*!< 主配前视相机板 */
        BOARD_MAJOR_REAR,       /*!< 主配后视相机板 */
        BOARD_ASSIST_FRONT,     /*!< 辅配前视相机板 */
        BOARD_ASSIST_REAR,      /*!< 辅配后视相机板 */
        BOARD_TYPE              /*!< 板类型 */
    };

    //! 枚举升级状态
    enum {
        UPGRADE_UNSTARTED = 0,  /*!< 状态: 未开始 */
        UPGRADE_DOING,          /*!< 状态: 升级中 */
        UPGRADE_FACTORY,        /*!< 状态: 切换至工厂模式 */
        UPGRADE_ERR_FACTORY,    /*!< 状态: 切换工厂模式失败 */
        UPGRADE_SEND,           /*!< 状态: 发送升级数据包 */
        UPGRADE_ERR_SEND,       /*!< 状态: 发送数据包失败 */
        UPGRADE_BURN,           /*!< 状态: 烧写程序 */
        UPGRADE_ERR_BURN,       /*!< 状态: 烧写程序失败 */
        UPGRADE_USER,           /*!< 状态: 切换至用户模式 */
        UPGRADE_ERR_USER,       /*!< 状态: 切换用户模式失败 */
        UPGRADE_OK              /*!< 状态: 成功 */
    };

    //! FPGA反馈状态掩码
    typedef enum {
        MASK_BURN_FLASH_FILE	    = 0x01 << 0,    /*!< 烧写用户配置文件状态 */
        MASK_LOAD_CUSTOM_MODE	    = 0x01 << 1,    /*!< 载入用户模式成功状态 */
        MASK_LOAD_FACTORY_MODE	    = 0x01 << 2,    /*!< 载入工厂模式成功状态 */
        MASK_CRC_ERROR              = 0x01 << 3,    /*!< CRC校验错误状态 */
        MASK_BURN_SDRAM_PAGE_FILE   = 0x01 << 4,    /*!< 页文件写入SDRAM成功状态 */
        MASK_BOARD_ADDRESS_ERROR    = 0x01 << 5,    /*!< 数据包板地址错误状态 */
        MASK_BURN_SDRAM_ALL_FILE    = 0x01 << 6,    /*!< 文件写入SDRAM成功状态 */
        MASK_RSV		    = 0x01 << 7             /*!< RSV */
    }fpgaStatus;

    //! 升级信息结构体
    struct structUpgradeInfo {
        bool bEn;                   /*!< 板使能 */
        int nBoard;                 /*!< 需升级板号: 0-广播地址(所有电路板) */
        int nBoardTotal;
        int nMode;                  /*!< 升级模式*/
        int nstep[MAX_UNIT_PER_INT];
        QString sUpgradeInfo[MAX_UNIT_PER_INT];    /*!< 当前升级内容*/
        QString sFileName;          /*!< 升级文件名称 */
        QByteArray fileBuf;
        int nWriteLen;
        int nStatus[MAX_UNIT_PER_INT];  /*!< 升级状态 */
        QVector<int> qvIntUnit;
        QVector<int> qvMajorUnit;
        QVector<int> qvAssistUnit;
    };

    structUpgradeInfo m_struInfo[BOARD_TYPE];   /*!< 升级信息 */
//    char fileBuf[BOARD_TYPE][PACKET_COUNT_BIG*FILE_PACKET_SIZE];    /*!< 保存升级文件中的数据 */
    char packetBuf[PACKET_SIZE];            /*!< 保存串口读取到的数据 */

    myPushButton *m_selBtns[BOARD_TYPE];    /*!< 板使能按钮 */
    myLabel *m_typeLbls[BOARD_TYPE];        /*!< 板类型标签 */
    QList<QString> BoardTypeNameList;     /*!< 板类型标签名 */
    MyComboBox *m_boardComboBox[BOARD_TYPE];    /*!< 升级板选择控件 */
    MyComboBox *m_modeComboBox[BOARD_TYPE];     /*!< 升级模式选择控件 */
    myLineEdit *m_fileLineEdits[BOARD_TYPE];    /*!< 文件选择框 */
    myPushButton *m_upgradeBtn;             /*!< 升级按钮 */
    myPushButton *m_backBtn;                /*!< 返回按钮 */
    myLabel *m_resultLbl;                   /*!< 结果显示控件 */

    QSignalMapper *m_selBtnSigMap;          /*!< 选择按钮信号集 */
    QSignalMapper *m_boardComboBoxSigMap;   /*!< 升级板选择控件信号集 */
    QSignalMapper *m_modeComboBoxSigMap;    /*!< 升级模式控件信号集 */
    QSignalMapper *m_fileSigMap;            /*!< 文件信号集 */

    int curBoardMode;       /*!< 当前板类型 */
    int step;               /*!< 当前步骤 */
    QList<QString> IntStepInfoList;
    QList<QString> MajorStepInfoList;
    QList<QString> SlaveStepInfoList;

};

#endif // UPGRADEFPGAWIDGET_H
