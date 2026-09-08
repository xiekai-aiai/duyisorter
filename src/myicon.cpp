#include "myicon.h"

MyIcon myIcon;

MyIcon::MyIcon()
{
}

void MyIcon::init()
{
    //! Action
//    Action_Apply = QIcon(":/res/png/Action_Apply.png");
//    Action_Back = QIcon(":/res/png/Action_Back.png");
    Action_Apply_Icon = QIcon(":/res/png/Action_Apply.png");

    Action_Apply = QIcon("");
    Action_Back = QIcon("");
    Action_Backup = QIcon(":/res/png/Action_Backup.png");
//    Action_Cancel = QIcon(":/res/png/Action_Cancel.png");
    Action_Cancel = QIcon();

    Action_Circle = QIcon(":/res/png/Action_Circle.png");
    Action_Clean = QIcon(":/res/png/Action_Clean.png");
    Action_Configure = QIcon(":/res/png/Action_Configure.png");
    Action_Minus = QIcon(":/res/png/Action_Minus.png");
    Action_OneKey = QIcon(":/res/png/Action_OneKey.png");
    Action_Parameter = QIcon(":/res/png/Action_Parameter.png");
    Action_Plus = QIcon(":/res/png/Action_Plus.png");
    Action_Refresh = QIcon(":/res/png/Action_Refresh.png");
    Action_Restore = QIcon(":/res/png/Action_Restore.png");
    Action_Select = QIcon(":/res/png/Action_Select.png");

    //! Arrow
    Arrow_Down = QIcon(":/res/png/Arrow_Down.png");
    Arrow_DownFast = QIcon(":/res/png/Arrow_DownFast.png");
    Arrow_Left = QIcon(":/res/png/Arrow_Left.png");
    Arrow_LeftFirst = QIcon(":/res/png/Arrow_LeftFirst.png");
    Arrow_LeftFast = QIcon(":/res/png/Arrow_LeftFast.png");
    Arrow_Right = QIcon(":/res/png/Arrow_Right.png");
    Arrow_RightFast = QIcon(":/res/png/Arrow_RightFast.png");
    Arrow_Up = QIcon(":/res/png/Arrow_Up.png");
    Arrow_UpFast = QIcon(":/res/png/Arrow_UpFast.png");

    //! Chart
    Chart_Bar = QIcon(":/res/png/Chart_Bar.png");
    Chart_Dot = QIcon(":/res/png/Chart_Dot.png");
    Chart_Line = QIcon(":/res/png/Chart_Line.png");

    //! Device
    Device_Camera = QIcon(":/res/png/Device_Camera.png");
    Device_Chip = QIcon(":/res/png/Device_Chip.png");
    Device_Network = QIcon(":/res/png/Device_Network.png");
    Device_NetworkError = QIcon(":/res/png/Device_NetworkError.png");

    //! Edit
    Edit_Copy = QIcon(":/res/png/Edit_Copy.png");
    Edit_Delete = QIcon(":/res/png/Edit_Delete.png");
    Edit_List = QIcon(":/res/png/Edit_List.png");
    Edit_Redo = QIcon(":/res/png/Edit_Redo.png");
    Edit_Rename = QIcon(":/res/png/Edit_Rename.png");
    Edit_Save = QIcon(":/res/png/Edit_Save.png");
    Edit_Search = QIcon(":/res/png/Edit_Search.png");
    Edit_ZoomIn = QIcon(":/res/png/Edit_ZoomIn.png");
    Edit_ZoomOut = QIcon(":/res/png/Edit_ZoomOut.png");

    //! Flag
    Flag_BD = QIcon(":/res/png/Flag_BD.png");
    Flag_BG = QIcon(":/res/png/Flag_BG.png");
    Flag_CN = QIcon(":/res/png/Flag_CN.png");
    Flag_ES = QIcon(":/res/png/Flag_ES.png");
    Flag_FR = QIcon(":/res/png/Flag_FR.png");
    Flag_GB = QIcon(":/res/png/Flag_GB.png");
    Flag_MM = QIcon(":/res/png/Flag_MM.png");
    Flag_ID = QIcon(":/res/png/Flag_ID.png");
    Flag_PL = QIcon(":/res/png/Flag_PL.png");
    Flag_IR = QIcon(":/res/png/Flag_IR.png");
    Flag_KR = QIcon(":/res/png/Flag_KR.png");
    Flag_PT = QIcon(":/res/png/Flag_PT.png");
    Flag_RU = QIcon(":/res/png/Flag_RU.png");
    Flag_SA = QIcon(":/res/png/Flag_SA.png");
    Flag_SK = QIcon(":/res/png/Flag_SK.png");
    Flag_TH = QIcon(":/res/png/Flag_TH.png");
    Flag_TR = QIcon(":/res/png/Flag_TR.png");
    Flag_VN = QIcon(":/res/png/Flag_VN.png");

    //! Input
    Input_Backspace = QIcon(":/res/png/Input_Backspace.png");
    Input_Language = QIcon(":/res/png/Input_Language.png");

    //! Mark
    Mark_Blue = QIcon(":/res/png/Mark_Blue.png");
    Mark_Green = QIcon(":/res/png/Mark_Green.png");
    Mark_Red = QIcon(":/res/png/Mark_Red.png");

    //! Media
    Media_Pause = QIcon(":/res/png/Media_Pause.png");
    Media_Start = QIcon(":/res/png/Media_Start.png");
    Media_Stop = QIcon(":/res/png/Media_Stop.png");
    Media_Video = QIcon(":/res/png/Media_Video.png");

    //! Message
    Message_Error = QIcon(":/res/png/Message_Error.png");
    Message_Help = QIcon(":/res/png/Message_Help.png");
    Message_Info = QIcon(":/res/png/Message_Info.png");
    Message_Warning = QIcon(":/res/png/Message_Warning.png");

    //! Path
    Path_Favorite = QIcon(":/res/png/Path_Favorite.png");
    Path_Globe = QIcon(":/res/png/Path_Globe.png");
    Path_Home = QIcon(":/res/png/Path_Home.png");

    //! Sorter
    Sorter = QIcon(":/res/png/Sorter.png");
    Sorter_AI = QIcon(":/res/png/Sorter_AI.png");
    Sorter_BootMode = QIcon(":/res/png/Sorter_BootMode.png");
    Sorter_CameraInfra = QIcon(":/res/png/Sorter_CameraInfra.png");
    Sorter_CameraRGB = QIcon(":/res/png/Sorter_CameraRGB.png");
    Sorter_Degaussing = QIcon(":/res/png/Sorter_Degaussing.png");
    Sorter_Ejector = QIcon(":/res/png/Sorter_Ejector.png");
    Sorter_EjectorStatistics = QIcon(":/res/png/Sorter_EjectorStatistics.png");
    Sorter_EjectorTest = QIcon(":/res/png/Sorter_EjectorTest.png");
    Sorter_EjectorTime = QIcon(":/res/png/Sorter_EjectorTime.png");
    Sorter_Feeder = QIcon(":/res/png/Sorter_Feeder.png");
    Sorter_FeederTest = QIcon(":/res/png/Sorter_FeederTest.png");
    Sorter_Light = QIcon(":/res/png/Sorter_Light.png");
    Sorter_RGB = QIcon(":/res/png/Sorter_RGB.png");
    Sorter_SchemeManage = QIcon(":/res/png/Sorter_SchemeManage.png");
    Sorter_SchemeParameter = QIcon(":/res/png/Sorter_SchemeParameter.png");
    Sorter_SchemeSelect = QIcon(":/res/png/Sorter_SchemeSelect.png");
    Sorter_Shape = QIcon(":/res/png/Sorter_Shape.png");
    Sorter_WipeManual = QIcon(":/res/png/Sorter_WipeManual.png");
    Sorter_WipeTime = QIcon(":/res/png/Sorter_WipeTime.png");

    //! System
    System_Calendar = QIcon(":/res/png/System_Calendar.png");
    System_Clock = QIcon(":/res/png/System_Clock.png");
    System_Contacts = QIcon(":/res/png/System_Contacts.png");
    System_Lock = QIcon(":/res/png/System_Lock.png");
    System_Remote = QIcon(":/res/png/System_Remote.png");
    System_ScreenSaver = QIcon(":/res/png/System_ScreenSaver.png");
    System_Shutdown = QIcon(":/res/png/System_Shutdown.png");
    System_TSCalibrate = QIcon(":/res/png/System_TSCalibrate.png");
    System_Unlock = QIcon(":/res/png/System_Unlock.png");

    //! Update
    Update_Chip = QIcon(":/res/png/Update_Chip.png");
    Update_LCD = QIcon(":/res/png/Update_LCD.png");
}
