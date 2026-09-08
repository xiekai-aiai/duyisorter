/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        CSingletonBase.h
 * \brief       单例模式类头文件
 * \date        2015.01.14
 */
#ifndef CSINGLETONBASE_H
#define CSINGLETONBASE_H

template<class T>

/*!
 * \brief 单例模式类
 */
class CSingletonBase
{
public:
    inline static T* getInstance(void)
    {
      static T  m_pt;
      return &m_pt;
    }

private:

    CSingletonBase(const CSingletonBase& sig){}

    CSingletonBase& operator = (const CSingletonBase& sig){}

protected:

    CSingletonBase(){}

    virtual ~CSingletonBase(){}
};

#endif // CSINGLETONBASE_H
