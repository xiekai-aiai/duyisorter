// merge_dialog.h — 合并标注目录对话框（同步阻塞，独占模式）
#ifndef MERGE_DIALOG_H
#define MERGE_DIALOG_H

#include <QDialog>
#include <QVector>
#include <QStringList>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFrame>

constexpr const char* DEFAULT_IMAGE_ROOT = "/home/mcgs/Desktop/data";
constexpr int MAX_SOURCE_DIRS = 10;

class MergeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MergeDialog(QWidget* parent = nullptr);
    ~MergeDialog();

private slots:
    void onAddSourceClicked();
    void onBrowseSource(QLineEdit* pathEdit);
    void onRemoveSource(QWidget* rowWidget);
    void onBrowseTarget();
    void onStartMerge();
    void onSourcePathChanged();
    void onModeChanged();

private:
    void initUi();
    void renumberSources();
    void refreshTargetName();
    QStringList collectSourceDirs() const;

    // UI 控件
    QScrollArea*    m_srcScroll      = nullptr;
    QWidget*        m_srcContainer   = nullptr;
    QVBoxLayout*    m_srcListLayout  = nullptr;
    QPushButton*    m_btnAddSource   = nullptr;

    QLineEdit*      m_tgtPathEdit     = nullptr;

    QRadioButton*   m_radioAll       = nullptr;
    QRadioButton*   m_radioLabeled    = nullptr;

    // 进度提示（一个 QLabel 显示文本汇总，替代所有 QProgressBar）
    QLabel*         m_progressLabel  = nullptr;

    QPushButton*    m_btnMerge        = nullptr;
    QPushButton*    m_btnClose        = nullptr;

    struct SourceRow {
        QWidget*   row    = nullptr;
        QLineEdit* path   = nullptr;
    };
    QVector<SourceRow> m_sourceRows;

    bool m_merging = false;
};

#endif // MERGE_DIALOG_H
