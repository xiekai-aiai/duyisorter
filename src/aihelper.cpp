#include "aihelper.h"
#include "sortertypes.h"
#include <QDir>


namespace ai_helper
{

    /**
     * @brief: 根据索引获取AI设备的IP地址
     * @return {*}
     */
    QString GetAiIpByIndex(int index)
    {
        return QString(AI_DEV_IP_FORMAT).arg(AI_DEV_ONE_IP + index);
    }

    /**
     * @brief: 获取sqlite数据库路径
     * @return {*}
     */
    QString GetDBPath()
    {
        QString path = QCoreApplication::applicationDirPath() + "/userdata/db/sorter.db";
        return path;
    }

    /**
     * @brief: 获取本地图片采集的根目录
     * @return {*}
     */
    QString GetAcqImgRootPath()
    {
        QString path = QCoreApplication::applicationDirPath() + "/userdata/image/";
        return path;
    }
};
