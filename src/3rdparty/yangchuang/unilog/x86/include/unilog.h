/**
*  @file     ILog.h
*  @brief    日志接口类定义.
*
*  系统中仅存在唯一的一个实例，在系统启动时实例化.\n
*  负责日志的初始化及写入
*
*  暂仅支持linux
*
*/

#ifndef UNI_ILog_H
#define UNI_ILog_H

#ifdef WIN32
#ifdef LOG_DLL_EXPORT
#define LOG_DLL_API __declspec(dllexport)
#else
#define LOG_DLL_API __declspec(dllimport)
#endif // LOG_DLL_EXPORT
#else
#define LOG_DLL_API
#endif // WIN32

#include <stdio.h>
#include <string>
#include <sstream>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif
	namespace uni
	{
		//! 类前向声明
		class ILogger;
		class ILog;

		/**
		*@brief 文件名处理
		*@param  file 带路径的文件名
		*@return 文件名
		*@note 仅限本库使用，如果源文件中存在重名的文件，日志中将不能区分
		*/
		LOG_DLL_API const char* FileName(const char* file);

		/*!
		* @brief log类型枚举
		*/
		typedef enum _log_type
		{
			LLT_TRACE = 1,            //!< 追踪类型日志
			LLT_DEBUG = 2,            //!< 调试类型日志
			LLT_INFO = 3,            //!< 记录类型日志
			LLT_WARN = 4,            //!< 警告类型日志
			LLT_ERROR = 5             //!< 错误类型日志
		}LOG_TYPE;

		/*!
		*@name log相关宏定义，格式化输出
		*@{
		*/
#define LOG_INFO_MAX_LEN            (5000)          //!< 日志内容长度

		//! 初始化宏定义
#define LOG_INIT(path) uni::ILog::GetInstance().Init(path);

//! TRACE日志带参数输出
#define LOG_TRACE_FMT(format,...)                                       \
        do {                                                                \
        char buf[LOG_INFO_MAX_LEN] = { 0 };                                 \
        snprintf(buf, LOG_INFO_MAX_LEN - 1, format, ##__VA_ARGS__);         \
        std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
        uni::ILog::GetInstance().Log(uni::LLT_TRACE, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
        } while(0)                                                          \

	//! DEBUG日志带参数输出
#define LOG_DEBUG_FMT(format,...)                                       \
        do {                                                                \
        char buf[LOG_INFO_MAX_LEN] = { 0 };                                 \
        snprintf(buf, LOG_INFO_MAX_LEN - 1, format, ##__VA_ARGS__);         \
        std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
        uni::ILog::GetInstance().Log(uni::LLT_DEBUG, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
        } while (0)                                                         \

	//! INFO日志带参数输出
#define LOG_INFO_FMT(format,...)                                        \
        do {                                                                \
        char buf[LOG_INFO_MAX_LEN] = { 0 };                                 \
        snprintf(buf, LOG_INFO_MAX_LEN - 1, format, ##__VA_ARGS__);         \
        std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
        uni::ILog::GetInstance().Log(uni::LLT_INFO, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
        } while (0)                                                         \

	//! WARNING日志带参数输出
#define LOG_WARN_FMT(format,...)                                        \
        do {                                                                \
        char buf[LOG_INFO_MAX_LEN] = { 0 };                                 \
        snprintf(buf, LOG_INFO_MAX_LEN - 1, format, ##__VA_ARGS__);         \
        std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
        uni::ILog::GetInstance().Log(uni::LLT_WARN, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
        } while (0)                                                         \

	//! ERROR日志带参数输出
#define LOG_ERROR_FMT(format,...)                                       \
        do {                                                                \
        char buf[LOG_INFO_MAX_LEN] = { 0 };                                 \
        snprintf(buf, LOG_INFO_MAX_LEN - 1, format, ##__VA_ARGS__);         \
        std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
        uni::ILog::GetInstance().Log(uni::LLT_ERROR, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
        } while (0)                                                         \

	//! 无参数输出共通
#define LOG_UNFMT(buf, level)                                               \
        do {                                                                \
        uni::LOG_TYPE log_type = uni::ILog::GetInstance().GetLogType();     \
            if(level < log_type) {                                          \
		        break;                                                      \
			}                                                               \
		std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
        uni::ILog::GetInstance().Log(level, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
        } while (0)                                                         \

	//! 无参日志输出
#define LOG_TRACE(buf)      LOG_UNFMT(buf, uni::LLT_TRACE)
#define LOG_DEBUG(buf)      LOG_UNFMT(buf, uni::LLT_DEBUG)
#define LOG_INFO(buf)       LOG_UNFMT(buf, uni::LLT_INFO)
#define LOG_WARN(buf)       LOG_UNFMT(buf, uni::LLT_WARN)
#define LOG_ERROR(buf)      LOG_UNFMT(buf, uni::LLT_ERROR)

//! 流方式输出共通
#define LOG_STREAM(msg, level)                                              \
        do {                                                                \
		    uni::LOG_TYPE log_type = uni::ILog::GetInstance().GetLogType(); \
            if(level < log_type) {                                          \
		        break;                                                      \
			}                                                               \
		    std::stringstream os_buffer;                                    \
			os_buffer << __FUNCTION__ << "() " << msg;                      \
			uni::ILog::GetInstance().Log(level, uni::FileName(__FILE__), __LINE__, os_buffer.str().c_str());        \
		}  while(0)                                                         \

	//! 流方式输出
#define LOG_TRACE_STM(msg)          LOG_STREAM(msg, uni::LLT_TRACE)
#define LOG_DEBUG_STM(msg)          LOG_STREAM(msg, uni::LLT_DEBUG)
#define LOG_INFO_STM(msg)           LOG_STREAM(msg, uni::LLT_INFO)
#define LOG_WARN_STM(msg)           LOG_STREAM(msg, uni::LLT_WARN)
#define LOG_ERROR_STM(msg)          LOG_STREAM(msg, uni::LLT_ERROR)
/** @} */


			/*!
			* @brief ILog类
			* 本地环境管理类，为其他模块提供本地环境的访问接口.
			*/
		class LOG_DLL_API ILog
		{
		public:
			/*!
			*@brief 初始化
			*@param logCfg 配置文件名，含路径
			*@return void
			*/
			void Init(std::string logCfg = "");

			/*!
			*@brief 获取系统唯一实例
			*@param  void
			*@return 唯一实例
			*/
			static ILog& GetInstance();

			/**
			*@brief 日志记录
			*@param level 日志类型
			*@param file 文件名
			*@param line 行数
			*@param msg 日志内容
			*@return void
			*/
			void Log(LOG_TYPE level, const char* file, int line, const char* msg);

			LOG_TYPE GetLogType();

		private:
			// 私有化构造、析构函数
			ILog();
			~ILog();

		private:
			ILogger* m_pImp;                //!< 实现者
			LOG_TYPE m_log_level_{ LOG_TYPE::LLT_INFO };
		};

		/**
		* @brief CChronoLog类
		* 时间差记录类，用于打印语句运行耗时.
		* @note 对象销毁时打印的是该对象创建到销毁时间差，log中文件名、函数名、行号为该对象析构函数对应信息
		*/
		class LOG_DLL_API CChronoLog
		{
		public:
			/**
			*@brief 构造
			*@param flg 打印标记语
			*@param endLog 对象销毁打印从创建到销毁时间差日志标志
			*@param type 日志等级
			*@return void
			*/
			CChronoLog(std::string flg, bool endLog = false, LOG_TYPE level = LLT_DEBUG);
			~CChronoLog();

			CChronoLog() = delete;
			//! 禁止拷贝
			CChronoLog(const CChronoLog&) = delete;
			CChronoLog& operator=(const CChronoLog&) = delete;

		public:
			std::chrono::steady_clock::time_point m_start;      //!< 起点
			std::string m_flg;                                  //!< 打印标记语
			LOG_TYPE m_level;                                   //!< 日志等级

		private:
			bool m_endLog;                                      //!< 销毁对象打印创建到销毁时间差日志标志
		};
	}

#ifdef __cplusplus
}
#endif

//! 逝去时间打印-带参数
#define CHRONO_LOG_FMT(obj, subMsg)                                                 \
    do {                                                                \
    char buf[LOG_INFO_MAX_LEN] = { 0 };                                 \
    std::string strSubMsg(subMsg);                                      \
    snprintf(buf, LOG_INFO_MAX_LEN - 1, "%s %s %ld ms", obj.m_flg.c_str(), strSubMsg.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - obj.m_start).count()); \
    std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
    uni::ILog::GetInstance().Log(obj.m_level, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
    } while (0)                                                         \


//! 逝去时间打印-不带参数
#define CHRONO_LOG(obj)                                                 \
    do {                                                                \
    char buf[LOG_INFO_MAX_LEN] = { 0 };                                 \
    snprintf(buf, LOG_INFO_MAX_LEN - 1, "%s %ld ms", obj.m_flg.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - obj.m_start).count()); \
    std::string msg = __FUNCTION__; msg += "() "; msg += buf;           \
    uni::ILog::GetInstance().Log(obj.m_level, uni::FileName(__FILE__), __LINE__, msg.c_str());        \
    } while (0)                                                         \

#endif // TULOG_TULOG_H
