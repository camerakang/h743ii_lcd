#ifndef __UTOOLS_UTMUTEX_H__
#define __UTOOLS_UTMUTEX_H__

#include "../utools_cfg.h"

#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
#include <mutex>
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
#include <pthread.h>
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
#include UTOOLS_FREERTOS_INCLUDE_FREERTOS_H
#include UTOOLS_FREERTOS_INCLUDE_TASK_H
#include UTOOLS_FREERTOS_INCLUDE_SEMAPHORE_H
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
#include "cmsis_os.h"
#endif

namespace utools::collab
{
    class Mutex
    {
    public:
        Mutex()
        {
#if UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            pthread_mutex_init(&mutex, nullptr);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            mutex = xSemaphoreCreateMutex();
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            mutex_attr.name = std::to_string(mutex_num++).c_str();
            mutex = osMutexNew(&mutex_attr);
#endif
        }

        ~Mutex()
        {
#if UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            pthread_mutex_destroy(&mutex);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            vSemaphoreDelete(mutex);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            osMutexDelete(mutex);
#endif
        }

        void lock()
        {
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            mtx.lock();
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            pthread_mutex_lock(&mutex);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            xSemaphoreTake(mutex, portMAX_DELAY);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            osMutexAcquire(mutex, osWaitForever);
#endif
        }

        void unlock()
        {
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            mtx.unlock();
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            pthread_mutex_unlock(&mutex);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            xSemaphoreGive(mutex);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            osMutexRelease(mutex);
#endif
        }

    private:
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
        std::mutex mtx;
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
        pthread_mutex_t mutex;
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
        SemaphoreHandle_t mutex;
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
        osMutexAttr_t mutex_attr;
        osMutexId_t mutex;
        inline static int32_t mutex_num{0};
#endif
    };

    /// @brief
    class LockGuard
    {
    public:
        LockGuard(Mutex &mtx) : mutex(mtx)
        {
            mutex.lock();
        }

        ~LockGuard()
        {
            mutex.unlock();
        }

    private:
        Mutex &mutex;
    };

    /// @brief 互斥锁类
    class UniqueLock
    {
    public:
        UniqueLock(Mutex &mtx) : mutex(mtx), ownsLock(true)
        {
            mutex.lock();
        }

        UniqueLock(Mutex &mtx, bool acquire) : mutex(mtx), ownsLock(acquire)
        {
            if (acquire)
            {
                mutex.lock();
            }
        }

        ~UniqueLock()
        {
            if (ownsLock)
            {
                mutex.unlock();
            }
        }

        void lock()
        {
            if (!ownsLock)
            {
                mutex.lock();
                ownsLock = true;
            }
        }

        void unlock()
        {
            if (ownsLock)
            {
                mutex.unlock();
                ownsLock = false;
            }
        }

        bool owns() const
        {
            return ownsLock;
        }

    private:
        Mutex &mutex;
        bool ownsLock;
    };
}

#endif // __UTOOLS_UTMUTEX_H__