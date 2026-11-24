/*!
 * \Copyright   Copyright (C) 2013 Hefei Duyi Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        darktitlebar.h
 * \brief       弹窗深色自绘标题栏（头文件实现，无需 moc）
 * \date        2026.11.21
 */
#ifndef DARKTITLEBAR_H
#define DARKTITLEBAR_H

#include <QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>

/*!
 * \brief 给使用系统标题栏的弹窗套上深色自绘标题栏
 *
 * 用法：弹窗 UI 与尺寸都设置完成后、exec()/show() 之前调用
 *      DarkTitleBar::apply(&dlg);
 * 注意：数字键盘、全键盘等输入弹窗不要调用。
 */
class DarkTitleBar : public QWidget
{
public:
    enum { BarHeight = 40 };

    static void apply(QDialog* dlg)
    {
        if (!dlg || !dlg->layout() || dlg->property("_darkTitleApplied").toBool())
        {
            return;
        }
        dlg->setProperty("_darkTitleApplied", true);

        const bool explicitSize = dlg->testAttribute(Qt::WA_Resized);

        // 去掉系统标题栏
        dlg->setWindowFlags((dlg->windowFlags()
                             & ~(Qt::WindowTitleHint | Qt::CustomizeWindowHint))
                            | Qt::FramelessWindowHint);

        // 顶部让出标题栏高度
        QLayout* lay = dlg->layout();
        QMargins m = lay->contentsMargins();
        lay->setContentsMargins(m.left(), m.top() + BarHeight, m.right(), m.bottom());

        DarkTitleBar* bar = new DarkTitleBar(dlg);
        if (explicitSize)
        {
            dlg->resize(dlg->width(), dlg->height() + BarHeight);
        }
        else
        {
            dlg->adjustSize();
        }
        bar->setGeometry(0, 0, dlg->width(), BarHeight);
        bar->raise();
        bar->show();

        // 布局在显示后才稳定，延迟一次同步标题栏宽度
        QTimer::singleShot(0, bar, [bar, dlg]() {
            bar->setGeometry(0, 0, dlg->width(), BarHeight);
            bar->raise();
        });
    }

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override
    {
        if (obj == m_dlg)
        {
            if (ev->type() == QEvent::Resize || ev->type() == QEvent::Show)
            {
                setGeometry(0, 0, m_dlg->width(), BarHeight);
                raise();
            }
            else if (ev->type() == QEvent::WindowTitleChange)
            {
                m_title->setText(m_dlg->windowTitle());
            }
        }
        else if (obj == m_close)
        {
            if (ev->type() == QEvent::MouseButtonPress)
            {
                m_close->setStyleSheet(closeStyle(true));
            }
            else if (ev->type() == QEvent::MouseButtonRelease)
            {
                m_close->setStyleSheet(closeStyle(false));
                QMouseEvent* me = static_cast<QMouseEvent*>(ev);
                if (me->button() == Qt::LeftButton && m_close->rect().contains(me->pos()))
                {
                    m_dlg->reject();
                }
                return true;
            }
        }
        return QWidget::eventFilter(obj, ev);
    }

private:
    explicit DarkTitleBar(QDialog* dlg)
        : QWidget(dlg), m_dlg(dlg)
    {
        setObjectName("_darkTitleBar");
        setFixedHeight(BarHeight);
        setStyleSheet("QWidget#_darkTitleBar {"
                      "  background: #0c161c;"
                      "  border-bottom: 1px solid #16303b;"
                      "}");

        QHBoxLayout* lay = new QHBoxLayout(this);
        lay->setContentsMargins(14, 0, 0, 0);
        lay->setSpacing(0);

        m_title = new QLabel(dlg->windowTitle(), this);
        m_title->setStyleSheet("background: transparent; color: #ffffff;"
                               " font-size: 16px; font-weight: 600;");

        // × 用 QLabel 右对齐绘制，点击区域仍保留 56×36
        m_close = new QLabel(QString::fromUtf8("×"), this);
        m_close->setFixedSize(56, 36);
        m_close->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_close->setStyleSheet(closeStyle(false));
        m_close->setCursor(Qt::PointingHandCursor);
        m_close->installEventFilter(this);

        lay->addWidget(m_title);
        lay->addStretch();
        lay->addWidget(m_close);

        dlg->installEventFilter(this);
    }

    static QString closeStyle(bool pressed)
    {
        if (pressed)
        {
            return QString("QLabel { background: #7f1d1d; color: #ffffff;"
                           " font-size: 34px; font-weight: 700;"
                           " padding-right: 8px; border-radius: 4px; }");
        }
        return QString("QLabel { background: transparent; color: #cfe3ea;"
                       " font-size: 34px; font-weight: 700;"
                       " padding-right: 8px; }");
    }

    QDialog* m_dlg;
    QLabel* m_title;
    QLabel* m_close;
};

#endif // DARKTITLEBAR_H
