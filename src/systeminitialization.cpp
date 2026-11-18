/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        systeminitialization.cpp
 * \brief       系统初始化界面源文件
 *              子页面包括通信自检、自检列表、系统初始化及系统预热。
 * \date        2015.01.14
 */
#include "systeminitialization.h"
#include "bootstage.h"
#include <QCoreApplication>
#include "configmgr.h"
#include "unilog.h"
#include <QtMath>
#include <QFontMetrics>
#include <QFile>
#include <QDateTime>
#include <QHash>
#include <QVector>
#include "common/globalconfig.h"

/*!
 * \brief 汇总自检失败的硬件设备名称（只读 struGsh，不改变自检逻辑）
 */
static QString collectHwErrorDetail()
{
    QStringList errors;

    /* 接口板：波特率既不是 115200 也不是 921600 判为异常 */
    for (int i = 0; i < struCnfg.nInterfaceBoardTotal && i < MAX_INT; i++)
    {
        if (struGsh.nIntBaud[i] != MY_UARTBAUD_115200
            && struGsh.nIntBaud[i] != MY_UARTBAUD_921600)
        {
            errors << QCoreApplication::translate("communication", "接口板%1").arg(i + 1);
        }
    }

    /* 控制板 */
    for (int i = 0; i < struCnfg.nCtrlBoardTotal && i < MAX_CTRL; i++)
    {
        if (struGsh.nCtrlBaud[i] != MY_UARTBAUD_115200
            && struGsh.nCtrlBaud[i] != MY_UARTBAUD_921600)
        {
            errors << QCoreApplication::translate("communication", "控制板%1").arg(i + 1);
        }
    }

    /* 相机板：版本号任一字节为 255 判为异常 */
    for (int i = 0; i < struCnfg.struLevelInfo[0].nUnitLevelTotal; i++)
    {
        for (int j = 0; j < struCnfg.nLevelTotal; j++)
        {
            if (struGsh.struVer.sUnit[j][i][0] == 255
                || struGsh.struVer.sUnit[j][i][1] == 255)
            {
                errors << QCoreApplication::translate("communication", "相机板%1")
                              .arg(struCnfg.struLevelInfo[j].nUnitId[i] + 1);
            }
        }
    }

    return errors.join(QCoreApplication::translate("communication", "、"));
}

 /**
  * @brief communication::通信自检控件
  * @param parent
  */
communication::communication(QWidget* parent) :
    QWidget(parent)
{
    waitCommLabel = new myLabel(myLan.msg_communicating, this);
    waitCommLabel->setStyleSheet("color: white;");  

    /*页面布局*/
    commVBLayout = new QVBoxLayout(this);
    commVBLayout->addWidget(waitCommLabel);
    //    setFixedHeight(LCD_HEIGHT-LCD_TITLE_HEIGHT-LCD_STATUS_HEIGHT-10);
}
/**
 *   自动通信检测功能
 */
void communication::startComSlt()
{
    int ret = 0;
    int ret1 = 0;

    // ---------------- 组 3：界面与数据表 / 加密授权 ----------------
    myFlow.bootStage(BOOT_STAGE_UI, BOOT_STATUS_ACTIVE, tr("正在校验加密授权…"));

    bool encyOk = true;
    if (myMachineInfo.macEncyCheck())
    {
        setDccryt dlg;
        //! 启动页为置顶窗口，授权对话框需同样置顶才能显示在其上方
        dlg.setWindowFlag(Qt::WindowStaysOnTopHint, true);
        ret = dlg.exec();
        encyOk = (ret == QDialog::Accepted);
        //! 行为保持不变：授权未通过也继续启动，仅在启动页红色提示
    }

    if (encyOk)
    {
        myFlow.bootStage(BOOT_STAGE_UI, BOOT_STATUS_OK, tr("UI加密正常"));
    }
    else
    {
        myFlow.bootStage(BOOT_STAGE_UI, BOOT_STATUS_ERROR, tr("加密授权未通过"));
    }

    // initInterfaceTransMode(0) = AI采集模式 (简化版 arg5=0)
    // 注意: 全链路都是 FPGA eth0 UDP 推图给 AI板, 没有串口收图!
    // mode=0 是 AI采集模式, mode=1 是传统采集模式 (简化版, AIMainWidget 拍照时自己发完整版)
    // FPGA 上电默认也是 eth0 UDP 推图, 这里发一次 initInterfaceTransMode(0) 双重保险
    myFlow.initInterfaceTransMode(0);  // ← 开机走 AI采集模式

    // ---------------- 组 4：设备自检 / AI 板自检 ----------------
    myFlow.bootStage(BOOT_STAGE_HW, BOOT_STATUS_ACTIVE, tr("正在执行设备自检…"));

    ret = myFlow.initCommunication();

    QStringList aiFailed;
    if (myFlow.isAiCheckEnabled())
    {
        ret1 = myFlow.initAiCommunication(&aiFailed);
    }

    QString detail = collectHwErrorDetail();
    if (!aiFailed.isEmpty())
    {
        if (!detail.isEmpty())
        {
            detail += tr("、");
        }
        detail += aiFailed.join(tr("、"));
    }

    if (ret == 0 && ret1 == 0)
    {
        myFlow.bootStage(BOOT_STAGE_HW, BOOT_STATUS_OK, tr("硬件工作正常"));
    }
    else
    {
        if (detail.isEmpty())
        {
            detail = tr("设备通信异常（详见自检列表）");
        }
        myFlow.bootStage(BOOT_STAGE_HW, BOOT_STATUS_ERROR, tr("硬件异常：") + detail);
    }

    //#endif
    if (ret == 1 || ret1 == 1)
    {
        emit updateCommunicationSig();
        emit goToCommListPageSig();     // 自检失败，进入自检列表页面
    }
    else
    {
        emit goToInitPageSig();         // 自检成功，进入系统初始化页面
    }
}
/*************************************************************************************/
/***
 *   通信列表控件
 */

namespace {

/*! 状态类型（与参考图一致） */
enum StateCell
{
    ST_NORMAL = 0,
    ST_WARNING,
    ST_ABNORMAL,
    ST_OFFLINE,
    ST_UNUSED
};

/*! UI 素材统一放在 /res/png2/state_page 下，优先取 qrc，其次取磁盘目录 */
QString stateResPath(const QString& name)
{
    const QString qrc = QString(":/res/png2/state_page/%1").arg(name);
    if (QFile::exists(qrc))
        return qrc;

    const QString disk = QString(APP_PATH "/res/png2/state_page/%1").arg(name);
    if (QFile::exists(disk))
        return disk;

    return QString();
}

/*! 图标 pixmap 缓存，避免每帧重复加载 */
const QPixmap* stateResPixmap(const QString& name)
{
    static QHash<QString, QPixmap> cache;
    if (!cache.contains(name))
    {
        const QString path = stateResPath(name);
        cache.insert(name, path.isEmpty() ? QPixmap() : QPixmap(path));
    }
    return &cache[name];
}

QColor stateColor(int state)
{
    switch (state)
    {
    case ST_NORMAL:   return QColor(0x22, 0xC5, 0x5E);
    case ST_WARNING:  return QColor(0xF5, 0x9E, 0x0B);
    case ST_ABNORMAL: return QColor(0xEF, 0x44, 0x44);
    case ST_OFFLINE:  return QColor(0x64, 0x74, 0x8B);
    default:          return QColor(0x2A, 0x5E, 0x70);
    }
}

QString stateDotName(int state)
{
    switch (state)
    {
    case ST_NORMAL:   return "dot_normal.png";
    case ST_WARNING:  return "dot_warning.png";
    case ST_ABNORMAL: return "dot_abnormal.png";
    default:          return "dot_offline.png";
    }
}

/*! 面板样式：有切图用 9-slice，没有则用纯色描边兜底 */
QString panelStyle(const QString& objName, const QString& image, const QString& borderColor)
{
    const QString path = stateResPath(image);
    if (!path.isEmpty())
    {
        return QString("QFrame#%1 { border-image: url(%2) 12 12 12 12 stretch stretch; }")
               .arg(objName, path);
    }
    return QString("QFrame#%1 { background-color: rgba(13, 34, 48, 216);"
                   " border: 1px solid %2; border-radius: 10px; }")
           .arg(objName, borderColor);
}

void drawSignalBars(QPainter& p, const QRectF& r, int state)
{
    if (state == ST_OFFLINE || state == ST_UNUSED)
        return;

    /* 统一 3 根信号柱 */
    const int lit = (state == ST_NORMAL) ? 3 : ((state == ST_WARNING) ? 2 : 1);
    const QColor col = stateColor(state);
    const qreal gap = 3.0;
    const qreal bw = (r.width() - gap * 2) / 3.0;
    p.setPen(Qt::NoPen);
    for (int i = 0; i < 3; i++)
    {
        const qreal bh = r.height() * (0.40 + 0.30 * i);
        QRectF bar(r.left() + i * (bw + gap), r.bottom() - bh, bw, bh);
        p.setBrush(i < lit ? col : QColor(0x2A, 0x3F, 0x4E));
        p.drawRoundedRect(bar, 1.5, 1.5);
    }
}

/*! 矩阵默认状态文案：与 communicationList 共用同一 i18n 上下文（供 lupdate 抽取） */
QString matrixText(const char* text)
{
    return QCoreApplication::translate("communicationList", text);
}

} // namespace

/*!
 * \brief 相机 / AI 板状态矩阵（自绘，避免上百个 QLabel 在 RK3566 上重绘卡顿）
 */
class StateMatrixWidget : public QWidget
{
public:
    explicit StateMatrixWidget(QWidget* parent = 0)
        : QWidget(parent)
    {
        setMinimumHeight(110);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_rowLabelTitle = "CH";
        m_txtNormal = matrixText("Normal");
        m_txtAbnormal = matrixText("Abnormal");
        m_txtWarning = matrixText("Warning");
        m_txtOffline = matrixText("Offline");
    }

    void setRowLabelTitle(const QString& title)
    {
        m_rowLabelTitle = title;
        update();
    }

    void setColumns(const QStringList& names)
    {
        m_columns = names;
        update();
    }

    void setStateText(const QString& normal, const QString& abnormal,
                      const QString& warning, const QString& offline)
    {
        m_txtNormal = normal;
        m_txtAbnormal = abnormal;
        m_txtWarning = warning;
        m_txtOffline = offline;
        update();
    }

    void clearRows()
    {
        m_rowLabels.clear();
        m_rowStates.clear();
        update();
    }

    void appendRow(const QString& label, const QVector<int>& states)
    {
        m_rowLabels.append(label);
        m_rowStates.append(states);
        update();
    }

protected:
    void paintEvent(QPaintEvent*)
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::TextAntialiasing, true);

        const QRectF panel = QRectF(rect()).adjusted(2, 2, -2, -2);
        const int cols = qMax(1, m_columns.size());
        const qreal headerH = 28.0;

        int basePx = font().pixelSize();
        if (basePx <= 0)
            basePx = 14;

        /* 列标题 */
        QFont hf = font();
        hf.setPixelSize(qMax(10, basePx - 2));

        const int rows = m_rowLabels.size();
        const qreal bodyTop = panel.top() + headerH + 5;
        const qreal bodyH = panel.bottom() - bodyTop - 5;
        const qreal rowH = (rows > 0) ? qMin<qreal>(58.0, bodyH / rows) : 0.0;

        QFont numFont = font();
        numFont.setPixelSize(qMax(11, int(qMax<qreal>(18.0, rowH) * 0.40)));
        numFont.setBold(true);

        QFont stFont = font();
        stFont.setPixelSize(qMax(10, int(qMax<qreal>(18.0, rowH) * 0.24)));
        stFont.setBold(true);
        
        /* 行标签列宽按最宽文本自适应，避免 STATUS 之类被截断 */
        qreal labelW = QFontMetrics(hf).horizontalAdvance(m_rowLabelTitle);
        if (rowH >= 18.0)
        {
            const QFontMetrics fm(numFont);
            for (int r = 0; r < rows; r++)
                labelW = qMax<qreal>(labelW, fm.horizontalAdvance(m_rowLabels.value(r)));
        }
        const qreal chuteW = qMax<qreal>(64.0, labelW + 14.0);
        const qreal cellW = (panel.width() - chuteW - 10.0) / cols;

        p.setFont(hf);
        p.setPen(QColor(0x8F, 0xB4, 0xC4));

        p.drawText(QRectF(panel.left() + 4, panel.top() + 4, chuteW - 8, headerH - 4),
                   Qt::AlignCenter, m_rowLabelTitle);
        for (int c = 0; c < cols; c++)
        {
            p.drawText(QRectF(panel.left() + chuteW + c * cellW, panel.top() + 4,
                              cellW, headerH - 4),
                       Qt::AlignCenter, m_columns.value(c));
        }

        p.setPen(QColor(0x1E, 0x3A, 0x4C));
        p.drawLine(QPointF(panel.left() + 8, panel.top() + headerH),
                   QPointF(panel.right() - 8, panel.top() + headerH));

        if (rows <= 0)
            return;

        if (rowH < 18.0)
            return;

        for (int r = 0; r < rows; r++)
        {
            QRectF rowRect(panel.left() + 6, bodyTop + r * rowH,
                           panel.width() - 12, rowH - 4);
            if (rowRect.bottom() > panel.bottom() - 2)
                break;

            p.setPen(Qt::NoPen);
            p.setBrush((r % 2) ? QColor(0x0B, 0x20, 0x2E, 120)
                              : QColor(0x08, 0x18, 0x24, 90));
            p.drawRoundedRect(rowRect, 6, 6);

            p.setFont(numFont);
            p.setPen(QColor(0xE8, 0xF6, 0xFF));
            p.drawText(QRectF(rowRect.left() + 2, rowRect.top(), chuteW - 6, rowRect.height()),
                       Qt::AlignCenter, m_rowLabels.value(r));

            for (int c = 0; c < cols; c++)
            {
                const int state = m_rowStates.value(r).value(c, ST_UNUSED);
                QRectF chip(rowRect.left() + chuteW + c * cellW + 2,
                            rowRect.top() + 4,
                            cellW - 8, rowRect.height() - 8);

                const QColor col = stateColor(state);
                p.setPen(QPen(col, 1.2));
                p.setBrush(QColor(col.red(), col.green(), col.blue(),
                                  state == ST_UNUSED ? 10 : 32));
                p.drawRoundedRect(chip, 5, 5);

                if (state == ST_UNUSED)
                    continue;

                const qreal dotS = qMin<qreal>(12.0, chip.height() - 8);
                QRectF dotRect(chip.left() + 8, chip.center().y() - dotS / 2.0, dotS, dotS);
                const QPixmap* pm = stateResPixmap(stateDotName(state));
                if (pm && !pm->isNull())
                {
                    p.drawPixmap(dotRect.toRect(), *pm);
                }
                else
                {
                    p.setPen(Qt::NoPen);
                    p.setBrush(col);
                    p.drawEllipse(dotRect);
                }

                p.setPen(col.lighter(118));
                const qreal textLeft = dotRect.right() + 6;
                const qreal barsW = qMin<qreal>(34.0, chip.width() * 0.24);
                const qreal textW = qMax<qreal>(1.0, chip.right() - barsW - textLeft - 8);
                const QString txt = stateText(state);

                /* 字号按可用宽度收缩，保证 Abnormal 完整显示 */
                QFont tf = stFont;
                int px = tf.pixelSize();
                while (px > 9 && QFontMetrics(tf).horizontalAdvance(txt) > textW)
                    tf.setPixelSize(--px);
                p.setFont(tf);

                p.drawText(QRectF(textLeft, chip.top(), textW, chip.height()),
                           Qt::AlignVCenter | Qt::AlignLeft, txt);

                drawSignalBars(p, QRectF(chip.right() - barsW - 4,
                                         chip.center().y() - 9, barsW, 18), state);
            }
        }
    }

private:
    QString stateText(int state) const
    {
        switch (state)
        {
        case ST_NORMAL:   return m_txtNormal;
        case ST_WARNING:  return m_txtWarning;
        case ST_ABNORMAL: return m_txtAbnormal;
        case ST_OFFLINE:  return m_txtOffline;
        default:          return "--";
        }
    }

    QString m_rowLabelTitle;
    QStringList m_columns;
    QString m_txtNormal;
    QString m_txtAbnormal;
    QString m_txtWarning;
    QString m_txtOffline;
    QStringList m_rowLabels;
    QVector<QVector<int> > m_rowStates;
};

void communicationList::commuUpdateCtrl()
{
    QStringList names;
    QVector<int> states;

    for (int i = 0; i < struCnfg.nCtrlBoardTotal && i < MAX_CTRL; i++)
    {
        names << QString("CTRL-%1").arg(i + 1, 2, 10, QChar('0'));
        const bool ok = (struGsh.nCtrlBaud[i] == MY_UARTBAUD_115200
                         || struGsh.nCtrlBaud[i] == MY_UARTBAUD_921600);
        states << (ok ? ST_NORMAL : ST_ABNORMAL);
    }

    m_ctrlMatrix->clearRows();
    m_ctrlMatrix->setRowLabelTitle("CTRL");
    m_ctrlMatrix->setColumns(names);
    m_ctrlMatrix->setStateText(tr("Normal"), tr("Abnormal"), tr("Warning"), tr("Offline"));
    m_ctrlMatrix->appendRow(tr("Status"), states);
    m_ctrlMatrix->setVisible(!names.isEmpty());
}

void communicationList::commuUpdateCamera()
{
    /* AI 板通信状态以本次启动的 AI 自检结果为准；未启用 AI 检测时显示离线 */
    const bool aiChecked = myFlow.isAiCheckEnabled();

    /* 固定显示 4 列：前视 / 后视 / AI前视 / AI后视，AI 未自检时显示离线 */
    m_matrix->setRowLabelTitle(myLan.chute);
    m_matrix->setColumns(QStringList() << myLan.front_view << myLan.rear_view
                                       << QString("AI ") + myLan.front_view
                                       << QString("AI ") + myLan.rear_view);
    m_matrix->setStateText(tr("Normal"), tr("Abnormal"), tr("Warning"), tr("Offline"));
    m_matrix->clearRows();

    /* 通道号 01..，每行两路相机（前视 / 后视） */
    const int unitTotal = struCnfg.struLevelInfo[0].nUnitLevelTotal;
    if (unitTotal <= 0)
        return;

    const int rowTotal = (unitTotal + 1) / 2;
    QVector<QVector<int> > rows(rowTotal, QVector<int>(4, ST_UNUSED));

    for (int i = 0; i < unitTotal; i++)
    {
        const int row = i / 2;
        int frontFlag = -1;

        for (int j = 0; j < struCnfg.nLevelTotal && j < MAX_LEVEL; j++)
        {
            const int camNo = struCnfg.struLevelInfo[j].nUnitId[i];
            const bool front = ((camNo % 2) == 0);
            if (frontFlag < 0)
                frontFlag = front ? 1 : 0;

            const bool ok = (struGsh.struVer.sUnit[j][i][0] != 255
                             && struGsh.struVer.sUnit[j][i][1] != 255);
            const int state = ok ? ST_NORMAL : ST_ABNORMAL;

            int& cell = rows[row][front ? 0 : 1];
            if (cell == ST_UNUSED || (cell == ST_NORMAL && state == ST_ABNORMAL))
                cell = state;
        }

        /* AI 板按单元索引；AI 未自检时保留列并显示离线 */
        int aiState = ST_OFFLINE;
        if (aiChecked)
            aiState = struGsh.aiResult[i].isEmpty() ? ST_ABNORMAL : ST_NORMAL;

        rows[row][(frontFlag == 1) ? 2 : 3] = aiState;
    }

    for (int r = 0; r < rowTotal; r++)
    {
        m_matrix->appendRow(QString("%1").arg(r + 1, 2, 10, QChar('0')), rows[r]);
    }
}

void communicationList::updateCommunicationResultSlt()
{
    commuUpdateCtrl();
    commuUpdateCamera();

    /* 顶部汇总卡：通道 / 相机 / AI / 告警 */
    const bool aiEnabled = myFlow.isAiCheckEnabled();
    const int unitTotal = struCnfg.struLevelInfo[0].nUnitLevelTotal;
    const int chuteTotal = (unitTotal + 1) / 2;
    int chuteOn = 0, camTotal = 0, camOn = 0, aiTotal = 0, aiOn = 0, alarms = 0;

    for (int i = 0; i < unitTotal; i++)
    {
        bool frontOk = false;
        bool rearOk = false;
        for (int j = 0; j < struCnfg.nLevelTotal && j < MAX_LEVEL; j++)
        {
            const bool ok = (struGsh.struVer.sUnit[j][i][0] != 255
                             && struGsh.struVer.sUnit[j][i][1] != 255);
            camTotal++;
            if (ok)
                camOn++;
            else
                alarms++;

            const int camNo = struCnfg.struLevelInfo[j].nUnitId[i];
            if ((camNo % 2) == 0)
                frontOk = frontOk || ok;
            else
                rearOk = rearOk || ok;
        }
        if (frontOk && rearOk)
            chuteOn++;

        /* AI 板按单元索引统计 */
        if (aiEnabled)
        {
            aiTotal++;
            if (!struGsh.aiResult[i].isEmpty())
                aiOn++;
            else
                alarms++;
        }
    }

    for (int i = 0; i < struCnfg.nCtrlBoardTotal; i++)
    {
        if (struGsh.nCtrlBaud[i] != MY_UARTBAUD_115200
            && struGsh.nCtrlBaud[i] != MY_UARTBAUD_921600)
            alarms++;
    }

    const int values[4] = { chuteOn, camOn, aiOn, alarms };
    const int totals[4] = { chuteTotal, camTotal, aiTotal, -1 };
    for (int i = 0; i < 4; i++)
    {
        m_sumValue[i]->setText(QString("%1").arg(values[i], 2, 10, QChar('0')));
        if (totals[i] >= 0)
            m_sumTotal[i]->setText(QString("/ %1").arg(totals[i]));
        else
            m_sumTotal[i]->clear();
    }
    m_sumValue[3]->setStyleSheet(QString("color:%1; background:transparent;")
                                 .arg(alarms > 0 ? "#EF4444" : "#22C55E"));
}

communicationList::communicationList(QWidget* parent) :
    QWidget(parent)
{
    config = g_Config::getInstance();

    const QFont baseFont = config->getFont(DEFAULT_FONT_SIZE);
    const QFont smallFont = config->getFont(FONT_SIZE_12);

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(14, 6, 14, 8);
    rootLayout->setSpacing(8);

    /* ================= 顶部汇总卡 ================= */
    QHBoxLayout* summaryRow = new QHBoxLayout;
    summaryRow->setSpacing(10);

    const QString sumTitles[4] = { tr("CHUTES"), tr("CAMERAS"), tr("AI LINK"), tr("ALARMS") };
    const QColor sumColors[4] = { QColor(0x35, 0xC8, 0xDE), QColor(0x22, 0xC5, 0x5E),
                                  QColor(0x35, 0xC8, 0xDE), QColor(0xEF, 0x44, 0x44) };
    for (int i = 0; i < 4; i++)
    {
        summaryRow->addWidget(createSummaryCard(i, sumTitles[i], sumColors[i]), 1);
    }
    rootLayout->addLayout(summaryRow);

    /* ================= 主区域 ================= */
    QHBoxLayout* mainRow = new QHBoxLayout;
    mainRow->setSpacing(12);

    /* ---- 左：相机 / AI 矩阵 ---- */
    QFrame* matrixPanel = new QFrame;
    matrixPanel->setObjectName("matrixPanel");
    matrixPanel->setStyleSheet(panelStyle("matrixPanel", "card_normal.png", "#2A5E70"));

    QVBoxLayout* matrixLay = new QVBoxLayout(matrixPanel);
    matrixLay->setContentsMargins(12, 8, 12, 10);
    matrixLay->setSpacing(4);

    QHBoxLayout* matrixHdr = new QHBoxLayout;
    QLabel* matrixTitle = new QLabel(tr("CAMERA / AI MATRIX"));
    QFont secFont = baseFont;
    secFont.setBold(true);
    matrixTitle->setFont(secFont);
    matrixTitle->setStyleSheet("color:#8FB4C4; background:transparent; letter-spacing:1px;");
    QLabel* matrixHint = new QLabel(tr("Chute matrix · live"));
    matrixHint->setFont(smallFont);
    matrixHint->setStyleSheet("color:#4B6B7C; background:transparent;");
    matrixHdr->addWidget(matrixTitle);
    matrixHdr->addStretch();
    matrixHdr->addWidget(matrixHint);
    matrixLay->addLayout(matrixHdr);

    m_matrix = new StateMatrixWidget;
    m_matrix->setFont(baseFont);
    matrixLay->addWidget(m_matrix, 1);

    /* ---- 右：控制板（与相机板相同的矩阵控件，并排显示） ---- */
    QFrame* ctrlPanel = new QFrame;
    ctrlPanel->setObjectName("ctrlPanel");
    ctrlPanel->setStyleSheet(panelStyle("ctrlPanel", "card_normal.png", "#2A5E70"));

    QVBoxLayout* ctrlLay = new QVBoxLayout(ctrlPanel);
    ctrlLay->setContentsMargins(12, 8, 12, 10);
    ctrlLay->setSpacing(4);

    QHBoxLayout* ctrlHdr = new QHBoxLayout;
    QLabel* ctrlTitle = new QLabel(tr("CONTROL BOARD"));
    ctrlTitle->setFont(secFont);
    ctrlTitle->setStyleSheet("color:#8FB4C4; background:transparent; letter-spacing:1px;");
    QLabel* ctrlHint = new QLabel(tr("Communication status"));
    ctrlHint->setFont(smallFont);
    ctrlHint->setStyleSheet("color:#4B6B7C; background:transparent;");
    ctrlHdr->addWidget(ctrlTitle);
    ctrlHdr->addStretch();
    ctrlHdr->addWidget(ctrlHint);
    ctrlLay->addLayout(ctrlHdr);

    m_ctrlMatrix = new StateMatrixWidget;
    m_ctrlMatrix->setFont(baseFont);
    ctrlLay->addWidget(m_ctrlMatrix, 1);

    mainRow->addWidget(matrixPanel, 2);
    mainRow->addWidget(ctrlPanel, 1);
    rootLayout->addLayout(mainRow, 1);

    /* ================= 底部：按钮 ================= */
    QHBoxLayout* footerRow = new QHBoxLayout;
    footerRow->setSpacing(14);
    footerRow->addStretch(1);

    /* 按钮 */
    rechekBtn = new myPushButton(myLan.recheck,
                                 QIcon(stateResPath("ico_refresh.png")), true, true, this);
    nextBtn = new myPushButton(myLan.enter_system,
                               QIcon(stateResPath("ico_arrow_right.png")), true, true, this);

    if (struCnfg.nLang == LANG_TURKEY)
    {
        rechekBtn->setButtonFont(config->getFont(FONT_SIZE_16));
        nextBtn->setButtonFont(config->getFont(FONT_SIZE_16));
    }

    rechekBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    nextBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    rechekBtn->setIconSize(QSize(16, 16));
    nextBtn->setIconSize(QSize(16, 16));

    /* 优先使用 state_page 素材，缺失时沿用方案管理页按钮样式 */
    const QString refreshImg = stateResPath("btn_refresh_normal.png");
    const QString refreshImgP = stateResPath("btn_refresh_pressed.png");
    const QString entryImg = stateResPath("btn_entry_normal.png");
    const QString entryImgP = stateResPath("btn_entry_pressed.png");

    if (!refreshImg.isEmpty() && !refreshImgP.isEmpty())
    {
        rechekBtn->setStyleSheet(QString(
            "QPushButton { border-width:12px 24px 12px 24px; border-style:solid;"
            " border-color:transparent; border-image:url(%1) 12 24 12 24 stretch stretch;"
            " color:#DCF3FA; font-size:14px; }"
            "QPushButton:pressed { border-image:url(%2) 12 24 12 24 stretch stretch;"
            " color:#FFFFFF; }").arg(refreshImg, refreshImgP));
    }
    else
    {
        rechekBtn->setStyleSheet(
            "QPushButton { border-width:8px 16px 8px 16px; border-style:solid;"
            " border-color:transparent;"
            " border-image:url(:/res/png2/button/btn_normal.png) 8 16 8 16 stretch stretch;"
            " color:#E8F6FF; font-size:14px; }"
            "QPushButton:pressed {"
            " border-image:url(:/res/png2/button/btn_pressed.png) 8 16 8 16 stretch stretch; }");
    }

    if (!entryImg.isEmpty() && !entryImgP.isEmpty())
    {
        nextBtn->setStyleSheet(QString(
            "QPushButton { border-width:12px 24px 12px 24px; border-style:solid;"
            " border-color:transparent; border-image:url(%1) 12 24 12 24 stretch stretch;"
            " color:#DCF3FA; font-size:14px; }"
            "QPushButton:pressed { border-image:url(%2) 12 24 12 24 stretch stretch;"
            " color:#FFFFFF; }").arg(entryImg, entryImgP));
    }
    else
    {
        nextBtn->setStyleSheet(
            "QPushButton { border-width:8px 16px 8px 16px; border-style:solid;"
            " border-color:transparent;"
            " border-image:url(:/res/png2/button/btn_normal.png) 8 16 8 16 stretch stretch;"
            " color:#E8F6FF; font-size:14px; }"
            "QPushButton:pressed {"
            " border-image:url(:/res/png2/button/btn_pressed.png) 8 16 8 16 stretch stretch; }");
    }

    footerRow->addWidget(rechekBtn);
    footerRow->addWidget(nextBtn);
    rootLayout->addLayout(footerRow);

    /*槽函数*/
    connect(rechekBtn, SIGNAL(clicked()), this, SLOT(onRecheckBtnClickedSlt()));
    connect(nextBtn, SIGNAL(clicked()), this, SLOT(onNextBtnClickedSlt()));
}

/*!
 * \brief 页面背景（state_page 素材，缺失时沿用系统默认背景）
 */
void communicationList::paintEvent(QPaintEvent*)
{
    const QPixmap* bg = stateResPixmap("bg_state.png");
    if (!bg || bg->isNull())
        return;

    QPainter p(this);
    /* 素材为无缝平铺纹理，按原尺寸平铺避免拉伸发虚 */
    for (int y = 0; y < height(); y += bg->height())
    {
        for (int x = 0; x < width(); x += bg->width())
        {
            p.drawPixmap(x, y, *bg);
        }
    }
}

/*!
 * \brief 创建顶部汇总卡片
 */
QWidget* communicationList::createSummaryCard(int index, const QString& title, const QColor& color)
{
    QFrame* card = new QFrame;
    card->setObjectName(QString("sumCard%1").arg(index));
    card->setStyleSheet(panelStyle(card->objectName(), "card_normal.png", "#2A5E70"));
    card->setMinimumHeight(62);
    card->setMaximumHeight(72);

    QVBoxLayout* lay = new QVBoxLayout(card);
    lay->setContentsMargins(12, 4, 12, 4);
    lay->setSpacing(0);

    QLabel* titleLbl = new QLabel(title);
    titleLbl->setFont(config->getFont(FONT_SIZE_12));
    titleLbl->setStyleSheet("color:#8FB4C4; background:transparent; letter-spacing:1px;");

    QHBoxLayout* valueRow = new QHBoxLayout;
    valueRow->setSpacing(6);

    m_sumValue[index] = new QLabel("00");
    QFont valueFont = config->getFont(DEFAULT_FONT_SIZE);
    valueFont.setBold(true);
    valueFont.setPixelSize(qMax(22, valueFont.pixelSize() + 12));
    m_sumValue[index]->setFont(valueFont);
    m_sumValue[index]->setStyleSheet(QString("color:%1; background:transparent;").arg(color.name()));

    m_sumTotal[index] = new QLabel("/ 00");
    m_sumTotal[index]->setFont(config->getFont(FONT_SIZE_12));
    m_sumTotal[index]->setStyleSheet("color:#64748B; background:transparent;");

    valueRow->addWidget(m_sumValue[index]);
    valueRow->addWidget(m_sumTotal[index], 0, Qt::AlignBottom);
    valueRow->addStretch();

    lay->addWidget(titleLbl);
    lay->addLayout(valueRow);
    return card;
}

/**
 * 重新检测
 */
void communicationList::onRecheckBtnClickedSlt()
{
    emit goToCommPageSig();
}
/**
 * 进入初始化页面
 */
void communicationList::onNextBtnClickedSlt()
{
    emit goToInitPageSig();
}

/************************************************************************************/
/***
 *   初始化页面
 */
namespace {
const int INIT_PAGE_W = LCD_WIDTH - 10;                       // 页面宽度
const int INIT_PAGE_H = LCD_HEIGHT - LCD_TITLE_HEIGHT - 20;   // 页面高度

const int RING_CX    = 236;     // 圆环中心
const int RING_CY    = 336;
const int RING_OUTER = 178;     // 外圈刻度半径
const int RING_DASH  = 140;     // 中圈虚线半径
const int RING_TRACK = 112;     // 进度轨道半径
const int RING_INNER = 100;     // 内圈实线半径

const int STEPS_X     = 486;    // 执行链路列表
const int STEPS_W     = 504;
const int STEP_ROW_Y0 = 96;
const int STEP_ROW_H  = 62;

const int FOOT_LINE_Y = INIT_PAGE_H - 52;   // 底部进度线
}

initialization::initialization(QWidget* parent) :
    QWidget(parent)
    , m_currentStep(0)
    , m_doneCount(0)
    , m_percent(0)
    , m_targetPercent(0)
    , m_pulseTick(0)
    , m_finishDelay(0)
    , m_finished(false)
{
    setFixedSize(INIT_PAGE_W, INIT_PAGE_H);

    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(33);
    connect(m_animTimer, SIGNAL(timeout()), this, SLOT(onAnimTickSlt()));
}

/***
 *   自动初始化，完成后跳到预热界面
 */
void initialization::startInitSlt()
{
    m_currentStep = 0;
    m_doneCount = 0;
    m_percent = 0;
    m_targetPercent = 0;
    m_pulseTick = 0;
    m_finishDelay = 0;
    m_finished = false;

    m_animTimer->start();
    update();

    /* 先显示 0% 再开始下发，避免一进页面就卡住 */
    QTimer::singleShot(200, this, SLOT(runNextStepSlt()));
}

/*!
 * \brief 顺序执行 5 个下发步骤，每步之间留出动画时间
 */
void initialization::runNextStepSlt()
{
    if (m_currentStep >= INIT_STEP_COUNT)
    {
        return;
    }

    const int idx = m_currentStep;
    switch (idx)
    {
    case 0:
        myFlow.initInterfaceTransMode(0);   // 下发图像传输模式
        break;
    case 1:
        myFlow.initCamera();                // 重置/下发全部相机参数
        break;
    case 2:
        myFlow.resetSortParams();           // 重置色选参数
        break;
    case 3:
        myFlow.resetControl();              // 重置控制板参数
        break;
    default:
        myFlow.resetAiModel();              // 重置AI模型参数
        break;
    }

    m_doneCount = idx + 1;
    m_currentStep = idx + 1;
    m_targetPercent = m_doneCount * 100.0 / INIT_STEP_COUNT;

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    update();

    if (m_currentStep < INIT_STEP_COUNT)
    {
        QTimer::singleShot(220, this, SLOT(runNextStepSlt()));
    }
}

/*!
 * \brief 动画节拍：百分比平滑逼近目标值、执行中图标脉动
 */
void initialization::onAnimTickSlt()
{
    ++m_pulseTick;

    if (m_percent < m_targetPercent)
    {
        m_percent += qMax<qreal>(0.7, (m_targetPercent - m_percent) * 0.12);
        if (m_percent > m_targetPercent)
        {
            m_percent = m_targetPercent;
        }

        update(RING_CX - RING_OUTER - 12, RING_CY - RING_OUTER - 12,
               (RING_OUTER + 12) * 2, (RING_OUTER + 12) * 2);
        update(0, FOOT_LINE_Y - 40, INIT_PAGE_W, INIT_PAGE_H - FOOT_LINE_Y + 40);
    }

    /* 执行中图标脉动，只刷新当前行 */
    if (m_currentStep < INIT_STEP_COUNT)
    {
        update(STEPS_X, STEP_ROW_Y0 + m_currentStep * STEP_ROW_H,
               STEPS_W, STEP_ROW_H);
    }

    /* 全部完成并等到 100% 后跳转预热页 */
    if (!m_finished && m_doneCount >= INIT_STEP_COUNT && m_percent >= 99.9)
    {
        if (++m_finishDelay > 12)
        {
            m_finished = true;
            m_animTimer->stop();
            emit goToPreheatPageSig();
        }
    }
}

QString initialization::stepText(int index)
{
    switch (index)
    {
    case 0:
        return myLan.init_step_trans;
    case 1:
        return myLan.init_step_camera;
    case 2:
        return myLan.init_step_sort;
    case 3:
        return myLan.init_step_control;
    case 4:
        return myLan.init_step_ai;
    default:
        return QString();
    }
}

void initialization::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    drawTitle(p);
    drawRing(p);
    drawSteps(p);
    drawFooter(p);
}

void initialization::drawTitle(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    p.fillRect(QRectF(24, 26, 3, 24), QColor(0x22, 0xD3, 0xEE));

    QFont font("Noto Sans SC");
    font.setPixelSize(22);
    font.setWeight(QFont::Medium);
    p.setFont(font);
    p.setPen(QColor(0xE8, 0xFA, 0xFF));
    p.drawText(QPointF(40, 46), myLan.msg_system_init);
}

void initialization::drawRing(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    const QPointF c(RING_CX, RING_CY);

    /* 外圈刻度：60 根，每 5 根加长 */
    p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 46), 1));
    for (int i = 0; i < 60; ++i)
    {
        const double a = i * 6.0 * M_PI / 180.0;
        const double r1 = RING_OUTER - ((i % 5 == 0) ? 10 : 5);
        p.drawLine(QPointF(c.x() + r1 * qCos(a), c.y() + r1 * qSin(a)),
                   QPointF(c.x() + RING_OUTER * qCos(a), c.y() + RING_OUTER * qSin(a)));
    }

    /* 中圈虚线：28 段 */
    const QRectF dashRc(c.x() - RING_DASH, c.y() - RING_DASH,
                        RING_DASH * 2, RING_DASH * 2);
    p.setPen(QPen(QColor(0x2A, 0x7E, 0x93, 140), 1.5));
    for (int i = 0; i < 28; ++i)
    {
        const double start = i * (360.0 / 28.0) + 1.5;
        p.drawArc(dashRc, int(start * 16), int((360.0 / 28.0 - 7.0) * 16));
    }

    /* 内圈实线 */
    p.setPen(QPen(QColor(0x1B, 0x41, 0x52), 1));
    p.drawEllipse(c, RING_INNER, RING_INNER);

    /* 进度轨道 */
    p.setPen(QPen(QColor(0x12, 0x30, 0x3C), 4, Qt::SolidLine, Qt::FlatCap));
    p.drawEllipse(c, RING_TRACK, RING_TRACK);

    /* 进度弧：从 6 点钟方向顺时针 */
    if (m_percent > 0.01)
    {
        const QRectF trackRc(c.x() - RING_TRACK, c.y() - RING_TRACK,
                             RING_TRACK * 2, RING_TRACK * 2);
        p.setPen(QPen(QColor(0x22, 0xD3, 0xEE), 5, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(trackRc, 270 * 16, int(-m_percent * 360.0 * 16.0));
    }

    /* 中心百分比 */
    QFont pctFont("Noto Sans SC");
    pctFont.setPixelSize(42);
    pctFont.setBold(true);
    p.setFont(pctFont);
    p.setPen(QColor(0xE8, 0xFA, 0xFF));

    const QString pct = QString("%1%").arg(int(m_percent + 0.5));
    QFontMetrics pctFm(pctFont);
    p.drawText(QPointF(c.x() - pctFm.horizontalAdvance(pct) / 2.0, c.y() + 8), pct);

    /* 中心状态词（跟随语言，不与英文同时出现） */
    QFont statusFont("Noto Sans SC");
    statusFont.setPixelSize(12);
    p.setFont(statusFont);
    p.setPen(QColor(0x4E, 0x7A, 0x8C));

    QFontMetrics statusFm(statusFont);
    p.drawText(QPointF(c.x() - statusFm.horizontalAdvance(myLan.init_doing) / 2.0,
                       c.y() + 44), myLan.init_doing);

    p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 90), 1));
    p.drawLine(QPointF(c.x() - 15, c.y() + 58), QPointF(c.x() + 15, c.y() + 58));
}

void initialization::drawSteps(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    /* 标题 */
    QFont headFont("Noto Sans SC");
    headFont.setPixelSize(14);
    p.setFont(headFont);
    p.setPen(QColor(0x4E, 0x7A, 0x8C));
    p.drawText(QPointF(STEPS_X, 66), myLan.init_link);

    p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 40), 1));
    p.drawLine(QPointF(STEPS_X, 78), QPointF(STEPS_X + STEPS_W, 78));

    for (int i = 0; i < INIT_STEP_COUNT; ++i)
    {
        const QRectF rowRc(STEPS_X, STEP_ROW_Y0 + i * STEP_ROW_H,
                           STEPS_W, STEP_ROW_H - 8);

        /* 0待执行 1执行中 2已完成 */
        const int state = (i < m_doneCount) ? 2 : ((i == m_currentStep) ? 1 : 0);

        if (state == 1)
        {
            p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, 90), 1));
            p.setBrush(QColor(0x22, 0xD3, 0xEE, 20));
            p.drawRect(rowRc);
            p.fillRect(QRectF(rowRc.left(), rowRc.top(), 3, rowRc.height()),
                       QColor(0x22, 0xD3, 0xEE));
        }
        else
        {
            p.setPen(QPen(QColor(255, 255, 255, 16), 1));
            p.drawLine(QPointF(rowRc.left(), rowRc.bottom() + 4),
                       QPointF(rowRc.right(), rowRc.bottom() + 4));
        }

        /* 步骤编号 */
        QFont numFont("Noto Sans SC");
        numFont.setPixelSize(14);
        numFont.setBold(state == 1);
        p.setFont(numFont);
        p.setPen(state == 1 ? QColor(0x22, 0xD3, 0xEE) : QColor(0x3E, 0x62, 0x72));
        p.drawText(QPointF(rowRc.left() + 14, rowRc.center().y() + 5),
                   QString("%1").arg(i + 1, 2, 10, QChar('0')));

        /* 步骤名称 */
        QFont nameFont("Noto Sans SC");
        nameFont.setPixelSize(16);
        p.setFont(nameFont);
        if (state == 1)
        {
            p.setPen(QColor(0xE8, 0xFA, 0xFF));
        }
        else if (state == 2)
        {
            p.setPen(QColor(0x5E, 0x86, 0x96));
        }
        else
        {
            p.setPen(QColor(0x6B, 0x8A, 0x99));
        }
        p.drawText(QPointF(rowRc.left() + 52, rowRc.center().y() + 6),
                   stepText(i));

        /* 右侧状态图标 */
        drawStateIcon(p, int(rowRc.right()) - 18, int(rowRc.center().y()), state);
    }
}

void initialization::drawStateIcon(QPainter& p, int cx, int cy, int state)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    if (state == 2)
    {
        /* 完成：绿色圆形打勾 */
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x3D, 0xDC, 0x97));
        p.drawEllipse(QPointF(cx, cy), 8, 8);

        p.setPen(QPen(QColor(0x0B, 0x1A, 0x24), 2, Qt::SolidLine,
                      Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(QPointF(cx - 3.5, cy + 0.5), QPointF(cx - 1, cy + 3));
        p.drawLine(QPointF(cx - 1, cy + 3), QPointF(cx + 4, cy - 3));
    }
    else if (state == 1)
    {
        /* 执行中：圆环 + 中心点，按节拍脉动 */
        const int alpha = 130 + int(120 * qAbs(qSin(m_pulseTick * 0.09)));
        p.setPen(QPen(QColor(0x22, 0xD3, 0xEE, alpha), 1.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(cx, cy), 8, 8);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x22, 0xD3, 0xEE));
        p.drawEllipse(QPointF(cx, cy), 3, 3);
    }
    else
    {
        /* 待执行：空心圆 */
        p.setPen(QPen(QColor(0x2A, 0x4E, 0x5E), 1.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(cx, cy), 7, 7);
    }
}

void initialization::drawFooter(QPainter& p)
{
    p.setRenderHint(QPainter::Antialiasing, true);

    QFont font("Noto Sans SC");
    font.setPixelSize(14);
    p.setFont(font);
    p.setPen(QColor(0x4E, 0x7A, 0x8C));

    /* 当前执行项（只在初始化过程中显示说明文字） */
    if (m_doneCount > 0 && m_doneCount <= INIT_STEP_COUNT)
    {
        const QString running = myLan.init_running + ": "
                + stepText(m_doneCount - 1);
        p.drawText(QPointF(24, FOOT_LINE_Y - 22), running);
    }

    /* 进度线 */
    const qreal lineX0 = 24;
    const qreal lineX1 = INIT_PAGE_W - 24;
    p.setPen(QPen(QColor(0x12, 0x30, 0x3C), 2));
    p.drawLine(QPointF(lineX0, FOOT_LINE_Y), QPointF(lineX1, FOOT_LINE_Y));

    if (m_percent > 0.01)
    {
        p.setPen(QPen(QColor(0x22, 0xD3, 0xEE), 2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(lineX0, FOOT_LINE_Y),
                   QPointF(lineX0 + (lineX1 - lineX0) * m_percent / 100.0,
                           FOOT_LINE_Y));
    }

    /* 左下角说明 + 右下角百分比 */
    QFont smallFont("Noto Sans SC");
    smallFont.setPixelSize(12);
    p.setFont(smallFont);
    p.setPen(QColor(0x3E, 0x62, 0x72));
    p.drawText(QPointF(lineX0, FOOT_LINE_Y + 30), myLan.system_initial);

    const QString pct = QString("%1%").arg(int(m_percent + 0.5));
    QFontMetrics fm(smallFont);
    p.drawText(QPointF(lineX1 - fm.horizontalAdvance(pct), FOOT_LINE_Y + 30), pct);
}

/**************************************************************************************/
/***
 *   预热页面控件
 */
preheat::preheat(QWidget* parent) :
    QWidget(parent)
{
    bFlagSkip = false;
    waitCommLabel = new myLabel(myLan.preheat, this);
    waitCommLabel->setGeometry(QRect(20, 200, 500, 40));
    skipBtn = new myPushButton(myLan.skip_preheat, myIcon.Edit_Redo, this);
    skipBtn->setGeometry(QRect(LCD_WIDTH - BTN_WIDTH - 20, LCD_CONTENT_HEIGTH - 50, BTN_WIDTH, 40));
    progressBar = new QProgressBar(this);
    progressBar->setGeometry(20, 260, LCD_WIDTH - 40, 40);
    progressBar->setMaximum(900);
    progressBar->setValue(-1);
    progressBar->setMinimumHeight(BTN_HEIGHT);

    /*尺寸限制*/
    skipBtn->setMaximumSize(QSize(BTN_WIDTH, BTN_HEIGHT));

    /*布局管理*/
    preheatGridLayout = new QGridLayout(this);
    preheatGridLayout->setVerticalSpacing((LCD_CONTENT_HEIGTH - 3 * BTN_HEIGHT) / 2);
    preheatGridLayout->addWidget(waitCommLabel, 0, 0, 1, 1);
    preheatGridLayout->addWidget(progressBar, 1, 0, 1, 1);

    preheatHBLayout = new QHBoxLayout();
    horizontalSpacer = new QSpacerItem(10, BTN_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    preheatHBLayout->addItem(horizontalSpacer);
    preheatHBLayout->addWidget(skipBtn);
    preheatGridLayout->addLayout(preheatHBLayout, 2, 0, 1, 1);

    /*槽函数*/
    connect(skipBtn, SIGNAL(pressed()), this, SLOT(onSkipBtnClickSlt()));
}

/***
 *   直接跳到预热页面
 */
void preheat::onSkipBtnClickSlt()
{
    bFlagSkip = true;
}

/***
 *   自动预热页面
 */
void preheat::startPreheatSlt()
{
    progressBar->setRange(0, 900);
    for (int i = 1; i <= 1; i++)
    {
        progressBar->setValue(i);
        if (bFlagSkip)
        {
            break;
        }
        QTime dieTime = QTime::currentTime().addSecs(1);
        while (QTime::currentTime() < dieTime)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }

        if (bFlagSkip)
        {
            break;
        }
    }
    emit goToHomePageSig();
}
