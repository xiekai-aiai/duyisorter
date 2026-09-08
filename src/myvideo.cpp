/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myvideo.cpp
 * \brief       视频图像显示界面源文件
 * \date        2015.08.17
 */
#include "myvideo.h"
int nSmallMatArea = 35 ;
/*!
 * \brief getHead
 * \param mem
 * \param size
 * \param str
 * \return
 */
char *getHead(const char *mem, int size, char *str)
{
    long lmem=size;
    char *cp = NULL;
    char *s1 = NULL;
    char *s2 = NULL;

    cp=(char*)mem;
    if((mem==NULL) || (str==NULL) || (size<=0)){
        return NULL;
    }

    if(!str){
        return ((char *)mem);
    }
    while(lmem>0){
        s1=cp;
        s2=(char *)str;
        while(*s1&&s2&&!(*s1-*s2)){
            s1++;
            s2++;
        }
        if(!*s2){
            return cp+10;
        }
        cp++;
        lmem--;
    }
    return NULL;
}



/*!
 * \brief 碎米率计算构造函数
 */
MyCalSmallMatThread::MyCalSmallMatThread()
{
    m_config = g_Config::getInstance();
#ifdef Q_OS_UNIX
    m_nImgWid = VIDEO_IMG_WID;
    m_nImgHei = VIDEO_ALL_IMG_HEI;
#else
    m_nImgWid = 1024;
    m_nImgHei = 500;
#endif
    m_pImgData = new uchar[m_nImgWid*m_nImgHei*3];
    m_pDispData = new uchar[m_nImgWid*m_nImgHei];

    m_nNum = 0;
    m_nTotal = 0;
    m_nSmall = 0;

    memset(m_backMax, 0, IMAGE_WIDTH_MAX*3);
    memset(m_backMin, 0, IMAGE_WIDTH_MAX*3);
}

/*!
 * \brief 析构函数
 */
MyCalSmallMatThread::~MyCalSmallMatThread()
{
    if(m_pImgData !=NULL)
        delete m_pImgData;
    if(m_pDispData !=NULL)
        delete m_pDispData;
}

//获取用于计算碎米率的图像(RBG)
bool MyCalSmallMatThread::getImageData()
{
    QByteArray imageByteArray;
    QImage img;
    imageQMutex.lock();
    if(!ImageQueue.isEmpty()){
       imageByteArray = ImageQueue.first();
       imageQMutex.unlock();
    }else{
        imageQMutex.unlock();
        return false;
    }

    if (!img.loadFromData(imageByteArray, "JPEG")) {
        qWarning("Failed to load image from JPEG data");
        return false;
     }

    if(img.isNull()){
        return false;
    }

    int oriIndex;
    uchar r,g,b;
    QColor rgb;
    for(int i=0; i<m_nImgHei; i++){
        for(int j=0; j<m_nImgWid; j++){
            rgb = img.pixel(j, i);
            oriIndex = (i*m_nImgWid+j);
            r = rgb.red();
            g = rgb.green();
            b = rgb.blue();
            m_pImgData[oriIndex*3] = r;
            m_pImgData[oriIndex*3+1] = g;
            m_pImgData[oriIndex*3+2] = b;
        }
    }

    //自动背景信息校验   倒数第0、1行相同，第2、3行相同
    return true;
}


/* 图像二值化 */
void MyCalSmallMatThread::gray()
{
    int nIndex;
    m_nPixelNum = 0;
    bool grayFlag,rFlag,gFlag,bFlag;
    for (int i = 0; i < m_nImgHei; i++) {
        for (int j = 0; j < m_nImgWid; j++) {
            nIndex = i*m_nImgWid+j;
            uchar r = m_pImgData[nIndex*3];
            uchar g = m_pImgData[nIndex*3+1];
            uchar b = m_pImgData[nIndex*3+2];

            grayFlag = myAIShare.pixelIsBackgroundcomm(r,g,b,j);
            if(grayFlag){
                m_pDispData[nIndex] = 255;
            } else {
                //m_nPixelNum++;                      //计算物料点总数
                m_pDispData[nIndex] = 0;
            }
        }
    }
}


/* 图像的腐蚀运算 */
void MyCalSmallMatThread::erosion()
{
    int flag;
    uchar *pTmpData = new uchar[m_nImgWid*m_nImgHei];
    memcpy(pTmpData, m_pDispData, m_nImgWid*m_nImgHei);

    for (int i = 1; i < m_nImgHei-1; i++) {
        for (int j = 1; j < m_nImgWid-1; j++) {
            flag = 1;
            for (int m = i-1; m < i+2; m++) {
                for (int n = j-1; n < j+2; n++) {
                    if (pTmpData[m*m_nImgWid+n] == 255) {
                        flag = 0;
                        break;
                    }
                }

                if (flag == 0) {
                    break;
                }
            }
            if (flag == 0) {
                m_pDispData[i*m_nImgWid+j]  = 255;
            } else {
                m_pDispData[i*m_nImgWid+j] = 0;
            }
        }
    }
    delete []pTmpData;
}

/* 图像的膨胀运算 */
void MyCalSmallMatThread::dilation()
{
    int flag;
    uchar *pTmpData = new uchar[m_nImgWid*m_nImgHei];
    memcpy(pTmpData, m_pDispData, m_nImgWid*m_nImgHei);

    for (int i = 1; i < m_nImgHei-1; i++) {
        for (int j = 1; j < m_nImgWid-1; j++) {
            flag = 1;
            for (int m = i-1; m < i+2; m++) {
                for (int n = j-1; n < j+2; n++) {
                    if (pTmpData[m*m_nImgWid+n] == 0) {
                        flag = 0;
                        break;
                    }
                }
                if(flag == 0) {
                    break;
                }
            }
            if (flag == 0) {
                m_pDispData[i*m_nImgWid+j] = 0;
            } else {
                m_pDispData[i*m_nImgWid+j] = 255;
            }
        }
    }

    delete []pTmpData;
}

/* 是否为背景点 */
bool MyCalSmallMatThread::IsBack(int x, int y)
{
    if(x<0 || x>m_nImgWid-1 || y<0 || y>m_nImgHei-1) {
        return true;
    }

    if(m_pDispData[y*m_nImgWid+x] == 0) {
        return false;
    } else {
        return true;
    }
}

/* DFS深度搜索 */
bool MyCalSmallMatThread::DFS(int x, int y, int label)
{
    if(IsBack(x, y)) {
        return false;
    }
//考虑加限制，避免找物料陷入死循环
#if 0
    if(m_nNum >500){
            return false;
    }
#endif
    m_pDispData[y*m_nImgWid+x] = 255;//搜索过的物料点标记为非0（背景）的点
    for(int i=0; i<4; i++) {
        if(!IsBack(x+pDirection[i][0], y+pDirection[i][1])) {
            m_nNum++;
            DFS(x+pDirection[i][0], y+pDirection[i][1], label);
        }
    }
    return true;
}

/* 寻找物料点 */
void MyCalSmallMatThread::findMaterial()
{
    int nMatSum = 1;
    m_nNum = 1;
    m_vMatParams.clear();
    m_nPixelNum = 0;
    for (int i = 0; i < m_nImgHei; i++) {
        for (int j = 0; j < m_nImgWid; j++) {
            if (DFS(j, i, nMatSum)) {
                MaterialParams matParams;
                matParams.nLabel = nMatSum;
                matParams.nNum = m_nNum;
                m_vMatParams.push_back(matParams);
                m_nPixelNum +=m_nNum;
                m_nNum = 1;
                nMatSum++;
            }
        }
    }
}

/* 寻找小物料并显示 */
void MyCalSmallMatThread::findSmallAndDisplay()
{
    char pSmallLabel[1024];      // 小物料的索引号
    int nSmallNum = 0;          // 小物料的数目
    int nMatArea = nSmallMatArea;
    //int nMatArea = 0.2 * (m_nPixelNum/ m_vMatParams.size());
    //qDebug("m_nPixelNum = %d, m_vMatParamsSize = %d\n", m_nPixelNum, m_vMatParams.size());
    double k = 0.0001;//一个物料点的重量g

    /* 1.寻找小物料 */
    for (int i = 0; i < m_vMatParams.size(); i++) {
        if (m_vMatParams.at(i).nNum <= nMatArea) {
            pSmallLabel[nSmallNum++] = i;
        }
    }
    /* 2.刷新碎米率 */
    m_nTotal = m_vMatParams.size();
    m_nSmall = nSmallNum;

    if(m_nTotal != 0)
        m_smallMatPer = (float)m_nSmall/m_nTotal *100;
    else
        m_smallMatPer = 0;

    if (m_nPixelNum == 0) {
        m_realThroughPut = 0;
    } else {
        m_realThroughPut = (double)((double)(m_nPixelNum*k)/1000.0*(3600*1000.0/50.0));
    }
    g_fRealSmallMatPer = m_smallMatPer;
    printf("totalMat = %d, smallMat = %d, smallPer = %.2f%\n", m_nTotal,m_nSmall,m_smallMatPer);
    fflush(0);
}


/*!
 * \brief 线程入口
 */
void MyCalSmallMatThread::run(void)
{
    bIsRunning = true;

    while(bIsRunning) {

        if(!getImageData())
        {
            myFlow.msleep(10);

            continue;
        }

        gray();

        /* 膨胀 */
        //dilation();
#if 1
        /* 腐蚀 */
        erosion();

        /* 根据设置的物料面积显示图片 */
        findMaterial();

        /* 寻找小物料并显示 */
        findSmallAndDisplay();
#endif
        myFlow.sleep(1);
    }
}

/* 终止线程 */
void MyCalSmallMatThread::stop()
{
    bIsRunning = false;
}

/*!
 * \brief 坏点标红处理函数
 */
char *MyVideoThread::simulateImage(const char *buffer, int bufferSize)
{
    int size = (VIDEO_IMG_WID*3);
    int showSize;
    showSize = bufferSize;

    char* tmpBuf = NULL;
	char  tmpImageBuf[size];
	for(int i=0; i<size; i++){
		tmpImageBuf[i] = 0;
	}
    tmpBuf = (char *)buffer;
    int i,j;

    //处理仿真图像数据部分
    for (i = showSize-1, j=size-1; i>=0, j>=0; i--) {
        if ((i+1)%4 ==0) {
            if (tmpBuf[i] != 0) {
                tmpBuf[i-1] = 0;
                tmpBuf[i-2] = 0;
                tmpBuf[i-3] = 255;
            }
            continue;
        }
        tmpImageBuf[j] = tmpBuf[i];
        j--;
    }

    tmpBuf = tmpImageBuf;
    return tmpBuf;
}

/*!
 * \brief MyVideoThread::beginStatistic
 */
void MyVideoThread::beginStatistic()
{
    m_nBeginTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    //! 界面选择前视，则前视统计产量及含杂
    //! 界面选择后视，则用前视统计产量，后视统计含杂
    if(struGsh.nUnit%2 == 0){
        nStaticUnit = struGsh.nUnit;
	bIsFront = true;
    } else {
        nStaticUnit = struGsh.nUnit - 1;
	bIsFront = false;
    }
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL, UNIT, 0, nStaticUnit, 0, 0, 0, 0, 1, 1);
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL, UNIT, 0, nStaticUnit+1, 0, 0, 0, 0, 1, 1);
}

/*!
 * \brief MyVideoThread::endStatistic
 */
void MyVideoThread::endStatistic()
{
    quint64 pixelNum = 0;
    quint64 badPointNum = 0;
    int nStatisticTime = 0;

    materialStatic.resize(32);
    materialStatic.fill(0);
    materialStatic1.resize(32);
    materialStatic1.fill(0);

    //! 产量测试停止
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL, UNIT, 0, nStaticUnit, 0, 0, 0, 0, 0, 1);
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_CTRL, UNIT, 0, nStaticUnit+1, 0, 0, 0, 0, 0, 1);
    m_nEndTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    nStatisticTime = int(m_nEndTime - m_nBeginTime);

    //! 产量和含杂信息上传
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_UPLOAD, UNIT, 0, nStaticUnit, 0, 0, 0, 0, 0, 1);
    int nRet = MySerial.com1Read(materialStatic.data(), materialStatic.size(), 1000);
    myFlow.msleep(500);
    MySerial.com1Write(CMD_UNIT_MATERIAL_STATISTIC_UPLOAD, UNIT, 0, nStaticUnit+1, 0, 0, 0, 0, 0, 1);
    int nRet1 = MySerial.com1Read(materialStatic1.data(), materialStatic1.size(), 1000);

    //! 检查回包长度及包头
    if ((nRet == materialStatic.size() && checkStatisticData(materialStatic))
            && (nRet1 == materialStatic1.size() && checkStatisticData(materialStatic1))) {
        materialStatic.remove(0, 10);
        materialStatic.truncate(materialStatic.size()-6);

        materialStatic1.remove(0, 10);
        materialStatic1.truncate(materialStatic1.size()-6);

        for (int i = 0; i < 8; i++) {
            pixelNum += ((quint64)materialStatic.at(i))<<((7-i)*8);
        }
        if (bIsFront) {
            for (int i = 8; i < 16; i++) {
                badPointNum += ((quint64)materialStatic.at(i))<<((15-i)*8);
            }
        } else {
            for (int i = 8; i < 16; i++) {
                badPointNum += ((quint64)materialStatic1.at(i))<<((15-i)*8);
            }
        }
    } else {
        pixelNum = 0;
        badPointNum = 0;
    }
    
    if (nStatisticTime == 0 || pixelNum == 0) {
        g_fRealThroughPut = 0;
        g_fRealDirtPer = 0;
    } else {
        g_fRealThroughPut = (double)(((double)(struCnfg.aFactor*pixelNum+struCnfg.bFactor))/(double)nStatisticTime);
        g_fRealDirtPer = (double)((double)badPointNum/(double)pixelNum);
    }

    printf("staticTime: %d\n pixelNum: %llu\n badPointNum: %llu\nrealThroughPut: %f\nrelDirtPer:%f\n",
           nStatisticTime,
           pixelNum,
           badPointNum,
           g_fRealThroughPut,
           g_fRealDirtPer);
    fflush(0);
}

/*!
 * \brief 获取视频图像数据
 */
QByteArray MyVideoThread::getVideoImage(void)
{
#ifdef Q_OS_UNIX
    QImage img, img1;
    img_vec.clear();
    QByteArray jpegData;
//    buffer.clear();
    uint32_t image_len;
    while (true && bIsRunning) {
        if(!bIsRunning){
            break;
        }
//        QElapsedTimer timer1;
//        timer1.start();
        // 临时缓冲区用于接收单次数据
        unsigned char temp_buf[409600];  // 单次接收4KB，避免大数组
        ssize_t recv_len = recvfrom(MyUpd.getSockfd(), temp_buf, sizeof(temp_buf), 0,
                                  (struct sockaddr*)&client_addr, &addr_len);
//        std::cout << "耗时timer1.elapsed(): " << timer1.nsecsElapsed() /1000 << " ms" << std::endl;

        // 处理接收结果（关键错误检查）
        if (recv_len < 0) {
//            std::cerr << "接收失败: " << strerror(errno) << std::endl;
            std::cout << "接收失败" << std::endl;
            if(!bIsRunning){
                break;
            }
            continue;  // 跳过错误，继续接收
        } else if (recv_len == 0) {
            std::cout << "客户端关闭连接" << std::endl;
            continue;
        }

        // 限制缓冲区大小，避免内存耗尽
        if (buffer.size() + recv_len > BUFFER_SIZE) {
            std::cout << "缓冲区即将溢出，重置缓冲区,buffer.size():"<<buffer.size()<<" recv_len:"<<recv_len<< std::endl;
            buffer.clear();
            continue;
        }
//        QElapsedTimer timer2;
//        timer2.start();

        // 将新接收的数据追加到全局缓冲区（保持数据连续性）
        buffer.insert(buffer.end(), temp_buf, temp_buf + recv_len);
//        std::cout << "已接收 " << recv_len << " 字节，累计: " << buffer.size() << " 字节" << std::endl;
//        std::cout << "耗时timer2.elapsed(): " << timer2.nsecsElapsed() /1000 << " ms" << std::endl;

//        QElapsedTimer timer3;
//        timer3.start();
        // 数据处理：检查是否有完整的头部（至少12字节）
        if (buffer.size() >= 12) {
            // 验证魔术字（0xA6A66A6A）
            if (buffer[0] == 0xA6 && buffer[1] == 0xA6 &&
                buffer[2] == 0x6A && buffer[3] == 0x6A) {

                // 解析图像长度（注意网络字节序转主机字节序）
                image_len = ntohl(*(reinterpret_cast<int*>(buffer.data() + 4)));
//                std::cout << "检测到图像头部，图像长度: " << image_len << " 字节" << std::endl;

                // 检查是否接收完整（头部8字节 + 图像数据）
                if (buffer.size() >= 8 + image_len + 4) {
                    try {
                        uint32_t tail =   8 + image_len;
                        if(buffer[tail] == 0xFF && buffer[tail+1] == 0xFF &&
                                buffer[tail+2] == 0xFF && buffer[tail+3] == 0xFF){
                            // 提取图像数据
                            img_vec.assign(buffer.begin() + 8,buffer.begin() + 8 + image_len);
                            // 移除已处理的数据，保留剩余字节（可能是下一个图像）
                            buffer.erase(buffer.begin(),buffer.begin()+12+image_len);
                            std::cout << "获取到完整图像,buffer.size:"<<buffer.size()<<"image_len:"<<image_len<< std::endl;
    //                        buffer.clear();
                            break;
                        }else{
                            buffer.clear();
                            std::cout << "包尾异常" << std::endl;
                            continue;
                        }

                    } catch (const std::exception& e) {
                        std::cerr << "图像处理出错: " << e.what() << std::endl;
                        buffer.clear();  // 出错时清空缓冲区，避免后续异常
                    }
                }
            } else{
                buffer.clear();

            }
         }
//        std::cout << "耗时timer3.elapsed(): " << timer3.nsecsElapsed() /1000  << " ms" << std::endl;

       }

        //! 视频图像数据填充
    //    img1 = QImage(tmpColorBuf, VIDEO_IMG_WID, VIDEO_ALL_IMG_HEI, QImage::Format_RGB888);
//        QElapsedTimer timer1;
//        timer1.start();
//        img1 = QImage((uchar *)img_vec.data(), IMAGE_WIDTH-4, IMAGE_HEIGHT, QImage::Format_RGB888);
//        img = img1.copy();
//        std::cout << "耗时timer1.elapsed(): " << timer1.elapsed() << " ms" << std::endl;
//            QElapsedTimer timer1;
//            timer1.start();
        jpegData = QByteArray(reinterpret_cast<const char*>(img_vec.data()), img_vec.size());
//        if (!img.loadFromData(jpegData, "JPG")) {
//            qWarning("Failed to load image from JPG data");
//            return QImage();
//        }else{
//            QByteArray pngData;
//            QBuffer buffer(&pngData);
//            QImage imgPng;
//            if (!buffer.open(QIODevice::WriteOnly) || !img.save(&buffer, "PNG")) {
//                if (!imgPng.loadFromData(pngData, "PNG")) {
//                    return  QImage();
//                }
//            }else{
//                return  QImage();
//            }
//            img1 = img.copy();
            img_vec.clear();
    //        buffer.clear();
//            pngData.clear();
//            buffer.close();
//            std::cout << "耗时timer1.elapsed(): " << timer1.elapsed() << " ms" << std::endl;

            return jpegData;


#endif
    return QByteArray();
}

/*!
 * \brief //! 为了防止和串口拍图片冲突，把切换模式协议行数定为2049
 */
void MyVideoThread::switchUARTModeRowNum()
{
//    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, nStaticUnit, 0, 0, 0,
//                        2048/256, 2048%256, 3);
//    MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, struGsh.nLevel, nStaticUnit+1, 0, 0, 0,
//                       2048/256, 2048%256, 3);
}

/*!
 * \brief MyVideoThread::checkStatisticData
 * \param data
 * \return
 */
bool MyVideoThread::checkStatisticData(QByteArray data)
{
    if (data.size() < 4) {
        return false;
    }
    if (data.at(0) == 0xAA
            && data.at(1) == 0xAA
            && data.at(2) == 0xAA
            && data.at(3) == 0xAA) {
        return true;
    }

    return false;
}


/*!
 * \brief 视频图像采集控制构造函数
 */
MyVideoThread::MyVideoThread()
{
    #ifdef Q_OS_UNIX
    addr_len = sizeof(client_addr);
    // 用vector作为动态缓冲区（便于处理剩余数据）
    //    std::vector<unsigned char> buffer;
    buffer.reserve(BUFFER_SIZE);  // 预分配内存提升效率
    #endif
}

/*!
 * \brief 视频图像采集控制析构函数
 */
MyVideoThread::~MyVideoThread()
{

}

/*!
 * \brief 视频图像采集线程入口 
 */  
void MyVideoThread::run(void)
{
    bIsRunning = true;
    while(bIsRunning) {
        //! 统计开始
//        beginStatistic();

        int updateTimes = 3;
        while (updateTimes > 0) {
            updateTimes--;
            if (!bIsRunning) {
                break;
            }
#ifdef Q_OS_UNIX
            // 开始计时
//            QElapsedTimer timer;
//            timer.start();
            QByteArray img = getVideoImage();
//            std::cout << "耗时timer.elapsed(): " << timer.elapsed() << " ms" << std::endl;

#else
            QByteArray img;
//            QImage img = QImage(VIDEO_IMG_WID, VIDEO_IMG_HEI, QImage::Format_RGB32);
//            if (qrand()%2) {
//                img.load(g_strVideo);
//            } else {
//                img.load(g_strMeyer);
//            }
//            img.load(g_blank);

#endif
            if (!img.isNull()) {
                dealImageQueue(img);
//                myFlow.msleep(20);
            } else {
                //! 若失败需等待1秒待接口板USB固件复位
//                myFlow.sleep(1);
            }
        }

        //! 统计结束
//        endStatistic();
    }
}

void MyVideoThread::dealImageQueue(QByteArray img)
{   
    ImageQueue.enqueue(img);
    while (ImageQueue.size() > 2) {
        imageQMutex.lock();
        ImageQueue.dequeue();
        imageQMutex.unlock();
    }
}
/* 终止线程 */
void MyVideoThread::stop()
{
    bIsRunning = false;
    bIsUdpReady = false;

}

/*!
 * \brief 视频图像显示控制类构造函数
 */
MyVideo::MyVideo(QWidget *parent) :
        QWidget(parent)
{
    m_bIsCapturing = false;
    // 视频图像采集
    m_pThread = new MyVideoThread;
    m_pCalSmallMatThread = new MyCalSmallMatThread;

    // 视频图像显示
    m_pVideoLabel = new myLabel("");
    m_pVideoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pVideoLabel->setFixedSize(1000, 500);

    m_pVideoLabel2 = new myLabel("");
    m_pVideoLabel2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pVideoLabel2->setFixedSize(1000, 500);

    infoLbl = new myLabel("");
    infoLbl->setFixedHeight(BTN_HEIGHT);
    infoLbl->setStyleSheet("color:rgb(230,0,45)");

    myPushButton *setBackButton = new myPushButton(myLan.back,myIcon.Action_Back);
    setBackButton->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *Lay = new QVBoxLayout;
    // 通道设置
    m_pUnitGroup = new myGroupBox(myLan.chute);

    QSize btnSize = g_Config::getInstance()->getBtnSize(SMALL_BTN_SIZE);
    m_pUnitGroup->setFont(g_Config::getInstance()->getFont());
    m_pUnitGroup->setFixedHeight(100);

    myLabel *m_RowBtn = new myLabel(myLan.row);
    m_RowBtn->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_Row = new QLCDNumber;
    m_Row->setDigitCount(3);
    m_Row->display(g_nRow);
    m_Row->setFixedSize(QSize(BTN_WIDTH, BTN_HEIGHT));
    m_RowMinusBtn = new myPushButton("",myIcon.Action_Minus);
    m_RowPlusBtn = new myPushButton("",myIcon.Action_Plus);
    m_RowMinusBtn->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);
    m_RowPlusBtn->setFixedSize(SMALL_BTN_WIDTH, BTN_HEIGHT);

    m_pUnitViewBtn = new myPushButton(myLan.front_view, QIcon(), true, true);
    m_pUnitPlusBtn = new myPushButton("", myIcon.Action_Plus);
    m_pUnitLcdNum = new QLCDNumber;
    m_pUnitLcdNum->setDigitCount(2);
    m_pUnitLcdNum->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);
    m_pUnitLcdNum->display(struGsh.nUnit/2+1);
    m_pUnitMinusBtn = new myPushButton("",myIcon.Action_Minus);
    m_pCaptureBtn = new myPushButton("",myIcon.Media_Pause);
    m_pSimulateBtn = new myPushButton("", myIcon.Sorter_AI);
    m_pSimulateBtn->hide();


    m_pAutoAnalysisBtn = new myPushButton("",myIcon.Sorter_AI);
    m_pAutoAnalysisBtn->hide();

    // 页面逻辑
    connect(m_pThread, SIGNAL(sVideoImageFinished(QByteArray)), this, SLOT(updateVideoImageQueue(QByteArray)));

    connect(m_pUnitPlusBtn, SIGNAL(pressed()), this, SLOT(onUnitPlusBtnClicked()));
    connect(m_pUnitMinusBtn, SIGNAL(pressed()), this, SLOT(onUnitMinusBtnClicked()));
    connect(m_pCaptureBtn, SIGNAL(pressed()), this, SLOT(onCaptureBtnClicked()));
    connect(m_pUnitViewBtn, SIGNAL(pressed()), this, SLOT(onUnitViewClicked()));
    connect(this, SIGNAL(sBoardNumChanged()), this, SLOT(updateBoardNum()));
    connect(setBackButton, SIGNAL(pressed()), this, SLOT(onSetBackBtnClicked()));
    connect(m_RowPlusBtn,  SIGNAL(pressed()), this, SLOT(onRowPlusBtnClicked()));
    connect(m_RowMinusBtn,  SIGNAL(pressed()), this, SLOT(onRowMinusBtnClicked()));
    connect(m_pSimulateBtn, SIGNAL(pressed()), this, SLOT(onSimulateBtnClicked()));
    connect(m_pAutoAnalysisBtn,SIGNAL(pressed()),this,SLOT(onAutoAnalysisBtnClicked()));
    // 页面布局
    m_pUnitGridLayout = new QHBoxLayout(m_pUnitGroup);

    m_pUnitGridLayout->addWidget(m_pUnitViewBtn, 0);
    m_pUnitGridLayout->addSpacing(20);
    m_pUnitGridLayout->addWidget(m_pSimulateBtn, 1);
    m_pUnitGridLayout->addWidget(m_pUnitMinusBtn, 2);
    m_pUnitGridLayout->addWidget(m_pUnitLcdNum, 3);
    m_pUnitGridLayout->addWidget(m_pUnitPlusBtn, 4);
    m_pUnitGridLayout->addSpacing(30);
    m_pUnitGridLayout->addWidget(m_pCaptureBtn, 5);
    m_pUnitGridLayout->addSpacing(30);
    m_pUnitGridLayout->addWidget(m_pAutoAnalysisBtn, 6);

    m_pUnitViewBtn->setFixedSize(btnSize);
    m_pUnitViewBtn->setFixedWidth(SMALL_BTN_WIDTH+20);
    m_pUnitPlusBtn->setFixedSize(btnSize);
    m_pUnitMinusBtn->setFixedSize(btnSize);
    m_pCaptureBtn->setFixedSize(btnSize);
    m_pSimulateBtn->setFixedSize(SMALL_BTN_WIDTH+20, BTN_HEIGHT);
    m_pAutoAnalysisBtn->setFixedSize(btnSize);
    m_pVideoLayout = new QVBoxLayout;
    m_pVideoLayout->addWidget(m_pVideoLabel);
    m_pVideoLayout->addSpacing(10);
    m_pVideoLayout->addWidget(m_pVideoLabel2);
    m_pVideoLayout->addStretch();
    m_pVideoLayout->addWidget(m_pUnitGroup);
    m_pVideoLayout->setContentsMargins(0, 0, 0, 0);

    downLay->addWidget(infoLbl);
    downLay->addStretch();
    downLay->addWidget(setBackButton);


    m_RowBtn->hide();
    m_RowMinusBtn->hide();
    m_RowPlusBtn->hide();
    m_Row->hide();

    Lay->addLayout(m_pVideoLayout);
    Lay->addLayout(downLay);
    Lay->addSpacing(10);

    setLayout(Lay);
}

MyVideo::~MyVideo()
{

}

/* 上中下层或者前后视切换按钮 */
void MyVideo::onUnitViewClicked()
{
    struGsh.nUnit += (struGsh.nUnit%2 == 0) ? 1 : -1;
    if (struGsh.nUnit%2 == 0) {
        m_pUnitViewBtn->setText(myLan.front_view);
    } else {
        m_pUnitViewBtn->setText(myLan.rear_view);
    }
    emit sBoardNumChanged();
}

/* 板号增加按钮 */
void MyVideo::onUnitPlusBtnClicked()
{
    int idTotal = 0;
    switch (struCnfg.struLevelInfo[struGsh.nLevel].nViewTotal) {
    case 1:     // 单视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal*2;
        break;
    case 2:     // 双视
        idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
        break;
	case 4:
		idTotal = struCnfg.struLevelInfo[struGsh.nLevel].nUnitLevelTotal;
		break;
    }

    if (struGsh.nUnit < idTotal-2) {
        struGsh.nUnit += 2;
        emit sBoardNumChanged();
    }
    m_pUnitLcdNum->display(struGsh.nUnit/2+1);
}

/* 板号减少按钮 */
void MyVideo::onUnitMinusBtnClicked()
{
    if (struGsh.nUnit > 1) {
        struGsh.nUnit -= 2;
        emit sBoardNumChanged();
    }
    m_pUnitLcdNum->display(struGsh.nUnit/2+1);
}

/* 信号开始/暂停按钮 */
void MyVideo::onCaptureBtnClicked()
{
    m_bIsCapturing = !m_bIsCapturing;
    startCapture(m_bIsCapturing);
}

//计算实时碎米率
void MyVideo::startCalSmallMatPerRSC(bool bIsCapturing)
{
    if (bIsCapturing) {
        m_pCalSmallMatThread->start();
    } else {
        if (m_pCalSmallMatThread->isRunning()) {
            m_pCalSmallMatThread->stop();
        }
    }
}

void MyVideo::startCapture(bool bIsCapturing)
{
    m_bIsCapturing = bIsCapturing;
           QByteArray img;
       if (m_bIsCapturing) {
           infoWidget->setLabelText(myLan.video_starting_info);
           infoWidget->delayShow();
           //启动视频
           QByteArray args;
           args.clear();
           args[0] = 1;
           //清空读图片缓冲区
           int ret1 = MyUpd.clear_udp_buffer(MyUpd.getSockfd());
           ImageQueue.clear();
           std::cout << "已清空缓冲区字节：" << ret1 << " 字节" << std::endl;
           MyUpd.writeDatagram(CMD_AI_IMG_VID_UPLOAD,struGsh.nUnit/2, 1, args, struGsh.addressList.at(struGsh.nUnit/2), AI_UDP_SEND_PORT);
           myFlow.msleep(50);
           MySerial.com1Write(0x0111, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0, 0, 1, 3);
           if (!isThreadRunning()) {
               m_pThread->start();
           }
   //        myFlow.sleep(2);
   //		int Times = 5;
   //        while (Times > 0) {
   //            if(!ImageQueue.isEmpty()) {
   //				break;
   //            } else {
   //				myFlow.sleep(1);
   //				Times--;
   //			}
   //		}
           myFlow.msleep(100);

           infoWidget->hide();
       } else {
           if (isThreadRunning()) {
               m_pThread->stop();
               infoWidget->setLabelText(myLan.video_stoping_info);
               infoWidget->delayShow();
               //启动视频
               MySerial.com1Write(0x0111, UNIT, struGsh.nLevel, struGsh.nUnit, 0, 0, 0, 0, 0, 3);
               myFlow.sleep(2);
               QByteArray args;
               args.clear();
               args[0] = 2;
               MyUpd.writeDatagram(CMD_AI_IMG_VID_UPLOAD,struGsh.nUnit/2, 1, args, struGsh.addressList.at(struGsh.nUnit/2), AI_UDP_SEND_PORT);
               infoWidget->hide();
           }
           if (!ImageQueue.isEmpty()) {
               imageQMutex.lock();
               ImageQueue.clear();
               imageQMutex.unlock();
           }
       }

       resetCaptureState(m_bIsCapturing);

       while (m_bIsCapturing) {
           if (!ImageQueue.isEmpty()) {
               imageQMutex.lock();
               img = ImageQueue.dequeue();
               imageQMutex.unlock();
               updateVideoImage(img);
               myFlow.msleep(50);
           } else {
               myFlow.msleep(10);
           }
       }
}


void MyVideo::resetCaptureMode(bool bIsUsb)
{
	int nCapMode = bIsUsb;
//    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
//		MySerial.com1Write(CMD_INT_IMAGE_CAPTURE_MODE, INT, i, 0, 0, 0, 0, 0, nCapMode, 3);
//	}
}

/*!
 * \brief 获取线程状态
 * \return  当前线程状态
 */
bool MyVideo::isThreadRunning()
{
    return m_pThread->isRunning();
}

void MyVideo::updateVideoImageQueue(QByteArray img)
{
	ImageQueue.enqueue(img);
    if (ImageQueue.size() > 3) {
        imageQMutex.lock();
        ImageQueue.dequeue();
        imageQMutex.unlock();
    }
}

void MyVideo::updateVideoImage(QByteArray img)
{
    QImage  mirrorImg;
    QPixmap pixmap1;
    QString infoStr;

//    //! 判断产量及含杂信息的合法性
//    //! 产量不大于36000kg/h 含杂不大于100%
//    if (g_fRealThroughPut > 0.01) {
//        g_fRealThroughPut = 0.01;
//    }
//    if (g_fRealDirtPer > 1.0) {
//        g_fRealDirtPer = 1.0;
//    }

//    if (struCnfe.nProduct == OPT_PRODUCT_ANCOO_RSC) {
//        infoStr = QString(myLan.real_output + ": %1 (kg/h)\t\t"
//                          +myLan.real_dirtper + ": %2%\t\t"
//                          +myLan.broken_rice_ratio + ": %3%")
//                .arg(g_fRealThroughPut*3600*1000, 0, 'f', 2)
//                .arg(g_fRealDirtPer*100, 0, 'f', 2)
//                .arg(g_fRealSmallMatPer, 0, 'f', 2);
//    } else {
//      switch(struCnfg.nLang){
//      case LANG_CHS:
//          infoStr.sprintf("产量: %.2f (kg/h)\t\t\t含杂: %.2f%%",
//                          g_fRealThroughPut*3600*1000,
//                          g_fRealDirtPer*100);
//          break;
//      case LANG_ENG:
//          infoStr.sprintf("Output: %.2f (kg/h)\t\t\tBad ratio: %.2f%%",
//                          g_fRealThroughPut*3600*1000,
//                          g_fRealDirtPer*100);
//          break;
//      default:
//          infoStr.sprintf("Output: %.2f (kg/h)\t\t\tBad ratio: %.2f%%",
//                          g_fRealThroughPut*3600*1000,
//                          g_fRealDirtPer*100);
//          break;
//           }

//    }
//    infoLbl->setText(infoStr);

//    if (struGsh.nUnit%2 == 0) {
//        mirrorImg = img.mirrored(true, false);
//    } else {
//        mirrorImg = img.mirrored(false, false);
//    }


    if (!mirrorImg.loadFromData(img, "JPEG")) {
        qWarning("Failed to load image from JPEG data");
        return;
    }

#ifdef Q_OS_UNIX
//    if (img != QImage(g_blank)) {
        /*传感器分辨率为1024显示整幅图片，为2048取整幅图片的左半个1024显示*/
//        imgs = mirrorImg.scaled(QSize(VIDEO_IMG_WID, VIDEO_IMG_HEI*2), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//        img1 = imgs.copy(0, 0, 1024, g_nRow);
//        img2 = imgs.copy(1024, 0, VIDEO_IMG_WID-1024, g_nRow);
        pixmap1 = QPixmap::fromImage(mirrorImg);

        m_pVideoLabel->setPixmap(pixmap1.scaled(1000,500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        m_pVideoLabel2->hide();
        return;
//    }
#endif

//    imgs = mirrorImg.scaled(QSize(VIDEO_IMG_WID, VIDEO_ALL_IMG_HEI), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

//    for (int i = 0; i < UPDATE_TIMES; i++) {
//        if (m_bIsCapturing == false) {
//            break;
//        }

//        //! 传感器分辨力为1024显示整幅图片，为2048分辨率取整幅图片的左半个1024显示
//        img1 = imgs.copy(0, VIDEO_ALL_IMG_HEI-g_nRow-i*g_nRow/8, 1024, g_nRow);

//        if (!img.isNull()) {
//            pixmap1 = QPixmap::fromImage(img1);

//            m_pVideoLabel->setPixmap(pixmap1.scaled(m_pVideoLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//            m_pVideoLabel2->hide();
//        }
//        myFlow.msleep(10);
//    }
}

void MyVideo::updateVideoWidget()
{
    updateVideo();
}

void MyVideo::updateBoardNum()
{
    if (m_bIsCapturing && ImageQueue.size() > 1) {
        imageQMutex.lock();
        ImageQueue.dequeue();
        imageQMutex.unlock();
    }
}

void MyVideo::resetCaptureState(bool bState)
{
	if (bState) {
        m_pCaptureBtn->setIcon(myIcon.Media_Pause);
	} else {
        m_pCaptureBtn->setIcon(myIcon.Media_Start);
	}

}

void MyVideo::onSetBackBtnClicked()
{
    startCapture(false);
    myFlow.initInterfaceTransMode(0);
    emit backToHomePageSig();
}

void MyVideo::onRowPlusBtnClicked()
{
    if(g_nRow < 256){
        g_nRow += 64;
    }
    m_Row->display(g_nRow);
    m_pVideoLabel->setFixedSize(1000, 500);
    m_pVideoLabel2->setFixedSize(1000, 500);
//    updateVideoImage(QImage(g_blank));
}

void MyVideo::onRowMinusBtnClicked()
{
    if(g_nRow > 64){
        g_nRow -= 64;
    }
    m_Row->display(g_nRow);
    m_pVideoLabel->setFixedSize(1000, 500);
    m_pVideoLabel2->setFixedSize(1000, 500);
//    updateVideoImage(QImage(g_blank));
}

void MyVideo::onSimulateBtnClicked()
{
    g_bIsSim = !g_bIsSim;
    for (int i = 0; i < struCnfg.nLevelTotal; i++) {
        for (int j = 0; j < struCnfg.struLevelInfo[i].nUnitLevelTotal; j++) {
            int nAddr = struCnfg.struLevelInfo[i].nUnitId[j];
//            MySerial.com1Write(CMD_UNIT_ONOFF, UNIT, i, nAddr, 0, 0, 0, 0, g_bIsSim, 3);
        }
    }
    if(g_bIsSim) {
        m_pSimulateBtn->setRedColor(GREEN);
    } else {
        m_pSimulateBtn->setRedColor(DEF);
    }
}

void MyVideo::onAutoAnalysisBtnClicked()
{
    if (m_bIsCapturing) {
        m_bOriCapStat = m_bIsCapturing;
        m_bIsCapturing = false;
        m_pThread->stop();
        infoWidget->setLabelText(myLan.video_stoping_info);
        infoWidget->delayShow();
        myFlow.sleep(4);
        infoWidget->hide();
        resetCaptureState(false);
    } else {
        m_bOriCapStat = m_bIsCapturing;
    }
    emit goToAutoAnalysisSig();
}

/*!
 * \brief 视频图像信息刷新 
 *
 */
void MyVideo::updateVideo()
{
    resetLocalParams();
    // video image
//    updateVideoImage(QImage(g_blank));

    // unit view
    m_pUnitLcdNum->display(struGsh.nUnit / 2 + 1);
    m_pUnitViewBtn->show();
    if (struGsh.nUnit%2 == 1) {
        m_pUnitViewBtn->setText(myLan.rear_view);
    } else {
        m_pUnitViewBtn->setText(myLan.front_view);
    }
    if (m_bIsCapturing) {
        m_pCaptureBtn->setIcon(myIcon.Media_Pause);
    } else {
        m_pCaptureBtn->setIcon(myIcon.Media_Start);
    }

    m_pUnitGroup->show();
}

void MyVideo::resetLocalParams()
{
    g_fRealThroughPut = 0;
    g_fRealDirtPer = 0;
    m_bIsViewOrUnitChanged = false;
}
