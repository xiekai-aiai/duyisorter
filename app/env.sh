export QTEDIR=/opt/qt5.15.15
export LD_LIBRARY_PATH=/opt/app/lib:/opt/qt5.15.15/lib:$LD_LIBRARY_PATH
export QT_QPA_PLATFORM_PLUGIN_PATH=$QTEDIR/plugins
export QT_QPA_FONTDIR=/opt/app/lib/fonts
export QT_QPA_PLATFORM=xcb
export QT_PLUGIN_DEBUG=1           # 插件加载日志
export QT_IMAGE_DEBUG=1            # 图片加载/渲染日志（专属JPG问题）
