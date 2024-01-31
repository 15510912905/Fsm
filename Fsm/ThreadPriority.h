///////////////////////////////////////////////////////////////////////////
/// @file     ThreadPriority.h
/// @brief    
///
/// @author   7094/DaiXiaoDong
/// @date     2021年10月28日
/// @version  0.1
///
/// @par Copyright(c):
///     2015 迈克医疗电子有限公司，All rights reserved.
///
/// @par History:
/// @li 7094/DaiXiaoDong，2021年10月28日，新建文件
///
///////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <thread>

#ifndef _WIN32
#include <sched.h>
#else
#define SCHED_FIFO 1
#define SCHED_RR 2
#endif

#define RT_BIND_RUN_CPU 1 //绑定CPU
#define RT_SCHED_STRATEGY SCHED_FIFO //抢占式调度策略
#define RT_SCHED_DEFAULT_STRATEGY SCHED_RR //时间片调度策略
#define RT_MAX_PRI 99 //实时调度最大优先级
#define RT_MIN_PRI 1 //实时调度最小优先级

class ThreadPriority : public std::thread
{
public:
    
    ///
    /// @brief  构造优先级的线程
    ///
    /// @param[in]  iPriority  线程优先级，线程优先级范围[1,99],1-最小优先级，99-最大优先级
    /// @param[in]  args  std::thread接收的参数
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年10月28日，新建函数
    ///
    template<typename... Args>
    ThreadPriority(int iPriority, Args&&... args)
        : std::thread(std::forward<Args>(args)...)
    {
#ifndef _WIN32

        if ((iPriority < sched_get_priority_min(RT_SCHED_STRATEGY)) || (iPriority > sched_get_priority_max(RT_SCHED_STRATEGY)))
        {
            iPriority = sched_get_priority_min(RT_SCHED_STRATEGY);
            std::cout << "[error]线程优先级参数错误" << std::endl;
        }

        SetBindCpu(RT_BIND_RUN_CPU);
        SetScheduling(RT_SCHED_STRATEGY, iPriority);

#endif
    }

    ///
    /// @brief  绑定线程在指定CPU
    ///
    /// @param[in]  iCpu  CPU的编号
    ///
    /// @return 当前对象
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月2日，新建函数
    ///
    ThreadPriority& SetBindCpu(int iCpu)
    {
#ifndef _WIN32

        cpu_set_t setmask;

        CPU_ZERO(&setmask);
        CPU_SET(iCpu, &setmask);//绑定在指定CPU
        if (pthread_setaffinity_np(native_handle(), sizeof(setmask), &setmask) < 0)
        {
            std::cout << "[error]绑定线程到指定CPU失败" << std::endl;
            std::exit(0);
        }

#endif

        return *this;
    }

    ///
    /// @brief  判断线程是否在指定的CPU上运行
    ///
    /// @param[in]  iCpu  CPU的编号
    ///
    /// @return true-在指定CPU上运行  false-不在指定CPU运行
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月2日，新建函数
    ///
    bool IsBindCpu(int iCpu)
    {
#ifndef _WIN32

        cpu_set_t getmask;

        CPU_ZERO(&getmask);
        if (pthread_getaffinity_np(native_handle(), sizeof(getmask), &getmask) < 0)
        {
            std::cout << "[error]获取线程绑定CPU信息失败" << std::endl;
        }

        if (CPU_ISSET(iCpu, &getmask))
        {
            return true;
        }

#endif

        return false;
    }

    ///
    /// @brief  设置线程的调度策略和线程优先级
    ///
    /// @param[in]  iPolicy    调度策略[SCHED_OTHER分时, SCHED_FIFO实时, SCHED_RR实时时间片轮转]
    /// @param[in]  iPriority  线程优先级，线程优先级范围[1,99],1-最小优先级，99-最大优先级
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    ThreadPriority& SetScheduling(int iPolicy, int iPriority)
    {
#ifndef _WIN32

        struct sched_param setparam;

        setparam.__sched_priority = iPriority;
        if (pthread_setschedparam(native_handle(), iPolicy, &setparam))
        {
            std::cout << "[error]设置线程调度策略及优先级失败" << std::endl;
            std::exit(0);
        }

#endif

        return *this;
    }

    ///
    /// @brief  设置线程的优先级
    ///
    /// @param[in]  iPriority  线程优先级，线程优先级范围[1,99],1-最小优先级，99-最大优先级
    ///
    /// @return 当前对象
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月2日，新建函数
    ///
    ThreadPriority& SetThreadPriority(int iPriority)
    {
#ifndef _WIN32

        if (pthread_setschedprio(native_handle(), iPriority) < 0)
        {
            std::cout << "[error]设置线程优先级失败" << std::endl;
        }

#endif

        return *this;
    }

    ///
    /// @brief  返回线程的优先级
    ///
    ///
    /// @return 线程优先级
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月2日，新建函数
    ///
    int GetThreadPriority()
    {
#ifndef _WIN32

        struct sched_param getparam = { 0 };
        int policy;

        if (pthread_getschedparam(native_handle(), &policy, &getparam))
        {
            std::cout << "[error]获取线程优先级失败" << std::endl;
        }

        return getparam.__sched_priority;

#else

        return 0;

#endif
    }

    ///
    /// @brief  返回线程调度策略
    ///
    ///
    /// @return 调度策略[SCHED_OTHER分时, SCHED_FIFO实时, SCHED_RR实时时间片轮转]
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    int GetPolicy()
    {
        int policy = 0;
#ifndef _WIN32

        struct sched_param getparam;

        if (pthread_getschedparam(native_handle(), &policy, &getparam))
        {
            std::cout << "[error]获取线程调度策略失败" << std::endl;
        }

#endif

        return policy;
    }

    ///
    /// @brief  静态函数-绑定线程在指定CPU
    ///
    /// @param[in]  iCpu  CPU的编号
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static void StaticSetBindCpu(int iCpu)
    {
#ifndef _WIN32

        cpu_set_t setmask;

        CPU_ZERO(&setmask);
        CPU_SET(iCpu, &setmask);//绑定在指定CPU
        if (pthread_setaffinity_np(pthread_self(), sizeof(setmask), &setmask) < 0)
        {
            std::cout << "[error]绑定线程到指定CPU失败" << std::endl;
        }

#endif
    }

    ///
    /// @brief  静态函数-判断线程是否在指定的CPU上运行
    ///
    /// @param[in]  iCpu  CPU的编号
    ///
    /// @return true-在指定CPU上运行  false-不在指定CPU运行
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static bool StaticIsBindCpu(int iCpu)
    {
#ifndef _WIN32

        cpu_set_t getmask;

        CPU_ZERO(&getmask);
        if (pthread_getaffinity_np(pthread_self(), sizeof(getmask), &getmask) < 0)
        {
            std::cout << "[error]获取线程绑定CPU信息失败" << std::endl;
        }

        if (CPU_ISSET(iCpu, &getmask))
        {
            return true;
        }

#endif

        return false;
    }

    ///
    /// @brief  静态函数-设置线程的调度策略和线程优先级
    ///
    /// @param[in]  iPolicy    调度策略[SCHED_OTHER分时, SCHED_FIFO实时, SCHED_RR实时时间片轮转]
    /// @param[in]  iPriority  线程优先级，线程优先级范围[1,99],1-最小优先级，99-最大优先级
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static void StaticSetScheduling(int iPolicy, int iPriority)
    {
#ifndef _WIN32

        struct sched_param setparam;

        setparam.__sched_priority = iPriority;
        if (pthread_setschedparam(pthread_self(), iPolicy, &setparam))
        {
            std::cout << "[error]设置线程调度策略及优先级失败" << std::endl;
        }

#endif
    }

    ///
    /// @brief  静态函数-设置线程的优先级
    ///
    /// @param[in]  iPriority  线程优先级，线程优先级范围[1,99],1-最小优先级，99-最大优先级
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static void StaticSetThreadPriority(int iPriority)
    {
#ifndef _WIN32

        if (pthread_setschedprio(pthread_self(), iPriority) < 0)
        {
            std::cout << "[error]设置线程优先级失败" << std::endl;
        }

#endif
    }

    ///
    /// @brief  静态函数-获取线程的优先级
    ///
    ///
    /// @return 线程优先级，线程优先级范围[1,99],1-最小优先级，99-最大优先级
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static int StaticGetThreadPriority()
    {
#ifndef _WIN32

        struct sched_param getparam = { 0 };
        int policy;

        if (pthread_getschedparam(pthread_self(), &policy, &getparam))
        {
            std::cout << "[error]获取线程优先级失败" << std::endl;
        }

        return getparam.__sched_priority;

#else

        return 0;

#endif
    }

    ///
    /// @brief  静态函数-返回线程调度策略
    ///
    ///
    /// @return 调度策略[SCHED_OTHER分时, SCHED_FIFO实时, SCHED_RR实时时间片轮转]
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static int StaticGetPolicy()
    {
        int policy = 0;
#ifndef _WIN32

        struct sched_param getparam;

        if (pthread_getschedparam(pthread_self(), &policy, &getparam))
        {
            std::cout << "[error]获取线程调度策略失败" << std::endl;
        }

#endif

        return policy;
    }

    ///
    /// @brief  静态函数-绑定进程在指定的CPU
    ///
    /// @param[in]  iCpu  CPU的编号
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static void StaticBindProcessToCpu(int iCpu)
    {
#ifndef _WIN32

        cpu_set_t setmask;

        CPU_ZERO(&setmask);
        CPU_SET(iCpu, &setmask);
        if (sched_setaffinity(0, sizeof(setmask), &setmask) < 0)
        {
            std::cout << "[error]绑定进程到指定CPU失败" << std::endl;
            std::exit(0);
        }

#endif
    }

    ///
    /// @brief  静态函数-判断进程是否在指定的CPU上运行
    ///
    /// @param[in]  iCpu  CPU的编号
    ///
    /// @return true-在指定CPU上运行  false-不在指定CPU运行
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static bool  StaticIsProcessBindCpu(int iCpu)
    {
#ifndef _WIN32

        cpu_set_t getmask;

        CPU_ZERO(&getmask);
        if (-1 == sched_getaffinity(0, sizeof(getmask), &getmask))
        {
            std::cout << "[error]获取进程绑定CPU信息失败" << std::endl;
        }

        if (CPU_ISSET(iCpu, &getmask))
        {
            return true;
        }

#endif

        return false;
    }

    ///
    /// @brief  静态函数-设置进程的调度策略和进程优先级
    ///
    /// @param[in]  iPolicy  调度策略[SCHED_OTHER分时, SCHED_FIFO实时, SCHED_RR实时时间片轮转]
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static void StaticSetProcessScheduling(int iPolicy)
    {
#ifndef _WIN32

        int maxpri;
        struct sched_param setparam;

        maxpri = sched_get_priority_max(iPolicy);
        setparam.__sched_priority = maxpri;
        if (-1 == sched_setscheduler(0, iPolicy, &setparam))
        {
            std::cout << "[error]进程设置调度策略及优先级失败" << std::endl;
            std::exit(0);
        }

#endif
    }

    ///
    /// @brief  静态函数-获取进程的调度策略
    ///
    ///
    /// @return 调度策略[SCHED_OTHER分时, SCHED_FIFO实时, SCHED_RR实时时间片轮转]
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static int StaticGetProcessPolicy()
    {
        int policy = 0;
#ifndef _WIN32

        policy = sched_getscheduler(0);

#endif

        return policy;
    }

    ///
    /// @brief  静态函数-获取进程优先级
    ///
    ///
    /// @return 进程优先级，进程优先级范围[1,99],1-最小优先级，99-最大优先级
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月3日，新建函数
    ///
    static int StaticGetProcessPriority()
    {
#ifndef _WIN32

        struct sched_param getparam = { 0 };

        if (-1 == sched_getparam(0, &getparam))
        {
            std::cout << "[error]获取进程调度优先级错误" << std::endl;
        }

        return getparam.__sched_priority;

#else

        return 0;

#endif
    }

    ///
    /// @brief  设置pthread_create创建线程的默认属性
    ///
    ///
    /// @return 
    ///
    /// @par History:
    /// @li 7094/DaiXiaoDong，2021年11月5日，新建函数
    ///
    static void StaticSetAllThreadAttr()
    {
#ifndef _WIN32

        pthread_attr_t defaultattr;
        cpu_set_t setmask;
        struct sched_param setparam;

        if (0 != pthread_getattr_default_np(&defaultattr))
        {
            std::cout << "[error]获取默认线程属性失败" << std::endl;
        }
        
        CPU_ZERO(&setmask);
        CPU_SET(RT_BIND_RUN_CPU, &setmask);
        if (0 != pthread_attr_setaffinity_np(&defaultattr, sizeof(setmask), &setmask))
        {
            std::cout << "[error]设置默认线程属性绑定CPU失败" << std::endl;
        }

        if (0 != pthread_attr_setschedpolicy(&defaultattr, RT_SCHED_STRATEGY))
        {
            std::cout << "[error]设置默认线程属性调度策略失败" << std::endl;
        }

        setparam.__sched_priority = sched_get_priority_min(RT_SCHED_STRATEGY);
        if (0 != pthread_attr_setschedparam(&defaultattr, &setparam))
        {
            std::cout << "[error]设置默认线程属性优先级失败" << std::endl;
        }

        if (0 != pthread_attr_setscope(&defaultattr, PTHREAD_SCOPE_SYSTEM))
        {
            std::cout << "[error]设置默认线程属性范围失败" << std::endl;
        }

        if (0 != pthread_attr_setinheritsched(&defaultattr, PTHREAD_EXPLICIT_SCHED))
        {
            std::cout << "[error]设置默认线程属性使用自己的属性失败" << std::endl;
        }

        if (0 != pthread_setattr_default_np(&defaultattr))
        {
            std::cout << "[error]设置pthread_create默认线程属性失败" << std::endl;
        }

#endif
    }

};

