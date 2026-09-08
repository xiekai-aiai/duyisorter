/********************************************************************************
** Form generated from reading UI file 'aimodelset.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AIMODELSET_H
#define UI_AIMODELSET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AiModelSet
{
public:
    QLabel *imgLabel;
    QLabel *modelNameLabel;
    QCheckBox *a_checkBox;
    QCheckBox *b_checkBox;
    QCheckBox *c_checkBox;
    QLabel *tipLabel;
    QPushButton *modelTrainPushButton;
    QPushButton *modelSelPushButton;
    QPushButton *simuPushButton;
    QPushButton *modelTrainPushButton_2;
    QPushButton *modelTrainPushButton_3;
    QPushButton *addTrainListPushButton;
    QPushButton *annotationPushButton;
    QPushButton *erasePushButton;
    QLabel *preImageNumLabel;
    QPushButton *importImgPushButton;
    QPushButton *modelTrainPushButton_6;
    QPushButton *modelTrainPushButton_7;
    QPushButton *modelTrainPushButton_8;
    QPushButton *m_prevBtn;
    QPushButton *m_nextBtn;
    QLabel *m_curPageInfoLabel;
    QLineEdit *m_curPagelineEdit;
    QLabel *modelNameLabel_4;
    QPushButton *backPushButton;
    QLabel *m_pageInfoLabel;
    QWidget *modeInfowidget;
    QLabel *modelInfoLabel;
    QLabel *modelNameInputLabel;
    QLabel *modelSizeLabel;
    QCheckBox *modelsize1CheckBox;
    QCheckBox *modelsize2CheckBox;
    QPushButton *okPushButton;
    QPushButton *addPushButton;
    QTextEdit *modelNameLineEdit;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QListWidget *m_imageList;
    QLabel *annotationInfoEdit;
    QPushButton *delTrainListPushButton;
    QPushButton *fgPushButton;
    QPushButton *selectPushButton;

    void setupUi(QWidget *AiModelSet)
    {
        if (AiModelSet->objectName().isEmpty())
            AiModelSet->setObjectName(QStringLiteral("AiModelSet"));
        AiModelSet->resize(1024, 768);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AiModelSet->sizePolicy().hasHeightForWidth());
        AiModelSet->setSizePolicy(sizePolicy);
        AiModelSet->setMinimumSize(QSize(1024, 768));
        AiModelSet->setMaximumSize(QSize(1024, 768));
        QFont font;
        font.setFamily(QStringLiteral("HarmonyOS Sans Medium"));
        AiModelSet->setFont(font);
        imgLabel = new QLabel(AiModelSet);
        imgLabel->setObjectName(QStringLiteral("imgLabel"));
        imgLabel->setGeometry(QRect(0, 160, 1020, 64));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(imgLabel->sizePolicy().hasHeightForWidth());
        imgLabel->setSizePolicy(sizePolicy1);
        imgLabel->setMinimumSize(QSize(1020, 64));
        imgLabel->setMaximumSize(QSize(1020, 64));
        QFont font1;
        font1.setFamily(QStringLiteral("HarmonyOS Sans Medium"));
        font1.setPointSize(8);
        imgLabel->setFont(font1);
        modelNameLabel = new QLabel(AiModelSet);
        modelNameLabel->setObjectName(QStringLiteral("modelNameLabel"));
        modelNameLabel->setGeometry(QRect(20, 40, 281, 41));
        modelNameLabel->setFont(font1);
        modelNameLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelNameLabel->setScaledContents(false);
        modelNameLabel->setIndent(-1);
        a_checkBox = new QCheckBox(AiModelSet);
        a_checkBox->setObjectName(QStringLiteral("a_checkBox"));
        a_checkBox->setGeometry(QRect(310, 40, 60, 40));
        sizePolicy1.setHeightForWidth(a_checkBox->sizePolicy().hasHeightForWidth());
        a_checkBox->setSizePolicy(sizePolicy1);
        a_checkBox->setMinimumSize(QSize(60, 40));
        a_checkBox->setMaximumSize(QSize(60, 40));
        a_checkBox->setFont(font1);
        b_checkBox = new QCheckBox(AiModelSet);
        b_checkBox->setObjectName(QStringLiteral("b_checkBox"));
        b_checkBox->setGeometry(QRect(380, 40, 60, 40));
        QFont font2;
        font2.setFamily(QStringLiteral("HarmonyOS Sans Medium"));
        font2.setPointSize(8);
        font2.setBold(false);
        font2.setWeight(50);
        b_checkBox->setFont(font2);
        c_checkBox = new QCheckBox(AiModelSet);
        c_checkBox->setObjectName(QStringLiteral("c_checkBox"));
        c_checkBox->setGeometry(QRect(440, 40, 60, 40));
        c_checkBox->setFont(font1);
        tipLabel = new QLabel(AiModelSet);
        tipLabel->setObjectName(QStringLiteral("tipLabel"));
        tipLabel->setGeometry(QRect(20, 100, 891, 31));
        QFont font3;
        font3.setFamily(QStringLiteral("HarmonyOS Sans Medium"));
        font3.setPointSize(7);
        tipLabel->setFont(font3);
        tipLabel->setIndent(-1);
        modelTrainPushButton = new QPushButton(AiModelSet);
        modelTrainPushButton->setObjectName(QStringLiteral("modelTrainPushButton"));
        modelTrainPushButton->setGeometry(QRect(720, 30, 141, 51));
        modelTrainPushButton->setFont(font1);
        modelTrainPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        modelSelPushButton = new QPushButton(AiModelSet);
        modelSelPushButton->setObjectName(QStringLiteral("modelSelPushButton"));
        modelSelPushButton->setGeometry(QRect(870, 30, 141, 51));
        modelSelPushButton->setFont(font1);
        modelSelPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        simuPushButton = new QPushButton(AiModelSet);
        simuPushButton->setObjectName(QStringLiteral("simuPushButton"));
        simuPushButton->setGeometry(QRect(610, 30, 91, 51));
        simuPushButton->setFont(font1);
        simuPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        modelTrainPushButton_2 = new QPushButton(AiModelSet);
        modelTrainPushButton_2->setObjectName(QStringLiteral("modelTrainPushButton_2"));
        modelTrainPushButton_2->setGeometry(QRect(200, 410, 111, 51));
        modelTrainPushButton_2->setFont(font1);
        modelTrainPushButton_2->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        modelTrainPushButton_3 = new QPushButton(AiModelSet);
        modelTrainPushButton_3->setObjectName(QStringLiteral("modelTrainPushButton_3"));
        modelTrainPushButton_3->setGeometry(QRect(320, 410, 111, 51));
        modelTrainPushButton_3->setFont(font1);
        modelTrainPushButton_3->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        addTrainListPushButton = new QPushButton(AiModelSet);
        addTrainListPushButton->setObjectName(QStringLiteral("addTrainListPushButton"));
        addTrainListPushButton->setGeometry(QRect(500, 410, 181, 51));
        addTrainListPushButton->setFont(font1);
        addTrainListPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        annotationPushButton = new QPushButton(AiModelSet);
        annotationPushButton->setObjectName(QStringLiteral("annotationPushButton"));
        annotationPushButton->setGeometry(QRect(690, 410, 91, 51));
        annotationPushButton->setFont(font1);
        annotationPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        erasePushButton = new QPushButton(AiModelSet);
        erasePushButton->setObjectName(QStringLiteral("erasePushButton"));
        erasePushButton->setGeometry(QRect(790, 410, 91, 51));
        erasePushButton->setFont(font1);
        erasePushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        preImageNumLabel = new QLabel(AiModelSet);
        preImageNumLabel->setObjectName(QStringLiteral("preImageNumLabel"));
        preImageNumLabel->setGeometry(QRect(920, 410, 91, 51));
        preImageNumLabel->setFont(font1);
        preImageNumLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        preImageNumLabel->setScaledContents(false);
        preImageNumLabel->setAlignment(Qt::AlignCenter);
        preImageNumLabel->setIndent(-1);
        importImgPushButton = new QPushButton(AiModelSet);
        importImgPushButton->setObjectName(QStringLiteral("importImgPushButton"));
        importImgPushButton->setGeometry(QRect(10, 600, 141, 51));
        importImgPushButton->setFont(font1);
        importImgPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        modelTrainPushButton_6 = new QPushButton(AiModelSet);
        modelTrainPushButton_6->setObjectName(QStringLiteral("modelTrainPushButton_6"));
        modelTrainPushButton_6->setGeometry(QRect(160, 600, 111, 51));
        modelTrainPushButton_6->setFont(font1);
        modelTrainPushButton_6->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        modelTrainPushButton_7 = new QPushButton(AiModelSet);
        modelTrainPushButton_7->setObjectName(QStringLiteral("modelTrainPushButton_7"));
        modelTrainPushButton_7->setGeometry(QRect(280, 600, 111, 51));
        modelTrainPushButton_7->setFont(font1);
        modelTrainPushButton_7->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        modelTrainPushButton_8 = new QPushButton(AiModelSet);
        modelTrainPushButton_8->setObjectName(QStringLiteral("modelTrainPushButton_8"));
        modelTrainPushButton_8->setGeometry(QRect(400, 600, 91, 51));
        modelTrainPushButton_8->setFont(font1);
        modelTrainPushButton_8->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        m_prevBtn = new QPushButton(AiModelSet);
        m_prevBtn->setObjectName(QStringLiteral("m_prevBtn"));
        m_prevBtn->setGeometry(QRect(600, 600, 81, 51));
        m_prevBtn->setFont(font1);
        m_prevBtn->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        m_nextBtn = new QPushButton(AiModelSet);
        m_nextBtn->setObjectName(QStringLiteral("m_nextBtn"));
        m_nextBtn->setGeometry(QRect(700, 600, 81, 51));
        m_nextBtn->setFont(font1);
        m_nextBtn->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        m_curPageInfoLabel = new QLabel(AiModelSet);
        m_curPageInfoLabel->setObjectName(QStringLiteral("m_curPageInfoLabel"));
        m_curPageInfoLabel->setGeometry(QRect(800, 600, 131, 21));
        QFont font4;
        font4.setFamily(QStringLiteral("HarmonyOS Sans Medium"));
        font4.setPointSize(6);
        m_curPageInfoLabel->setFont(font4);
        m_curPageInfoLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        m_curPageInfoLabel->setScaledContents(false);
        m_curPageInfoLabel->setAlignment(Qt::AlignCenter);
        m_curPageInfoLabel->setIndent(-1);
        m_curPagelineEdit = new QLineEdit(AiModelSet);
        m_curPagelineEdit->setObjectName(QStringLiteral("m_curPagelineEdit"));
        m_curPagelineEdit->setGeometry(QRect(940, 610, 41, 31));
        m_curPagelineEdit->setAlignment(Qt::AlignCenter);
        modelNameLabel_4 = new QLabel(AiModelSet);
        modelNameLabel_4->setObjectName(QStringLiteral("modelNameLabel_4"));
        modelNameLabel_4->setGeometry(QRect(980, 600, 31, 51));
        modelNameLabel_4->setFont(font1);
        modelNameLabel_4->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelNameLabel_4->setScaledContents(false);
        modelNameLabel_4->setAlignment(Qt::AlignCenter);
        modelNameLabel_4->setIndent(-1);
        backPushButton = new QPushButton(AiModelSet);
        backPushButton->setObjectName(QStringLiteral("backPushButton"));
        backPushButton->setGeometry(QRect(840, 660, 161, 40));
        backPushButton->setFont(font1);
        backPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        m_pageInfoLabel = new QLabel(AiModelSet);
        m_pageInfoLabel->setObjectName(QStringLiteral("m_pageInfoLabel"));
        m_pageInfoLabel->setGeometry(QRect(800, 630, 131, 20));
        m_pageInfoLabel->setFont(font4);
        m_pageInfoLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        m_pageInfoLabel->setScaledContents(false);
        m_pageInfoLabel->setAlignment(Qt::AlignCenter);
        m_pageInfoLabel->setIndent(-1);
        modeInfowidget = new QWidget(AiModelSet);
        modeInfowidget->setObjectName(QStringLiteral("modeInfowidget"));
        modeInfowidget->setGeometry(QRect(160, 40, 701, 421));
        modelInfoLabel = new QLabel(modeInfowidget);
        modelInfoLabel->setObjectName(QStringLiteral("modelInfoLabel"));
        modelInfoLabel->setGeometry(QRect(250, 20, 211, 41));
        QFont font5;
        font5.setFamily(QStringLiteral("HarmonyOS Sans Medium"));
        font5.setPointSize(9);
        modelInfoLabel->setFont(font5);
        modelInfoLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelInfoLabel->setScaledContents(false);
        modelInfoLabel->setAlignment(Qt::AlignCenter);
        modelInfoLabel->setIndent(-1);
        modelNameInputLabel = new QLabel(modeInfowidget);
        modelNameInputLabel->setObjectName(QStringLiteral("modelNameInputLabel"));
        modelNameInputLabel->setGeometry(QRect(60, 100, 131, 41));
        modelNameInputLabel->setFont(font5);
        modelNameInputLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelNameInputLabel->setScaledContents(false);
        modelNameInputLabel->setAlignment(Qt::AlignCenter);
        modelNameInputLabel->setIndent(-1);
        modelSizeLabel = new QLabel(modeInfowidget);
        modelSizeLabel->setObjectName(QStringLiteral("modelSizeLabel"));
        modelSizeLabel->setGeometry(QRect(60, 210, 131, 41));
        modelSizeLabel->setFont(font5);
        modelSizeLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelSizeLabel->setScaledContents(false);
        modelSizeLabel->setAlignment(Qt::AlignCenter);
        modelSizeLabel->setIndent(-1);
        modelsize1CheckBox = new QCheckBox(modeInfowidget);
        modelsize1CheckBox->setObjectName(QStringLiteral("modelsize1CheckBox"));
        modelsize1CheckBox->setGeometry(QRect(240, 210, 61, 40));
        QFont font6;
        font6.setFamily(QStringLiteral("HarmonyOS Sans Medium"));
        font6.setPointSize(9);
        font6.setBold(false);
        font6.setWeight(50);
        modelsize1CheckBox->setFont(font6);
        modelsize2CheckBox = new QCheckBox(modeInfowidget);
        modelsize2CheckBox->setObjectName(QStringLiteral("modelsize2CheckBox"));
        modelsize2CheckBox->setGeometry(QRect(360, 210, 60, 40));
        modelsize2CheckBox->setFont(font5);
        okPushButton = new QPushButton(modeInfowidget);
        okPushButton->setObjectName(QStringLiteral("okPushButton"));
        okPushButton->setGeometry(QRect(280, 360, 161, 41));
        okPushButton->setFont(font1);
        okPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        addPushButton = new QPushButton(modeInfowidget);
        addPushButton->setObjectName(QStringLiteral("addPushButton"));
        addPushButton->setGeometry(QRect(520, 100, 151, 41));
        addPushButton->setFont(font1);
        addPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        modelNameLineEdit = new QTextEdit(modeInfowidget);
        modelNameLineEdit->setObjectName(QStringLiteral("modelNameLineEdit"));
        modelNameLineEdit->setGeometry(QRect(200, 100, 301, 40));
        line = new QFrame(AiModelSet);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 80, 1021, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(AiModelSet);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(0, 380, 1021, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(AiModelSet);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(0, 580, 1021, 16));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        m_imageList = new QListWidget(AiModelSet);
        m_imageList->setObjectName(QStringLiteral("m_imageList"));
        m_imageList->setGeometry(QRect(40, 470, 911, 60));
        m_imageList->setMaximumSize(QSize(1025, 60));
        annotationInfoEdit = new QLabel(AiModelSet);
        annotationInfoEdit->setObjectName(QStringLiteral("annotationInfoEdit"));
        annotationInfoEdit->setGeometry(QRect(10, 280, 891, 31));
        annotationInfoEdit->setFont(font3);
        annotationInfoEdit->setIndent(-1);
        delTrainListPushButton = new QPushButton(AiModelSet);
        delTrainListPushButton->setObjectName(QStringLiteral("delTrainListPushButton"));
        delTrainListPushButton->setGeometry(QRect(880, 410, 81, 51));
        delTrainListPushButton->setFont(font1);
        delTrainListPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        fgPushButton = new QPushButton(AiModelSet);
        fgPushButton->setObjectName(QStringLiteral("fgPushButton"));
        fgPushButton->setGeometry(QRect(10, 410, 81, 51));
        fgPushButton->setFont(font1);
        fgPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));
        selectPushButton = new QPushButton(AiModelSet);
        selectPushButton->setObjectName(QStringLiteral("selectPushButton"));
        selectPushButton->setGeometry(QRect(100, 410, 81, 51));
        selectPushButton->setFont(font1);
        selectPushButton->setStyleSheet(QLatin1String("QPushButton {\n"
"	   text-align: center;\n"
"	   outline: none;\n"
"	   background-color: lightcyan;\n"
"	   border-color: #afafaf;\n"
"	   border-width: 2px;\n"
"	   border-style: solid;\n"
"	   border-radius: 5;\n"
"	   padding: 2px;}\n"
"\n"
"QPushButton:pressed {\n"
"	   padding-left: 5px;\n"
"	   padding-top: 5px;\n"
"	   background-color: lightgrey;}"));

        retranslateUi(AiModelSet);

        QMetaObject::connectSlotsByName(AiModelSet);
    } // setupUi

    void retranslateUi(QWidget *AiModelSet)
    {
        AiModelSet->setWindowTitle(QApplication::translate("AiModelSet", "Form", Q_NULLPTR));
        imgLabel->setText(QString());
        modelNameLabel->setText(QString());
        a_checkBox->setText(QApplication::translate("AiModelSet", " A", Q_NULLPTR));
        b_checkBox->setText(QApplication::translate("AiModelSet", "B", Q_NULLPTR));
        c_checkBox->setText(QApplication::translate("AiModelSet", "C", Q_NULLPTR));
        tipLabel->setText(QString());
        modelTrainPushButton->setText(QApplication::translate("AiModelSet", "\345\274\200\345\247\213\350\256\255\347\273\203", Q_NULLPTR));
        modelSelPushButton->setText(QApplication::translate("AiModelSet", "\346\250\241\345\236\213\345\212\240\350\275\275", Q_NULLPTR));
        simuPushButton->setText(QApplication::translate("AiModelSet", "\344\273\277\347\234\237", Q_NULLPTR));
        modelTrainPushButton_2->setText(QApplication::translate("AiModelSet", "\346\211\213\345\212\250\346\240\207\346\263\250", Q_NULLPTR));
        modelTrainPushButton_3->setText(QApplication::translate("AiModelSet", "\350\207\252\345\212\250\346\240\207\346\263\250", Q_NULLPTR));
        addTrainListPushButton->setText(QApplication::translate("AiModelSet", "+ \346\267\273\345\212\240\345\210\260\350\256\255\347\273\203\350\265\204\346\272\220", Q_NULLPTR));
        annotationPushButton->setText(QApplication::translate("AiModelSet", "\346\240\207\346\263\250", Q_NULLPTR));
        erasePushButton->setText(QApplication::translate("AiModelSet", "\346\223\246\351\231\244", Q_NULLPTR));
        preImageNumLabel->setText(QString());
        importImgPushButton->setText(QApplication::translate("AiModelSet", "\345\257\274\345\205\245\345\233\276\347\211\207\350\265\204\346\272\220", Q_NULLPTR));
        modelTrainPushButton_6->setText(QApplication::translate("AiModelSet", "\346\270\205\347\251\272\350\265\204\346\272\220", Q_NULLPTR));
        modelTrainPushButton_7->setText(QApplication::translate("AiModelSet", "\345\205\250\351\203\250\345\210\240\351\231\244", Q_NULLPTR));
        modelTrainPushButton_8->setText(QApplication::translate("AiModelSet", "\345\210\240\351\231\244", Q_NULLPTR));
        m_prevBtn->setText(QApplication::translate("AiModelSet", "\344\270\212\344\270\200\351\241\265", Q_NULLPTR));
        m_nextBtn->setText(QApplication::translate("AiModelSet", "\344\270\213\344\270\200\351\241\265", Q_NULLPTR));
        m_curPageInfoLabel->setText(QString());
        m_curPagelineEdit->setText(QApplication::translate("AiModelSet", "11", Q_NULLPTR));
        modelNameLabel_4->setText(QString());
        backPushButton->setText(QApplication::translate("AiModelSet", "\350\277\224\345\233\236", Q_NULLPTR));
        m_pageInfoLabel->setText(QString());
        modelInfoLabel->setText(QString());
        modelNameInputLabel->setText(QString());
        modelSizeLabel->setText(QString());
        modelsize1CheckBox->setText(QApplication::translate("AiModelSet", "2", Q_NULLPTR));
        modelsize2CheckBox->setText(QApplication::translate("AiModelSet", "3", Q_NULLPTR));
        okPushButton->setText(QApplication::translate("AiModelSet", "\347\241\256\345\256\232", Q_NULLPTR));
        addPushButton->setText(QApplication::translate("AiModelSet", "\345\212\240\350\275\275\345\267\262\346\234\211\346\250\241\345\236\213", Q_NULLPTR));
        annotationInfoEdit->setText(QString());
        delTrainListPushButton->setText(QApplication::translate("AiModelSet", "deltrain", Q_NULLPTR));
        fgPushButton->setText(QApplication::translate("AiModelSet", "fg", Q_NULLPTR));
        selectPushButton->setText(QApplication::translate("AiModelSet", "select", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AiModelSet: public Ui_AiModelSet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AIMODELSET_H
