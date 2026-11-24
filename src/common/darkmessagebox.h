/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        darkmessagebox.h
 * \brief       告警/提示框（QMessageBox）深色配色 + 深色标题栏（头文件实现，无需 moc）
 * \date        2026.11.21
 */
#ifndef DARKMESSAGEBOX_H
#define DARKMESSAGEBOX_H

#include <QMessageBox>
#include <QEvent>

#include "darktitlebar.h"

/*!
 * \brief 全局安装后，所有 QMessageBox（warning/information/question/critical）
 *        自动套用与其它弹窗一致的深色配色和深色标题栏。
 * \note  在 Polish（窗口创建之前）阶段改造，不隐藏、不重新 show，
 *        因此不会影响 exec() 的模态显示。
 */
class DarkMessageBoxFilter : public QObject
{
public:
    explicit DarkMessageBoxFilter(QObject* parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override
    {
        QMessageBox* box = qobject_cast<QMessageBox*>(obj);
        if (!box)
        {
            return QObject::eventFilter(obj, ev);
        }

        // Polish 在窗口首次显示、创建原生窗口之前触发，此时改窗口标志是安全的
        if (ev->type() == QEvent::Polish && !box->property("_darkMsgStyled").toBool())
        {
            box->setProperty("_darkMsgStyled", true);

            box->setStyleSheet(
                "QMessageBox { background: #0c161c; }"
                "QMessageBox QLabel { color: #cfe3ea; font-size: 16px; background: transparent; }"
                "QMessageBox QPushButton { background: #0d1a21; border: 1px solid #1e3a4a;"
                "  color: #cfe3ea; padding: 6px 20px; border-radius: 4px;"
                "  font-size: 16px; min-width: 86px; }"
                "QMessageBox QPushButton:hover { border-color: #155e6b; color: #22d3ee; }"
                "QMessageBox QPushButton:pressed { background: #12303c; }");

            // 深色自绘标题栏（此时窗口尚未创建，无需重新 show）
            DarkTitleBar::apply(box);
        }

        return QObject::eventFilter(obj, ev);
    }
};

#endif // DARKMESSAGEBOX_H
