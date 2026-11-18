/*!
 * \file        mytoast.h
 * \brief       轻量提示条：父窗口顶部居中显示「✓ 文案」，停留后淡出
 * \date        2026.11.12
 */
#ifndef MYTOAST_H
#define MYTOAST_H

#include <QWidget>
#include <QPixmap>
#include <QString>

class QTimer;
class QPropertyAnimation;
class QGraphicsOpacityEffect;

/*!
 * \brief 顶部提示条控件，不参与布局、不拦截鼠标
 */
class myToast : public QWidget
{
    Q_OBJECT

public:
    explicit myToast(QWidget *parent = nullptr);

    /*!
     * \brief 弹出提示
     * \param text 提示文案
     */
    void showToast(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_bg;                       //! toast 底板（9-slice）
    QPixmap m_icon;                     //! 绿色对勾
    QString m_text;                     //! 提示文案
    QTimer *m_holdTimer;                //! 停留计时
    QGraphicsOpacityEffect *m_effect;   //! 淡出效果
    QPropertyAnimation *m_fade;         //! 淡出动画
};

#endif // MYTOAST_H
