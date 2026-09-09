/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        remotecontrol.cpp
 * \brief       远程控制界面源文件
 *              子页面包括网络配置、设置服务器、设置短信及调试管理。
 * \date        2015.01.14
 */
#include "remotecontrol.h"
#include "unilog.h"

 /****************************************************************************
 **
 ** definition of class MyRemoteControl
 **
 ****************************************************************************/
MyRemoteControl::MyRemoteControl(QWidget* parent) :
    QWidget(parent)
{
    createRemoteControlPage();

    /* 每5s与服务器建立一次连接，判断服务器是否有效 */
    m_thread = new pingThread;
    connect(m_thread, SIGNAL(pingFailSig()), this, SLOT(onServerUnaliveSlt()));

    /* 从本地文件中读取相关配置 */
    getConfFromFile();
    //    getPasswordFromFile();

        /* 创建网络通信交互线程 */
        //devStatusMonitorThread = new udpSockThread();
        //devStatusMonitorThread->start();
}

/* 创建远程控制页面 */
void MyRemoteControl::createRemoteControlPage()
{
    QGridLayout* upLay = new QGridLayout;
    QHBoxLayout* downLay = new QHBoxLayout;
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    m_serverIPLabel = new myLabel(myLan.server_ip);
    m_serverIPLineEdit = new myLineEdit("");
    m_serverIPLineEdit->setFixedHeight(BTN_HEIGHT);
    m_portLabel = new myLabel(myLan.server_port);
    m_portLineEdit = new myLineEdit("");
    m_portLabelNC = new myLabel("NC " + myLan.server_port);
    m_portLineEditNC = new myLineEdit("10000");
    m_portLineEdit->setFixedHeight(BTN_HEIGHT);
    m_portLineEditNC->setFixedHeight(BTN_HEIGHT);
    m_usernameLabel = new myLabel(myLan.username);
    m_usernameLineEdit = new myLineEdit("");
    m_usernameLineEdit->setFixedHeight(BTN_HEIGHT);
    m_passwordLabel = new myLabel(myLan.password);
    m_passwordLineEdit = new myLineEdit("");
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setFixedHeight(BTN_HEIGHT);
    m_localVirtualAddrLabel = new myLabel(myLan.local_virtual_ip);
    m_localVirtualAddrLineEdit = new myLineEdit("0.0.0.0");
    m_localVirtualAddrLineEdit->setFixedHeight(BTN_HEIGHT);

    m_connectBtn = new myPushButton(myLan.connect, myIcon.Device_Network);
    m_connectBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_disconnectBtn = new myPushButton(myLan.disconnect, myIcon.Device_NetworkError);
    m_disconnectBtn->setFixedSize(BTN_WIDTH, BTN_HEIGHT);
    m_disconnectBtn->setEnabled(false);

    upLay->addWidget(m_localVirtualAddrLabel, 0, 0);
    upLay->addWidget(m_localVirtualAddrLineEdit, 0, 1);
    upLay->addWidget(m_serverIPLabel, 1, 0);
    upLay->addWidget(m_serverIPLineEdit, 1, 1);
    upLay->addWidget(m_portLabel, 2, 0);
    upLay->addWidget(m_portLineEdit, 2, 1);
    upLay->addWidget(m_portLabelNC, 3, 0);
    upLay->addWidget(m_portLineEditNC, 3, 1);
    upLay->addWidget(m_usernameLabel, 4, 0);
    upLay->addWidget(m_usernameLineEdit, 4, 1);
    upLay->addWidget(m_passwordLabel, 5, 0);
    upLay->addWidget(m_passwordLineEdit, 5, 1);
    m_portLabelNC->hide();
    m_portLineEditNC->hide();
    m_usernameLabel->hide();
    m_usernameLineEdit->hide();
    m_passwordLabel->hide();
    m_passwordLineEdit->hide();
    downLay->addWidget(m_connectBtn);
    downLay->addWidget(m_disconnectBtn);
    downLay->setContentsMargins(0, 5, 0, 0);
    downLay->setSpacing(200);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);
    mainLay->setContentsMargins(10, 0, 10, 0);

    /* 连接预览页面中控件的槽函数 */
    connect(m_serverIPLineEdit, SIGNAL(pressed()), this, SLOT(onServerIPLineEditPressed()));
    connect(m_portLineEdit, SIGNAL(pressed()), this, SLOT(onPortLineEditPressed()));
    connect(m_portLineEditNC, SIGNAL(pressed()), this, SLOT(onPortLineEditNCPressed()));
    connect(m_usernameLineEdit, SIGNAL(pressed()), this, SLOT(onUsernameLineEditPressed()));
    connect(m_passwordLineEdit, SIGNAL(pressed()), this, SLOT(onPasswordLineEditPressed()));
    connect(m_connectBtn, SIGNAL(pressed()), this, SLOT(onConnectBtnPressed()));
    connect(m_disconnectBtn, SIGNAL(pressed()), this, SLOT(onDisconnectBtnPressed()));
}

/* 响应服务器地址输入框被按下事件 */
void MyRemoteControl::onServerIPLineEditPressed()
{
    myInputPanel* inputDlg = new myInputPanel(textType, 0, 0, 0);
    inputDlg->setText(m_serverIPLineEdit->text());
    inputDlg->setTitle(m_serverIPLabel->text());

    if (inputDlg->exec() == QDialog::Accepted)
    {
        m_serverIPLineEdit->setText(inputDlg->getText());
        setConfFile();
    }
}

/* 响应服务器端口号被按下事件 */
void MyRemoteControl::onPortLineEditPressed()
{
    myInputPanel* inputDlg = new myInputPanel(intType, 0, 65534, m_portLineEdit->text().toInt());
    inputDlg->setTitle(myLan.server_port);

    if (inputDlg->exec() == QDialog::Accepted)
    {
        m_portLineEdit->setText(inputDlg->getText());
        setConfFile();
    }
}

void MyRemoteControl::onPortLineEditNCPressed()
{
    myInputPanel* inputDlg = new myInputPanel(intType, 0, 65534, m_portLineEditNC->text().toInt());
    inputDlg->setTitle("NC " + myLan.server_port);

    if (inputDlg->exec() == QDialog::Accepted)
    {
        m_portLineEditNC->setText(inputDlg->getText());
        strNetInfo.selfNetControlPort = m_portLineEditNC->text().toInt();
    }
}

/* 响应用户名输入框被按下事件 */
void MyRemoteControl::onUsernameLineEditPressed()
{
    myInputMethod* inputDlg = new myInputMethod(myLan.username, m_usernameLineEdit->text());
    inputDlg->setTitle(myLan.username);

    if (inputDlg->exec() == QDialog::Accepted)
    {
        m_usernameLineEdit->setText(inputDlg->getText());
        setPasswordFile();
    }
}

/* 响应密码输入框被按下事件 */
void MyRemoteControl::onPasswordLineEditPressed()
{
    myInputPanel* inputDlg = new myInputPanel(passwdType, 0, 0, 0);
    inputDlg->setTitle(myLan.password);

    if (inputDlg->exec() == QDialog::Accepted)
    {
        m_passwordLineEdit->setText(inputDlg->getText());
        setPasswordFile();
    }
}

/* 响应连接按钮按下事件 */
void MyRemoteControl::onConnectBtnPressed()
{
    infoWidget->setLabelText(myLan.msg_connecting);
    infoWidget->delayShow();

#ifdef Q_OS_UNIX
    char cmd[64];
    sprintf(cmd, "cd /opt/app/OpenVPN;openvpn --config client.conf &");
    system(cmd);
    myFlow.sleep(3);
#endif
    if (connetToServer())    // 连接服务器成功
    {
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);
        infoWidget->hide();
    }
    else                    // 连接服务器失败
    {
#ifdef Q_OS_UNIX
        system("killall openvpn");
#endif
        infoWidget->setLabelText(myLan.msg_connect_fail);
        infoWidget->showSecs();
    }
}

/* 响应断开连接按钮按下事件 */
void MyRemoteControl::onDisconnectBtnPressed()
{
#ifdef Q_OS_UNIX
    char cmd[64];
    sprintf(cmd, "killall openvpn");
    system(cmd);
#endif

    m_connectBtn->setEnabled(true);
    m_disconnectBtn->setEnabled(false);
    m_localVirtualAddrLineEdit->setText("0.0.0.0");
}

/* 响应返回按钮按下事件 */
void MyRemoteControl::onBackBtnPressed()
{
    emit backToFactoryMainPageSig();
}

/* 根据用户设置修改配置文件 */
void MyRemoteControl::setConfFile()
{
    QFile file("/opt/app/OpenVPN/client.conf");
    if (!file.open(QIODevice::ReadWrite))
    {
        qDebug("client.conf open error!");
        return;
    }

    QTextStream io(&file);
    QString str;
    int lineNum = 0;
    while (!io.atEnd())
    {
        str = io.readLine();
        lineNum++;
        QString tmpStr = str.mid(0, 6);
        if (tmpStr == QString("remote"))
        {
            str = QString("remote %1 %2").arg(m_serverIPLineEdit->text()).arg(m_portLineEdit->text());
#ifdef Q_OS_UNIX
            char cmd[128];
            sprintf(cmd, "sed -i '%dc\%s' /opt/app/OpenVPN/client.conf", lineNum, str.toLatin1().data());
            system(cmd);
#endif
        }
    }
}

/* 根据用户设置修改用户名密码文件 */
void MyRemoteControl::setPasswordFile()
{
    QFile file(QString("%1/userdata/cnf/cnf.vpn-auth").arg(APP_PATH));
    QTextStream in(&file);

    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        char str[256] = {};
        sprintf(str, "%s%s", APP_PATH, "/userdata/cnf/cnf.vpn-auth : File open error!");
        qDebug(str);
        return;
    }

    in << m_usernameLineEdit->text() << "\n" << m_passwordLineEdit->text();

    file.close();
}

/* 进入远程升级界面时从本地文件中读取配置信息 */
void MyRemoteControl::getConfFromFile()
{
    QFile file("/opt/app/OpenVPN/client.conf");

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QTextStream out(&file);
    QString str, subStr;
    QString ipStr, portStr;
    int ipLen = 0;
    while (!out.atEnd())
    {
        str = out.readLine();
        subStr = str.mid(0, 6);
        if (subStr == QString("remote"))
        {
            for (int i = 7; i < str.length(); i++)
            {
                if (str.at(i) == ' ')
                {
                    ipLen = i - 7;
                    ipStr = str.mid(7, ipLen);
                    m_serverIPLineEdit->setText(ipStr);
                    portStr = str.mid(8 + ipLen, str.length());
                    m_portLineEdit->setText(portStr);
                    break;
                }
            }
        }
    }

    file.close();
}

/* 进入远程升级界面时从本地文件中读取用户名密码 */
void MyRemoteControl::getPasswordFromFile()
{
    QFile file(QString("%1/userdata/cnf/cnf.vpn-auth").arg(APP_PATH));
    //    QFile file("/userdata/cnf/cnf.vpn-auth");

        /* 若认证文件不存在,则新建文件并写入默认的用户名与密码 */
    if (!file.exists())
    {
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug("userdata/cnf/cnf.vpn-auth : File create error!");
            return;
        }
        QTextStream in(&file);
        in << "PCUSER1" << "\n" << "140219";
        m_usernameLineEdit->setText("PCUSER1");
        m_passwordLineEdit->setText("140219");
        file.close();

        return;
    }

    /* 若认证文件存在,则从文件中读取用户名与密码 */
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("userdata/cnf/cnf.vpn-auth : File open error!");
        return;
    }

    QTextStream out(&file);
    QString usernameStr, passwordStr;

    out >> usernameStr;
    out >> passwordStr;
    m_usernameLineEdit->setText(usernameStr);
    m_passwordLineEdit->setText(passwordStr);

    file.close();
}

/* 获取本地虚拟网络地址 */
bool MyRemoteControl::getLocalVirtualAddr()
{
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    if (interfaceList.size() == 0)
    {
        return false;
    }
    for (int i = 0; i < interfaceList.size(); i++)
        //    foreach (QNetworkInterface interface, interfaceList)
    {
        if (interfaceList.at(i).name() == QString("tun0"))
        {
            m_localVirtualAddrLineEdit->setText(getVpnIpAddress());
            return true;
        }
    }
    return false;
}

QString MyRemoteControl::getVpnIpAddress()
{
    QProcess process;
    // 使用 sh -c 来执行包含管道的命令
    process.start("sh", QStringList() << "-c" << "ip addr show tun0 | grep 'inet ' | awk '{print $2}' | cut -d/ -f1");
    process.waitForFinished();

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0)
    {
        QString output = process.readAllStandardOutput();
        output = output.simplified(); // 去除前后空格
        return output;
    }
    else
    {
        // 处理命令执行失败的情况
        QString error = process.readAllStandardError();
        qDebug() << "Command execution failed:" << error;
        return "";
    }

}

/* 判断屏连接服务器是否成功 */
bool MyRemoteControl::connetToServer()
{
    int maxTimes = 30;

    for (int i = 0; i < maxTimes; i++)
    {
        if (getLocalVirtualAddr())
            return true;

        myFlow.sleep(1);
    }

    return false;
}

/* 响应服务器失效的槽函数 */
void MyRemoteControl::onServerUnaliveSlt()
{
    onDisconnectBtnPressed();
}

/****************************************************************************
**
** definition of class pingThread
**
****************************************************************************/
/* pingThread的构造函数 */
pingThread::pingThread()
{
    m_bIsRunning = true;
}

/* 判断服务器是否有效 */
bool pingThread::serverIsAlive()
{
    m_cmd = new QProcess;
    m_cmd->start("ping -w 5 10.8.0.1");

    if (m_cmd->waitForFinished())
    {
        QString str = m_cmd->readAll();

#ifdef Q_OS_UNIX
        if (!str.contains("ttl"))
        {
#else
        if (!str.contains("TTL"))
        {
#endif

            m_cmd->kill();
            return false;
        }
    }

    return true;
}

/* pingThread的运行函数 */
void pingThread::run()
{
    m_bIsRunning = true;
    while (m_bIsRunning)
    {
        if (!serverIsAlive())
        {
            qDebug("ping Fail!");
            emit pingFailSig();
            return;
        }

        qDebug("ping OK!");
        myFlow.sleep(20);
    }
}

/* 停止ping操作 */
void pingThread::stopPing()
{
    m_bIsRunning = false;
    m_cmd->kill();
}

/****************************************************************************
**
** definition of class MyNetConfig
**
****************************************************************************/
/* 网络配置构造函数 */
MyNetConfig::MyNetConfig(QWidget * parent)
    : QWidget(parent)
{
    /* page init */
    for (int i = 0; i < 6; i++)
    {
        m_hostLabel.append(new myLabel(tr("")));
        m_hostValue.append(new myLineEdit(tr("")));
        m_hostValue[i]->setFixedHeight(BTN_HEIGHT);
    }

    /* page update */
    for (int i = 0; i < m_hostValue.size(); i++)
    {
        m_hostLabel[i]->setText(getNetText(i));
    }

    m_hostValue[NET_ROUTE]->hide();//外网IP不用
    m_hostLabel[NET_ROUTE]->hide();

    /* get host address */
    getHostAddress();

    /* signals to slots */
    m_sigMapper = new QSignalMapper;
    for (int i = 0; i < m_hostValue.size(); i++)
    {
        /*
        if (i == NET_MAC || i == NET_ROUTE)
            continue;
        */
        m_sigMapper->setMapping(m_hostValue[i], i);
        connect(m_hostValue[i], SIGNAL(pressed()), m_sigMapper, SLOT(map()));
    }
    connect(m_sigMapper, SIGNAL(mapped(int)), this, SLOT(setHostAddress(int)));

    /* page layout */
    QGridLayout* mainLayout = new QGridLayout(this);
    for (int i = 0; i < m_hostValue.size(); i++)
    {
        mainLayout->addWidget(m_hostLabel[i], i, 0);
        mainLayout->addWidget(m_hostValue[i], i, 1);
    }
}

/* 获取对应的网络选项文本 */
QString MyNetConfig::getNetText(int index)
{
    QString text;
    switch (index)
    {
    case NET_IP:
        text = myLan.ip_address;
        break;
    case NET_MASK:
        text = myLan.subnet_mask;
        break;
    case NET_GATEWAY:
        text = myLan.default_gateway;
        break;
    case NET_DNS:
        text = myLan.dns_server;
        break;
    case NET_MAC:
        text = myLan.mac_address;
        break;
    case NET_ROUTE:
        text = myLan.route_address;
        break;
    }

    return text;
}

/* 获取主机网络地址 */
void MyNetConfig::getNetConfig()
{
#ifdef Q_OS_UNIX
    getNetSetting(QString("%1/userdata/cnf/cnf.network").arg(APP_PATH));
#else
    getHostAddress();
#endif
}

void MyNetConfig::setNetConfig(int index)
{
#ifdef Q_OS_UNIX
    setNetSetting(QString("%1/userdata/cnf/cnf.network").arg(APP_PATH));
#else
    setHostAddress(index);
#endif
}

/* 获取文件系统网络配置信息 */
void MyNetConfig::getNetSetting(QString setting)
{
    QSettings netConfig(setting, QSettings::IniFormat);

    m_hostValue[NET_IP]->setText(netConfig.value("IP").toString());
    m_hostValue[NET_MASK]->setText(netConfig.value("Mask").toString());
    m_hostValue[NET_GATEWAY]->setText(netConfig.value("Gateway").toString());
    m_hostValue[NET_DNS]->setText(netConfig.value("DNS").toString());
    m_hostValue[NET_MAC]->setText(netConfig.value("MAC").toString());
}

/* 保存网络配置信息至文件系统 */
void MyNetConfig::setNetSetting(QString setting)
{
    /* remove exist setting file */
    QDir dir;
    dir.remove(setting);

    /* config setting file */
    QSettings netConfig(setting, QSettings::IniFormat);

    netConfig.setValue("IP", m_hostValue[NET_IP]->text());
    netConfig.setValue("Mask", m_hostValue[NET_MASK]->text());
    netConfig.setValue("Gateway", m_hostValue[NET_GATEWAY]->text());
    netConfig.setValue("DNS", m_hostValue[NET_DNS]->text());
    netConfig.setValue("MAC", m_hostValue[NET_MAC]->text());
    netConfig.sync();
}

/* 获取默认网关信息 */
QString MyNetConfig::getGateway()
{
    QProcess process;
    process.setReadChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_UNIX
    process.start("route", QStringList() << "-n");
#else
    process.start("route", QStringList() << "PRINT");
#endif

    if (process.waitForFinished())
    {
        QString input = process.readAll();
#ifdef Q_OS_UNIX
        QStringList allLine = input.split("\n");
#else
        QStringList allLine = input.split("\r\n");
#endif
        foreach(QString line, allLine)
        {
            QStringList field = line.split(" ", QString::SkipEmptyParts);
            if (field.size() > 0)
            {
                QString value = field.first();
                if (value == QHostAddress(QHostAddress::Any).toString())
                {
#ifdef Q_OS_UNIX
                    return field.at(field.indexOf(value) + 1);
#else
                    return field.at(field.indexOf(value) + 2);
#endif
                }
            }
        }
    }
    return NULL;
}

/* 获取域名服务信息 */
QString MyNetConfig::getDNS()
{
    QProcess process;
    process.setReadChannelMode(QProcess::MergedChannels);
    process.start("nslookup", QStringList() << QHostInfo::localHostName());

    if (process.waitForFinished())
    {
        QString input = process.readAll();

#ifdef Q_OS_UNIX
        QStringList allLine = input.split("\n");
#else
        QStringList allLine = input.split("\r\n");
#endif
        foreach(QString line, allLine)
        {
            QStringList field = line.split(":", QString::SkipEmptyParts);
            foreach(QString value, field)
            {
                if (value.contains("Address"))
                {
                    return field.at(field.indexOf(value) + 1).simplified();
                }
            }
        }
    }

    return NULL;
}

/* 获取路由公网出口信息 */
QString MyNetConfig::getRoute()
{
    QProcess process;
    process.setReadChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_UNIX
    process.start("traceroute", QStringList() << "-n");
#else
    process.start("route", QStringList() << "PRINT");
#endif

    if (process.waitForFinished())
    {
        QString input = process.readAll();
#ifdef Q_OS_UNIX
        QStringList allLine = input.split("\n");
#else
        QStringList allLine = input.split("\r\n");
#endif
        foreach(QString line, allLine)
        {
            QStringList field = line.split(" ", QString::SkipEmptyParts);
            if (field.size() > 0)
            {
                QString value = field.first();
                if (value == QHostAddress(QHostAddress::Any).toString())
                {
#ifdef Q_OS_UNIX
                    return field.at(field.indexOf(value) + 1);
#else
                    return field.at(field.indexOf(value) + 2);
#endif
                }
            }
        }
    }

    return NULL;
}

/* 获取网络配置信息 */
void MyNetConfig::getHostAddress()
{
    /* get setting by default */
    QString ip, netmask, gateway, mac, dns, route;

    /* get setting from config file */
    getNetSetting(QString("%1/userdata/cnf/cnf.network").arg(APP_PATH));

    /* get setting from system config */
        /* get ip, netmask, mac */
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    if (interfaceList.size() == 0)
    {
        return;
    }
    for (int i = 0; i < interfaceList.size(); i++)
    {
        //    foreach (QNetworkInterface interface, interfaceList) {
        if (!(interfaceList.at(i).flags() & QNetworkInterface::IsUp) || (interfaceList.at(i).flags() & QNetworkInterface::IsLoopBack))
        {
            //        if (interfaceList.at(i).flags() & QNetworkInterface::IsLoopBack)
            continue;
        }
        //        qDebug()<<"123:"<<interfaceList.at(i).name();

#ifdef Q_OS_UNIX
        if (interfaceList.at(i).name() != "eth0" && interfaceList.at(i).name() != "eth1")
        {
            break;
        }
#endif
        QList<QNetworkAddressEntry> hostAddressList = interfaceList.at(i).addressEntries();
        if (hostAddressList.size() == 0)
        {
            return;
        }
        for (int j = 0; j < hostAddressList.size(); j++)
            //        foreach (QNetworkAddressEntry address, hostAddressList)
        {
            if (hostAddressList.at(j).ip().protocol() != QAbstractSocket::IPv4Protocol)
                continue;
            if (hostAddressList.at(j).ip().toString() == QHostAddress(QHostAddress::LocalHost).toString())
                continue;
            ip = hostAddressList.at(j).ip().toString();
            netmask = hostAddressList.at(j).netmask().toString();
            break;
        }
        if (ip != QHostAddress(QHostAddress::LocalHost).toString())
        {
            mac = interfaceList.at(i).hardwareAddress();
            break;
        }
    }

    /* get gateway */
    gateway = getGateway();

    /* get dns */
//    dns = getDNS();

    // set text 
    m_hostValue[NET_IP]->setText(ip);
    m_hostValue[NET_MASK]->setText(netmask);
    m_hostValue[NET_GATEWAY]->setText(gateway);
    m_hostValue[NET_MAC]->setText(mac);
    //  m_hostValue[NET_DNS]->setText(dns);
}

/* 设置主机网络配置信息 */
void MyNetConfig::setHostAddress(int index)
{
    QString hostAddress;

    myInputPanel inputDlg(textType, 0, 0, 0);
    inputDlg.setTitle(getNetText(index));
    inputDlg.setText(m_hostValue[index]->text());
    if (inputDlg.exec() == QDialog::Accepted)
    {
        /* get input value */
        hostAddress = inputDlg.getText();

        /* config network interface */
        QString cmd;
        switch (index)
        {
        case NET_IP:
#ifdef Q_OS_UNIX
            cmd = QString("ifconfig eth0 %1 netmask %2 up").arg(hostAddress).arg(m_hostValue[NET_MASK]->text());
            system(qPrintable(cmd));
            cmd = QString("route add default gw %1").arg(m_hostValue[NET_GATEWAY]->text());
            system(qPrintable(cmd));
#endif
            break;
        case NET_MASK:
#ifdef Q_OS_UNIX
            cmd = QString("ifconfig eth0 %1 netmask %2 up").arg(m_hostValue[NET_IP]->text()).arg(hostAddress);
            system(qPrintable(cmd));
            cmd = QString("route add default gw %1").arg(m_hostValue[NET_GATEWAY]->text());
            system(qPrintable(cmd));
#endif
            break;
        case NET_GATEWAY:
#ifdef Q_OS_UNIX
            cmd = QString("route add default gw %1").arg(hostAddress);
            system(qPrintable(cmd));
#endif
            break;
        case NET_DNS:
#ifdef Q_OS_UNIX
            cmd = QString("echo nameserver %1 > /etc/resolv.conf").arg(hostAddress);
            system(qPrintable(cmd));
#endif
            break;
        case NET_MAC:
#ifdef Q_OS_UNIX
            cmd = QString("ifconfig eth0 down");
            system(qPrintable(cmd));
            cmd = QString("ifconfig eth0 hw ether %1").arg(hostAddress);
            system(qPrintable(cmd));
            cmd = QString("ifconfig eth0 %1 netmask %2 up").arg(m_hostValue[NET_IP]->text()).arg(m_hostValue[NET_MASK]->text());
            system(qPrintable(cmd));
            cmd = QString("route add default gw %1").arg(m_hostValue[NET_GATEWAY]->text());
            system(qPrintable(cmd));
#endif
            break;
        case NET_ROUTE:
            break;
        }

        /* save to text control */
        m_hostValue[index]->setText(hostAddress);

        /* save setting file */
#ifdef Q_OS_UNIX
        setNetSetting(QString("%1/userdata/cnf/cnf.network").arg(APP_PATH));
#endif
    }
}

/****************************************************************************
**
** definition of class MyBigData
**
****************************************************************************/
MyBigData::MyBigData(QWidget * parent)
    : QWidget(parent)
{
    /* page init */
        /* QR code Group */
    m_qrcodeCbx = new myGroupBox(myLan.qrcode);
    /* widget to generate QR code */
    m_qrCodeLabel = new myLabel("");
    if (LCD_HEIGHT == 480)
    {
        m_qrCodeLabel->setFixedSize(200, 200);
    }
    else
    {
        m_qrCodeLabel->setMinimumSize(400, 400);
    }
    m_qrCodeLabel->setPixmap(QPixmap(":res/png/QRCode.png").scaled(m_qrCodeLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    /* server Group */
    m_serverCbx = new myGroupBox(myLan.server);
    m_serverLabel = new myLabel(myLan.server);
    m_serverValue = new myLineEdit(myString.sServer);
    m_serverValue->setFixedHeight(BTN_HEIGHT);

    connect(m_serverValue, SIGNAL(pressed()), this, SLOT(setServer()));

    /* page layout */
        // qrcode layout
    QHBoxLayout* qrcodeLayout = new QHBoxLayout;
    qrcodeLayout->addStretch();
    qrcodeLayout->addWidget(m_qrCodeLabel);
    qrcodeLayout->addStretch();

    // server layout 
    QHBoxLayout* serverLayout = new QHBoxLayout(m_serverCbx);
    serverLayout->addWidget(m_serverLabel);
    serverLayout->addWidget(m_serverValue);

    // main layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(qrcodeLayout);
}

MyBigData::~MyBigData()
{

}

QString MyBigData::generateQRString()
{
    QString url = QString("%1/meyerdata/myindex.php?").arg(myString.sServer);

    /* information */
        /* engineer */
    url.append(QString("%1=%2&").arg("E0001").arg("meyer"));

    /* datetime */
    QString sDatetime;
    sDatetime.append(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss"));
    url.append(QString("%1=%2&").arg("E0002").arg(sDatetime));

    /* location */
    url.append(QString("%1=%2&").arg("E0003").arg("Hefei"));

    /* factory config */
        /* machine serial number */
    url.append(QString("%1=%2&").arg("E1001").arg(6000));

    /* machine type */
    url.append(QString("%1=%2&").arg("E1002").arg(struCnfe.nMachine));

    /* product type */
    url.append(QString("%1=%2&").arg("E1003").arg(struCnfe.nProduct));

    /* machine version */
    QString sVersion = QString("%1_%2").arg(myString.sAppVersion).arg(myString.sAppSVN);
    url.append(QString("%1=%2&").arg("E1004").arg(sVersion));

    /* arithmetic list */

    /* language list */

/* global config */
    /* delay code */
    url.append(QString("%1=%2&").arg("E2001").arg(paramDelayCode.ena));

    /* current languange */
    url.append(QString("%1=%2&").arg("E2002").arg(struCnfg.nLang));

    /* current profile */

    /* profile total */

/* camera config */
    /* sensor type */
    url.append(QString("%1=%2&").arg("E3001").arg(struCnfc.nSensorType));

    /* sensor pixel */
    url.append(QString("%1=%2&").arg("E3002").arg(struCnfc.nSensorPixel));

    /* sensor frequency */
    url.append(QString("%1=%2&").arg("E3003").arg(struCnfc.nSensorRowFre));

    /* profile config */
        /* profile's name */
    QString sProfileName = QString().fromLocal8Bit(struCnfg.struProfileIndex[struCnfg.nProfile].sMaterialName);
    url.append(QString("%1=%2&").arg("E4001").arg(sProfileName));

    /* arithmetic list */

    /* feeder value */

    return url;
}

void MyBigData::setServer()
{
    QString server;
    myInputMethod inputDlg(myLan.server, m_serverValue->text());
    if (inputDlg.exec() == QDialog::Accepted)
    {
        // get input value
        server = inputDlg.getText();
        m_serverValue->setText(server);

        // save to global param 
        myString.sServer = server;
        // save to config file 
        myFlow.saveServerSetting();
    }
}

void MyBigData::generateQRcode()
{

}

/****************************************************************************
**
** definition of class MyLoggerViewer
**
****************************************************************************/

MyLoggerViewer::MyLoggerViewer(QWidget * parent)
    : QWidget(parent)
{
    g_Config* config = new g_Config();
    QDateTime dateTime = QDateTime::currentDateTime();
    QVBoxLayout* qTextLayout = new QVBoxLayout(this);
    char cDateTmp[11] = { 0 };        //当前记录的时间

    logEventSeq = LOG_ALL;
    maxLine = 2000;
    qFile.setFileName(QString("%1").arg(LOG_FILE_NAME));
    qFile.open(QIODevice::ReadOnly);

    tmpFileData = new char[MAX_LEN_LOGFILE_SIZE];   //待整理数据
    if (tmpFileData == NULL)
    {
        LOG_ERROR_STM("malloc failed!");
        return;
    }
    memset(tmpFileData, 0, MAX_LEN_LOGFILE_SIZE);

    dateLen = qFile.read(tmpFileData, MAX_LEN_LOGFILE_SIZE);

    isBackFileHandled = 1;
    if (dateLen >= sizeof(cDateTmp))
    {
        for (quint32 i = 0; i < sizeof(cDateTmp) - 1; i++)
        {
            cDateTmp[i] = tmpFileData[i];
        }
        FirstdateInCurFile = QString("%1").fromUtf8(cDateTmp);
    }
    else
    {
        FirstdateInCurFile = QDateTime::currentDateTime().date().toString("yyyy-MM-dd");
    }

    qFile.close();

    qText = new QTextBrowser(this);
    qText->setFont(config->getFont());
    qText->setReadOnly(true);
    qText->setWordWrapMode(QTextOption::WordWrap);
    if (LCD_WIDTH == 640)
    {
        qText->setMinimumSize(455, 195);
    }
    else
    {
        qText->setMinimumSize(760, 440);
    }

    QHBoxLayout* qTextContentLayout = new QHBoxLayout;
    qTextContentLayout->addWidget(qText, 0, Qt::AlignLeft);

    myLabel* from = new myLabel(myLan.from);
    dateTimeEditFrom = new QDateTimeEdit(this);
    dateTimeEditFrom->setFont(config->getFont());
    dateTimeEditFrom->setDisplayFormat("yyyy-MM-dd");
    dateTimeEditFrom->setCalendarPopup(true);
    QCalendarWidget* calendarWidgetFrom = dateTimeEditFrom->calendarWidget();
    calendarWidgetFrom->setFont(config->getFont());
    calendarWidgetFrom->setGridVisible(true);
    dateTimeEditFrom->setDateTime(dateTime);

    myLabel* to = new myLabel(myLan.to);
    dateTimeEditTo = new QDateTimeEdit(this);
    dateTimeEditTo->setFont(config->getFont());
    dateTimeEditTo->setDisplayFormat("yyyy-MM-dd");
    dateTimeEditTo->setCalendarPopup(true);
    QCalendarWidget* calendarWidgetTo = dateTimeEditTo->calendarWidget();
    calendarWidgetTo->setFont(config->getFont());
    calendarWidgetTo->setGridVisible(true);
    dateTimeEditTo->setDateTime(dateTime);

    QHBoxLayout* qTextControlLayout = new QHBoxLayout;
    qTextControlLayout->addWidget(from, 0, Qt::AlignLeft);
    qTextControlLayout->addWidget(dateTimeEditFrom, 0, Qt::AlignLeft);
    qTextControlLayout->addStretch();
    qTextControlLayout->addWidget(to, 0, Qt::AlignLeft);
    qTextControlLayout->addWidget(dateTimeEditTo, 0, Qt::AlignLeft);
    qTextControlLayout->addStretch();

    eventTypeCombo = new MyComboBox();
    eventTypeCombo->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    eventTypeCombo->setFixedHeight(BTN_HEIGHT - 12);

    reLoadFileBtn = new myPushButton(myLan.fresh, QIcon());
    reLoadFileBtn->setMaximumSize(BTN_HEIGHT + 10, BTN_HEIGHT - 12);

    qTextControlLayout->addWidget(eventTypeCombo, 0, Qt::AlignLeft);
    qTextControlLayout->addStretch();
    qTextControlLayout->addWidget(reLoadFileBtn, 0, Qt::AlignLeft);
    qTextControlLayout->addStretch();
    qTextLayout->addLayout(qTextControlLayout);
    qTextLayout->addLayout(qTextContentLayout);
    reLoadFileBtn->hide();

    eventTypeCombo->insertItem(LOG_ALL, myLan.all);                       //LOG_ALL
    eventTypeCombo->insertItem(LOG_POWER, myLan.power_switch);            //LOG_POWER
    eventTypeCombo->insertItem(LOG_FEED_SWITCH, myLan.feeder_switch);     //LOG_FEED_SWITCH
    //eventTypeCombo->insertItem(LOG_MACHINE,myLan.machine);             //LOG_MACHINE
    eventTypeCombo->insertItem(LOG_PROFILE, myLan.profile);               //LOG_PROFILE
    eventTypeCombo->insertItem(LOG_CAMERA, myLan.camera);                 //LOG_CAMERA
    eventTypeCombo->insertItem(LOG_BACKGROUD, myLan.bg);                  //LOG_BACKGROUD
    eventTypeCombo->insertItem(LOG_ARITH, myLan.arith);                   //LOG_ARITH
    eventTypeCombo->insertItem(LOG_EJECT, myLan.reject);                  //LOG_EJECT
    eventTypeCombo->insertItem(LOG_WIPE, myLan.wipe);                     //LOG_WIPE
    eventTypeCombo->insertItem(LOG_FEED, myLan.feeder_value);             //LOG_FEED
    eventTypeCombo->insertItem(LOG_DEBUG, myLan.debug);                   //LOG_DEBUG
    eventTypeCombo->insertItem(LOG_WARM, myLan.warm);                   //LOG_DEBUG

    connect(dateTimeEditFrom, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onDateTimeEditFromChanged(QDateTime)));
    connect(dateTimeEditTo, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onDateTimeEditToChanged(QDateTime)));
    connect(eventTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnEventChanged(int)));
    connect(reLoadFileBtn, SIGNAL(pressed()), this, SLOT(onReloadLogFileBtnClicked()));
    updateTextByDate();
    updateTextByEventSeq();
}

MyLoggerViewer::~MyLoggerViewer()
{
    delete[]tmpFileData;
}

/* 在长度为len的字符串str中找到字符c的索引 */
qint64 MyLoggerViewer::getIndexOfCharacter(const char* str, quint64 len, char c)
{
    quint64 i = 0;
    qint64 ret;
    bool isFound = false;
    for (i = 0; i < len; i++)
    {
        if (str[i] == c)
        {
            ret = i;
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        ret = -1;
    }
    return ret;
}

/* 根据日期将符合条件限制的日志放到qLog中 */
void MyLoggerViewer::updateTextByDate()
{
    dateTimeFrom = dateTimeEditFrom->dateTime();
    dateTimeTo = dateTimeEditTo->dateTime();
    QString from = dateTimeFrom.date().toString("yyyy-MM-dd");
    QString to = dateTimeTo.date().toString("yyyy-MM-dd");
    qint64 pos = 0;                 //当前记录的开始位置
    qint64 recordCurEnd = 0;        //当前记录的结束位置
    qint64 recordEnd = 0;           //记录的结束位置
    QString DateTmp;                //获取当前记录中时间
    char cDateTmp[11] = { 0 };        //当前记录的时间
    int countRecord = 0;            //符合当前日期显示的记录数

    if (tmpFileData == NULL)
    {
        LOG_ERROR_STM("malloc failed!");
        return;
    }
    qLog.clear();

    if (from > to)
    {                //调整查询日志的起止时间
        DateTmp = from;
        from = to;
        to = DateTmp;
    }

    while (1)
    {
        if ((pos + sizeof(cDateTmp) - 1) > dateLen)
        {
            break;
        }
        for (quint32 i = 0; i < sizeof(cDateTmp) - 1; i++)
        {
            cDateTmp[i] = tmpFileData[pos + i];
        }
        DateTmp = QString("%1").fromUtf8(cDateTmp); //获取当前记录的时间

        recordCurEnd = getIndexOfCharacter(tmpFileData + pos, dateLen - pos, '\n');
        if (pos == 0)
        {
            recordEnd = recordCurEnd;
        }
        else
        {
            recordEnd = pos + recordCurEnd;
        }
        if (recordCurEnd == -1)
        {       //-1表示未找到该字符
            break;
        }

        if (DateTmp < from)
        {
            pos = recordEnd + 1;
            continue;
        }
        else if (DateTmp > to)
        {
            break;
        }
        else
        {
            char tmp = tmpFileData[pos + recordCurEnd + 1];
            tmpFileData[pos + recordCurEnd + 1] = '\0';
            qLog.append(tmpFileData + pos);
            tmpFileData[pos + recordCurEnd + 1] = tmp;

            countRecord++;
            pos = recordEnd + 1;
        }
    }
}

void MyLoggerViewer::onDateTimeEditFromChanged(QDateTime dateTime)
{
    dateTimeFrom.setDate(dateTime.date());
    updateText();
}

void MyLoggerViewer::onDateTimeEditToChanged(QDateTime dateTime)
{
    dateTimeTo.setDate(dateTime.date());
    updateText();
}

void MyLoggerViewer::updateTextByEventSeq()
{
    int countLogRecord = 0;         //符合当前日期及当前事件类型的记录数
    const char* tmpLog = NULL;
    int numIndexDec = 0;   //十位数
    int numIndex = 0;      //个位数
    int eventSeqFromLog = 0;
    int logEndIndex = 0;
    int* logIndexArr = NULL;
    QString qLogStr;

    logIndexArr = new int[maxLine];
    if (tmpFileData == NULL || logIndexArr == NULL)
    {
        LOG_ERROR_STM("malloc failed!");
        return;
    }

    for (logEndIndex = qLog.size() - 1; logEndIndex >= 0; logEndIndex--)
    {
        tmpLog = qPrintable(qLog.at(logEndIndex));
        numIndexDec = getIndexOfCharacter(tmpLog, strlen(tmpLog), '[') + 1;   //十位数
        numIndex = getIndexOfCharacter(tmpLog, strlen(tmpLog), ']') - 1;      //个位数
        eventSeqFromLog = 0;
        if (numIndexDec == 0)
        { //没找到[]字符
            continue;
        }
        if (numIndexDec == numIndex)
        {
            eventSeqFromLog = tmpLog[numIndex] - '0';
        }
        else
        {
            eventSeqFromLog = ((tmpLog[numIndexDec] - '0') * 10) + tmpLog[numIndex] - '0';
        }
        if (logEventSeq == eventSeqFromLog || logEventSeq == LOG_ALL)
        {
            if (countLogRecord >= maxLine)
            {
                break;
            }
            else
            {
                logIndexArr[countLogRecord] = logEndIndex;
            }
            countLogRecord++;
        }
    }
    for (int i = countLogRecord - 1; i >= 0; i--)
    {
        qLogStr.append(qLog.at(logIndexArr[i]));
    }

    if (isBackFileHandled == 0)
    {
        qLogStr.append(QString("%1%2").arg("Backup Total:").arg(countLogRecord));
    }
    else
    {
        qLogStr.append(QString("%1%2").arg("Total:").arg(countLogRecord));
    }

    qText->append(qLogStr);
    delete logIndexArr;
}

/* 响应事件触发类型选择框变化，在qLog中过滤符合该事件的日志  */
void MyLoggerViewer::OnEventChanged(int eventSeq)
{
    logEventSeq = eventSeq;
    updateText();
}

void MyLoggerViewer::reloadLogFile()
{
    if (dateTimeEditFrom->dateTime().date() >= QDateTime::currentDateTime().date()
        || dateTimeEditTo->dateTime().date() >= QDateTime::currentDateTime().date())
    {
        if ((FirstdateInCurFile >= dateTimeEditFrom->dateTime().date().toString("yyyy-MM-dd")
            || FirstdateInCurFile >= dateTimeEditTo->dateTime().date().toString("yyyy-MM-dd"))
            && ((QFile(LOG_BACK_FILE_NAME).exists() == 1) && (isBackFileHandled == 0)))
        {
            qFile.setFileName(QString("%1").arg(LOG_BACK_FILE_NAME));
        }
        else
        {
            qFile.setFileName(QString("%1").arg(LOG_FILE_NAME));
            isBackFileHandled = 1;
        }
        qFile.open(QIODevice::ReadOnly);

        if (tmpFileData == NULL)
        {
            LOG_ERROR_STM("malloc failed!");
            return;
        }
        memset(tmpFileData, 0, MAX_LEN_LOGFILE_SIZE);
        dateLen = qFile.read(tmpFileData, MAX_LEN_LOGFILE_SIZE);

        if (isBackFileHandled == 1)
        {       //查询结束，记下当前第一条记录的时间
            char cDateTmp[11] = { 0 };
            if (dateLen >= sizeof(cDateTmp))
            {
                for (quint32 i = 0; i < sizeof(cDateTmp) - 1; i++)
                {
                    cDateTmp[i] = tmpFileData[i];
                }
                FirstdateInCurFile = QString("%1").fromUtf8(cDateTmp);
            }
            else
            {
                FirstdateInCurFile = QDateTime::currentDateTime().date().toString("yyyy-MM-dd");
            }
        }

        qFile.close();
    }
}

void MyLoggerViewer::onReloadLogFileBtnClicked()
{
    updateText();
}

void MyLoggerViewer::updateText()
{
    infoWidget->setLabelText(myLan.msg_applying);
    infoWidget->delayShow();

    qText->clear();
    isBackFileHandled = 0;

    while (1)
    {
        reloadLogFile();
        updateTextByDate();
        updateTextByEventSeq();
        if (isBackFileHandled == 1)
        {
            break;
        }
        isBackFileHandled = 1;
    }

    infoWidget->hide();
}

/*!
 *
 * 从设备类
 *
 **********/
MySlaveList::MySlaveList(QWidget * parent)
    : QWidget(parent)
{
    QGridLayout* mainLay = new QGridLayout(this);

    for (int i = 0; i < MAX_SLAVE; i++)
    {
        m_label[i] = new myLabel(QString("%1").arg(i + 1));
        m_label[i]->setAlignment(Qt::AlignCenter);
        m_lineEdit[i] = new myLineEdit;
        mainLay->addWidget(m_label[i], i, 0);
        mainLay->addWidget(m_lineEdit[i], i, 1);
    }

    m_sigMap = new QSignalMapper(this);
    for (int i = 0; i < MAX_SLAVE; i++)
    {
        m_sigMap->setMapping(m_lineEdit[i], i);
        connect(m_lineEdit[i], SIGNAL(pressed()), m_sigMap, SLOT(map()));
    }
    connect(m_sigMap, SIGNAL(mapped(int)), this, SLOT(onLineEditPressed(int)));
}

/*!
 * \brief MySlaveList::~MySlaveList
 */
MySlaveList::~MySlaveList()
{
}

/*!
 * \brief MySlaveList::refreshDisplay
 */
void MySlaveList::refreshDisplay()
{
    int nCount = struGsh.strSlaveList.count();
    if (nCount > MAX_SLAVE)
    {
        nCount = MAX_SLAVE;
    }

    for (int i = 0; i < nCount; i++)
    {
        m_lineEdit[i]->setText(struGsh.strSlaveList.at(i));
    }
}

/*!
 * \brief MySlaveList::onLineEditPressed
 * \param nIndex
 */
void MySlaveList::onLineEditPressed(int nIndex)
{
    myInputPanel* input = new myInputPanel(textType, 0, 0, 0);
    input->setText(m_lineEdit[nIndex]->text());

    if (input->exec() == QDialog::Accepted)
    {
        m_lineEdit[nIndex]->setText(input->getText());
        QFile file(CNF_SLAVE_LIST);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            return;
        }

        QTextStream in(&file);
        struGsh.strSlaveList.clear();
        for (int i = 0; i < MAX_SLAVE; i++)
        {
            if (m_lineEdit[i]->text() != QString())
            {
                in << m_lineEdit[i]->text() << "\n";
                struGsh.strSlaveList.push_back(m_lineEdit[i]->text());
            }
        }
        file.close();
    }
}
