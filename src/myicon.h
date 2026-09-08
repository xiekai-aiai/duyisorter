#ifndef MYICON_H
#define MYICON_H

#include <QtCore>
#include <QIcon>

class MyIcon
{
public:
    MyIcon();
    void init(void);    //! 初始化

    //! Action
    QIcon Action_Apply_Icon;
    QIcon Action_Apply;
    QIcon Action_Back;
    QIcon Action_Backup;
    QIcon Action_Cancel;
    QIcon Action_Circle;
    QIcon Action_Clean;
    QIcon Action_Configure;
    QIcon Action_Minus;
    QIcon Action_OneKey;
    QIcon Action_Parameter;
    QIcon Action_Plus;
    QIcon Action_Refresh;
    QIcon Action_Restore;
    QIcon Action_Select;

    //! Arrow
    QIcon Arrow_Down;
    QIcon Arrow_DownFast;
    QIcon Arrow_Left;
    QIcon Arrow_LeftFast;
    QIcon Arrow_LeftFirst;
    QIcon Arrow_Right;
    QIcon Arrow_RightFast;
    QIcon Arrow_Up;
    QIcon Arrow_UpFast;

    //! Chart
    QIcon Chart_Bar;
    QIcon Chart_Dot;
    QIcon Chart_Line;

    //! Device
    QIcon Device_Camera;
    QIcon Device_Chip;
    QIcon Device_Network;
    QIcon Device_NetworkError;

    //! Edit
    QIcon Edit_Copy;
    QIcon Edit_Delete;
    QIcon Edit_List;
    QIcon Edit_Redo;
    QIcon Edit_Rename;
    QIcon Edit_Save;
    QIcon Edit_Search;
    QIcon Edit_ZoomIn;
    QIcon Edit_ZoomOut;

    //! Flag
    QIcon Flag_BD;
    QIcon Flag_BG;
    QIcon Flag_CN;
    QIcon Flag_ES;
    QIcon Flag_FR;
    QIcon Flag_GB;
    QIcon Flag_MM;
    QIcon Flag_ID;
    QIcon Flag_PL;
    QIcon Flag_IR;
    QIcon Flag_KR;
    QIcon Flag_PT;
    QIcon Flag_RU;
    QIcon Flag_SA;
    QIcon Flag_SK;
    QIcon Flag_TH;
    QIcon Flag_TR;
    QIcon Flag_VN;

    //! Input
    QIcon Input_Backspace;
    QIcon Input_Language;

    //! Mark
    QIcon Mark_Blue;
    QIcon Mark_Green;
    QIcon Mark_Red;

    //! Media
    QIcon Media_Pause;
    QIcon Media_Start;
    QIcon Media_Stop;
    QIcon Media_Video;

    //! Message
    QIcon Message_Error;
    QIcon Message_Help;
    QIcon Message_Info;
    QIcon Message_Warning;

    //! Path
    QIcon Path_Favorite;
    QIcon Path_Globe;
    QIcon Path_Home;

    //! Sorter
    QIcon Sorter;
    QIcon Sorter_AI;
    QIcon Sorter_BootMode;
    QIcon Sorter_CameraInfra;
    QIcon Sorter_CameraRGB;
    QIcon Sorter_Degaussing;
    QIcon Sorter_Ejector;
    QIcon Sorter_EjectorStatistics;
    QIcon Sorter_EjectorTest;
    QIcon Sorter_EjectorTime;
    QIcon Sorter_Feeder;
    QIcon Sorter_FeederTest;
    QIcon Sorter_Light;
    QIcon Sorter_SchemeManage;
    QIcon Sorter_SchemeParameter;
    QIcon Sorter_SchemeSelect;
    QIcon Sorter_Shape;
    QIcon Sorter_RGB;
    QIcon Sorter_WipeManual;
    QIcon Sorter_WipeTime;

    //! System
    QIcon System_Calendar;
    QIcon System_Clock;
    QIcon System_Contacts;
    QIcon System_Lock;
    QIcon System_Remote;
    QIcon System_ScreenSaver;
    QIcon System_Shutdown;
    QIcon System_TSCalibrate;
    QIcon System_Unlock;

    //! Update
    QIcon Update_Chip;
    QIcon Update_LCD;
};

extern MyIcon myIcon;

#endif // MYICON_H
