export QTEDIR=/opt/qt5.15.15
export LD_LIBRARY_PATH=/opt/app/lib:/opt/qt5.15.15/lib:$LD_LIBRARY_PATH
export QT_QPA_PLATFORM_PLUGIN_PATH=$QTEDIR/plugins
export QT_QPA_FONTDIR=/opt/app/lib/fonts
export QT_QPA_PLATFORM=xcb
export QT_PLUGIN_DEBUG=1           # 插件加载日志
export QT_IMAGE_DEBUG=1            # 图片加载/渲染日志（专属JPG问题）

# 确保 SVM 训练程序有可执行权限
# （OTA 升级包/上位机 SFTP 拷贝可能丢失执行位，导致 AIClassComputer 调用返回 126）
if [ -f /opt/app/norm_train ]; then
    chmod 755 /opt/app/norm_train 2>/dev/null
fi
