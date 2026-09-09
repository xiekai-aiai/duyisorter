/********************************************************************************
** Form generated from reading UI file 'aimodelset.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AIMODELSET_H
#define UI_AIMODELSET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
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
            AiModelSet->setObjectName(QString::fromUtf8("AiModelSet"));
        AiModelSet->resize(1024, 768);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AiModelSet->sizePolicy().hasHeightForWidth());
        AiModelSet->setSizePolicy(sizePolicy);
        AiModelSet->setMinimumSize(QSize(1024, 768));
        AiModelSet->setMaximumSize(QSize(1024, 768));
        QFont font;
        font.setFamily(QString::fromUtf8("HarmonyOS Sans Medium"));
        AiModelSet->setFont(font);
        imgLabel = new QLabel(AiModelSet);
        imgLabel->setObjectName(QString::fromUtf8("imgLabel"));
        imgLabel->setGeometry(QRect(0, 160, 1020, 64));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(imgLabel->sizePolicy().hasHeightForWidth());
        imgLabel->setSizePolicy(sizePolicy1);
        imgLabel->setMinimumSize(QSize(1020, 64));
        imgLabel->setMaximumSize(QSize(1020, 64));
        QFont font1;
        font1.setFamily(QString::fromUtf8("HarmonyOS Sans Medium"));
        font1.setPointSize(8);
        imgLabel->setFont(font1);
        modelNameLabel = new QLabel(AiModelSet);
        modelNameLabel->setObjectName(QString::fromUtf8("modelNameLabel"));
        modelNameLabel->setGeometry(QRect(20, 40, 281, 41));
        modelNameLabel->setFont(font1);
        modelNameLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelNameLabel->setScaledContents(false);
        modelNameLabel->setIndent(-1);
        a_checkBox = new QCheckBox(AiModelSet);
        a_checkBox->setObjectName(QString::fromUtf8("a_checkBox"));
        a_checkBox->setGeometry(QRect(310, 40, 60, 40));
        sizePolicy1.setHeightForWidth(a_checkBox->sizePolicy().hasHeightForWidth());
        a_checkBox->setSizePolicy(sizePolicy1);
        a_checkBox->setMinimumSize(QSize(60, 40));
        a_checkBox->setMaximumSize(QSize(60, 40));
        a_checkBox->setFont(font1);
        b_checkBox = new QCheckBox(AiModelSet);
        b_checkBox->setObjectName(QString::fromUtf8("b_checkBox"));
        b_checkBox->setGeometry(QRect(380, 40, 60, 40));
        QFont font2;
        font2.setFamily(QString::fromUtf8("HarmonyOS Sans Medium"));
        font2.setPointSize(8);
        font2.setBold(false);
        font2.setWeight(50);
        b_checkBox->setFont(font2);
        c_checkBox = new QCheckBox(AiModelSet);
        c_checkBox->setObjectName(QString::fromUtf8("c_checkBox"));
        c_checkBox->setGeometry(QRect(440, 40, 60, 40));
        c_checkBox->setFont(font1);
        tipLabel = new QLabel(AiModelSet);
        tipLabel->setObjectName(QString::fromUtf8("tipLabel"));
        tipLabel->setGeometry(QRect(20, 100, 891, 31));
        QFont font3;
        font3.setFamily(QString::fromUtf8("HarmonyOS Sans Medium"));
        font3.setPointSize(7);
        tipLabel->setFont(font3);
        tipLabel->setIndent(-1);
        modelTrainPushButton = new QPushButton(AiModelSet);
        modelTrainPushButton->setObjectName(QString::fromUtf8("modelTrainPushButton"));
        modelTrainPushButton->setGeometry(QRect(720, 30, 141, 51));
        modelTrainPushButton->setFont(font1);
        modelTrainPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        modelSelPushButton->setObjectName(QString::fromUtf8("modelSelPushButton"));
        modelSelPushButton->setGeometry(QRect(870, 30, 141, 51));
        modelSelPushButton->setFont(font1);
        modelSelPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        simuPushButton->setObjectName(QString::fromUtf8("simuPushButton"));
        simuPushButton->setGeometry(QRect(610, 30, 91, 51));
        simuPushButton->setFont(font1);
        simuPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        modelTrainPushButton_2->setObjectName(QString::fromUtf8("modelTrainPushButton_2"));
        modelTrainPushButton_2->setGeometry(QRect(200, 410, 111, 51));
        modelTrainPushButton_2->setFont(font1);
        modelTrainPushButton_2->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        modelTrainPushButton_3->setObjectName(QString::fromUtf8("modelTrainPushButton_3"));
        modelTrainPushButton_3->setGeometry(QRect(320, 410, 111, 51));
        modelTrainPushButton_3->setFont(font1);
        modelTrainPushButton_3->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        addTrainListPushButton->setObjectName(QString::fromUtf8("addTrainListPushButton"));
        addTrainListPushButton->setGeometry(QRect(500, 410, 181, 51));
        addTrainListPushButton->setFont(font1);
        addTrainListPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        annotationPushButton->setObjectName(QString::fromUtf8("annotationPushButton"));
        annotationPushButton->setGeometry(QRect(690, 410, 91, 51));
        annotationPushButton->setFont(font1);
        annotationPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        erasePushButton->setObjectName(QString::fromUtf8("erasePushButton"));
        erasePushButton->setGeometry(QRect(790, 410, 91, 51));
        erasePushButton->setFont(font1);
        erasePushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        preImageNumLabel->setObjectName(QString::fromUtf8("preImageNumLabel"));
        preImageNumLabel->setGeometry(QRect(920, 410, 91, 51));
        preImageNumLabel->setFont(font1);
        preImageNumLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        preImageNumLabel->setScaledContents(false);
        preImageNumLabel->setAlignment(Qt::AlignCenter);
        preImageNumLabel->setIndent(-1);
        importImgPushButton = new QPushButton(AiModelSet);
        importImgPushButton->setObjectName(QString::fromUtf8("importImgPushButton"));
        importImgPushButton->setGeometry(QRect(10, 600, 141, 51));
        importImgPushButton->setFont(font1);
        importImgPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        modelTrainPushButton_6->setObjectName(QString::fromUtf8("modelTrainPushButton_6"));
        modelTrainPushButton_6->setGeometry(QRect(160, 600, 111, 51));
        modelTrainPushButton_6->setFont(font1);
        modelTrainPushButton_6->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        modelTrainPushButton_7->setObjectName(QString::fromUtf8("modelTrainPushButton_7"));
        modelTrainPushButton_7->setGeometry(QRect(280, 600, 111, 51));
        modelTrainPushButton_7->setFont(font1);
        modelTrainPushButton_7->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        modelTrainPushButton_8->setObjectName(QString::fromUtf8("modelTrainPushButton_8"));
        modelTrainPushButton_8->setGeometry(QRect(400, 600, 91, 51));
        modelTrainPushButton_8->setFont(font1);
        modelTrainPushButton_8->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        m_prevBtn->setObjectName(QString::fromUtf8("m_prevBtn"));
        m_prevBtn->setGeometry(QRect(600, 600, 81, 51));
        m_prevBtn->setFont(font1);
        m_prevBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        m_nextBtn->setObjectName(QString::fromUtf8("m_nextBtn"));
        m_nextBtn->setGeometry(QRect(700, 600, 81, 51));
        m_nextBtn->setFont(font1);
        m_nextBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        m_curPageInfoLabel->setObjectName(QString::fromUtf8("m_curPageInfoLabel"));
        m_curPageInfoLabel->setGeometry(QRect(800, 600, 131, 21));
        QFont font4;
        font4.setFamily(QString::fromUtf8("HarmonyOS Sans Medium"));
        font4.setPointSize(6);
        m_curPageInfoLabel->setFont(font4);
        m_curPageInfoLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        m_curPageInfoLabel->setScaledContents(false);
        m_curPageInfoLabel->setAlignment(Qt::AlignCenter);
        m_curPageInfoLabel->setIndent(-1);
        m_curPagelineEdit = new QLineEdit(AiModelSet);
        m_curPagelineEdit->setObjectName(QString::fromUtf8("m_curPagelineEdit"));
        m_curPagelineEdit->setGeometry(QRect(940, 610, 41, 31));
        m_curPagelineEdit->setAlignment(Qt::AlignCenter);
        modelNameLabel_4 = new QLabel(AiModelSet);
        modelNameLabel_4->setObjectName(QString::fromUtf8("modelNameLabel_4"));
        modelNameLabel_4->setGeometry(QRect(980, 600, 31, 51));
        modelNameLabel_4->setFont(font1);
        modelNameLabel_4->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelNameLabel_4->setScaledContents(false);
        modelNameLabel_4->setAlignment(Qt::AlignCenter);
        modelNameLabel_4->setIndent(-1);
        backPushButton = new QPushButton(AiModelSet);
        backPushButton->setObjectName(QString::fromUtf8("backPushButton"));
        backPushButton->setGeometry(QRect(840, 660, 161, 40));
        backPushButton->setFont(font1);
        backPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        m_pageInfoLabel->setObjectName(QString::fromUtf8("m_pageInfoLabel"));
        m_pageInfoLabel->setGeometry(QRect(800, 630, 131, 20));
        m_pageInfoLabel->setFont(font4);
        m_pageInfoLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        m_pageInfoLabel->setScaledContents(false);
        m_pageInfoLabel->setAlignment(Qt::AlignCenter);
        m_pageInfoLabel->setIndent(-1);
        modeInfowidget = new QWidget(AiModelSet);
        modeInfowidget->setObjectName(QString::fromUtf8("modeInfowidget"));
        modeInfowidget->setGeometry(QRect(160, 40, 701, 421));
        modelInfoLabel = new QLabel(modeInfowidget);
        modelInfoLabel->setObjectName(QString::fromUtf8("modelInfoLabel"));
        modelInfoLabel->setGeometry(QRect(250, 20, 211, 41));
        QFont font5;
        font5.setFamily(QString::fromUtf8("HarmonyOS Sans Medium"));
        font5.setPointSize(9);
        modelInfoLabel->setFont(font5);
        modelInfoLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelInfoLabel->setScaledContents(false);
        modelInfoLabel->setAlignment(Qt::AlignCenter);
        modelInfoLabel->setIndent(-1);
        modelNameInputLabel = new QLabel(modeInfowidget);
        modelNameInputLabel->setObjectName(QString::fromUtf8("modelNameInputLabel"));
        modelNameInputLabel->setGeometry(QRect(60, 100, 131, 41));
        modelNameInputLabel->setFont(font5);
        modelNameInputLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelNameInputLabel->setScaledContents(false);
        modelNameInputLabel->setAlignment(Qt::AlignCenter);
        modelNameInputLabel->setIndent(-1);
        modelSizeLabel = new QLabel(modeInfowidget);
        modelSizeLabel->setObjectName(QString::fromUtf8("modelSizeLabel"));
        modelSizeLabel->setGeometry(QRect(60, 210, 131, 41));
        modelSizeLabel->setFont(font5);
        modelSizeLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/fonts/HarmonyOS_Sans_SC_Medium.ttf")));
        modelSizeLabel->setScaledContents(false);
        modelSizeLabel->setAlignment(Qt::AlignCenter);
        modelSizeLabel->setIndent(-1);
        modelsize1CheckBox = new QCheckBox(modeInfowidget);
        modelsize1CheckBox->setObjectName(QString::fromUtf8("modelsize1CheckBox"));
        modelsize1CheckBox->setGeometry(QRect(240, 210, 61, 40));
        QFont font6;
        font6.setFamily(QString::fromUtf8("HarmonyOS Sans Medium"));
        font6.setPointSize(9);
        font6.setBold(false);
        font6.setWeight(50);
        modelsize1CheckBox->setFont(font6);
        modelsize2CheckBox = new QCheckBox(modeInfowidget);
        modelsize2CheckBox->setObjectName(QString::fromUtf8("modelsize2CheckBox"));
        modelsize2CheckBox->setGeometry(QRect(360, 210, 60, 40));
        modelsize2CheckBox->setFont(font5);
        okPushButton = new QPushButton(modeInfowidget);
        okPushButton->setObjectName(QString::fromUtf8("okPushButton"));
        okPushButton->setGeometry(QRect(280, 360, 161, 41));
        okPushButton->setFont(font1);
        okPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        addPushButton->setObjectName(QString::fromUtf8("addPushButton"));
        addPushButton->setGeometry(QRect(520, 100, 151, 41));
        addPushButton->setFont(font1);
        addPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        modelNameLineEdit->setObjectName(QString::fromUtf8("modelNameLineEdit"));
        modelNameLineEdit->setGeometry(QRect(200, 100, 301, 40));
        line = new QFrame(AiModelSet);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(0, 80, 1021, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(AiModelSet);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(0, 380, 1021, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(AiModelSet);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(0, 580, 1021, 16));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        m_imageList = new QListWidget(AiModelSet);
        m_imageList->setObjectName(QString::fromUtf8("m_imageList"));
        m_imageList->setGeometry(QRect(40, 470, 911, 60));
        m_imageList->setMaximumSize(QSize(1025, 60));
        annotationInfoEdit = new QLabel(AiModelSet);
        annotationInfoEdit->setObjectName(QString::fromUtf8("annotationInfoEdit"));
        annotationInfoEdit->setGeometry(QRect(10, 280, 891, 31));
        annotationInfoEdit->setFont(font3);
        annotationInfoEdit->setIndent(-1);
        delTrainListPushButton = new QPushButton(AiModelSet);
        delTrainListPushButton->setObjectName(QString::fromUtf8("delTrainListPushButton"));
        delTrainListPushButton->setGeometry(QRect(880, 410, 81, 51));
        delTrainListPushButton->setFont(font1);
        delTrainListPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        fgPushButton->setObjectName(QString::fromUtf8("fgPushButton"));
        fgPushButton->setGeometry(QRect(10, 410, 81, 51));
        fgPushButton->setFont(font1);
        fgPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        selectPushButton->setObjectName(QString::fromUtf8("selectPushButton"));
        selectPushButton->setGeometry(QRect(100, 410, 81, 51));
        selectPushButton->setFont(font1);
        selectPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        AiModelSet->setWindowTitle(QCoreApplication::translate("AiModelSet", "Form", nullptr));
        imgLabel->setText(QString());
        modelNameLabel->setText(QString());
        a_checkBox->setText(QCoreApplication::translate("AiModelSet", " A", nullptr));
        b_checkBox->setText(QCoreApplication::translate("AiModelSet", "B", nullptr));
        c_checkBox->setText(QCoreApplication::translate("AiModelSet", "C", nullptr));
        tipLabel->setText(QString());
        modelTrainPushButton->setText(QCoreApplication::translate("AiModelSet", "\345\274\200\345\247\213\350\256\255\347\273\203", nullptr));
        modelSelPushButton->setText(QCoreApplication::translate("AiModelSet", "\346\250\241\345\236\213\345\212\240\350\275\275", nullptr));
        simuPushButton->setText(QCoreApplication::translate("AiModelSet", "\344\273\277\347\234\237", nullptr));
        modelTrainPushButton_2->setText(QCoreApplication::translate("AiModelSet", "\346\211\213\345\212\250\346\240\207\346\263\250", nullptr));
        modelTrainPushButton_3->setText(QCoreApplication::translate("AiModelSet", "\350\207\252\345\212\250\346\240\207\346\263\250", nullptr));
        addTrainListPushButton->setText(QCoreApplication::translate("AiModelSet", "+ \346\267\273\345\212\240\345\210\260\350\256\255\347\273\203\350\265\204\346\272\220", nullptr));
        annotationPushButton->setText(QCoreApplication::translate("AiModelSet", "\346\240\207\346\263\250", nullptr));
        erasePushButton->setText(QCoreApplication::translate("AiModelSet", "\346\223\246\351\231\244", nullptr));
        preImageNumLabel->setText(QString());
        importImgPushButton->setText(QCoreApplication::translate("AiModelSet", "\345\257\274\345\205\245\345\233\276\347\211\207\350\265\204\346\272\220", nullptr));
        modelTrainPushButton_6->setText(QCoreApplication::translate("AiModelSet", "\346\270\205\347\251\272\350\265\204\346\272\220", nullptr));
        modelTrainPushButton_7->setText(QCoreApplication::translate("AiModelSet", "\345\205\250\351\203\250\345\210\240\351\231\244", nullptr));
        modelTrainPushButton_8->setText(QCoreApplication::translate("AiModelSet", "\345\210\240\351\231\244", nullptr));
        m_prevBtn->setText(QCoreApplication::translate("AiModelSet", "\344\270\212\344\270\200\351\241\265", nullptr));
        m_nextBtn->setText(QCoreApplication::translate("AiModelSet", "\344\270\213\344\270\200\351\241\265", nullptr));
        m_curPageInfoLabel->setText(QString());
        m_curPagelineEdit->setText(QCoreApplication::translate("AiModelSet", "11", nullptr));
        modelNameLabel_4->setText(QString());
        backPushButton->setText(QCoreApplication::translate("AiModelSet", "\350\277\224\345\233\236", nullptr));
        m_pageInfoLabel->setText(QString());
        modelInfoLabel->setText(QString());
        modelNameInputLabel->setText(QString());
        modelSizeLabel->setText(QString());
        modelsize1CheckBox->setText(QCoreApplication::translate("AiModelSet", "2", nullptr));
        modelsize2CheckBox->setText(QCoreApplication::translate("AiModelSet", "3", nullptr));
        okPushButton->setText(QCoreApplication::translate("AiModelSet", "\347\241\256\345\256\232", nullptr));
        addPushButton->setText(QCoreApplication::translate("AiModelSet", "\345\212\240\350\275\275\345\267\262\346\234\211\346\250\241\345\236\213", nullptr));
        annotationInfoEdit->setText(QString());
        delTrainListPushButton->setText(QCoreApplication::translate("AiModelSet", "deltrain", nullptr));
        fgPushButton->setText(QCoreApplication::translate("AiModelSet", "fg", nullptr));
        selectPushButton->setText(QCoreApplication::translate("AiModelSet", "select", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AiModelSet: public Ui_AiModelSet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AIMODELSET_H
