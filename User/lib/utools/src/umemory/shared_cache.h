#ifndef __UTOOLS_SHARED_CACHE_H__
#define __UTOOLS_SHARED_CACHE_H__

#include "../utools_cfg.h"

#include <unordered_map>
#include <memory>
#include <type_traits>

#include "../collab/utmutex.h"
#include "../collab/utcondition_variable.h"

namespace utools::mem
{
    /// @brief 数据共享缓存类
    /// @tparam KEY_TYPE key类型
    template <typename KEY_TYPE>
    class SharedCache
    {
    public:
        typedef KEY_TYPE key_type;                // 键类型
        typedef std::shared_ptr<void> value_type; // 值类型

        SharedCache() = default;
        virtual ~SharedCache() = default;

        /// @brief 判断键值是否存在，如果不存在增加一个数据，返之直接更新缓存中的数据
        /// @tparam T 数据类型
        /// @param key 键
        /// @param data 数据
        /// @note 如果数据存在，直接更新缓存中的数据，如果不存在，添加到缓存中
        template <typename T>
        void put(const key_type &key, T &&data)
        {
            auto it = __caches.find(key);
            if (it != __caches.end())
            {
                it->second = std::static_pointer_cast<void>(
                    std::make_shared<typename std::decay<T>::type>(std::forward<T>(data)));
            }
            else
            {
                __caches[key] = std::make_shared<typename std::decay<T>::type>(
                    std::forward<T>(data));
            }
        }

        /// @brief 获取数据
        /// @tparam T 数据类型
        /// @param key 键
        /// @return std::shared_ptr<T> 数据，如果不存在返回空指针
        template <typename T>
        std::shared_ptr<T> get_ptr(const key_type &key) const
        {
            auto it = __caches.find(key);
            if (it != __caches.end())
            {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr; // 未找到
        }

        /// @brief 获取数据
        /// @tparam T 数据类型
        /// @param key 键
        /// @return T& 数据，如果不存在返回默认值
        template <typename T>
        T &get(const key_type &key) const
        {
            auto it = __caches.find(key);
            if (it != __caches.end())
            {
                return *std::static_pointer_cast<T>(it->second);
            }
            return T(); // 未找到
        }

        /// @brief 从缓存中获取数据
        /// @tparam T 数据类型
        /// @param key 键
        /// @param def 如果没有找到，返回默认值
        /// @return T& 数据
        template <typename T>
        T &get(const key_type &key, const T &def) const
        {
            auto existing_ptr = get_ptr<T>(key);
            if (existing_ptr)
            {
                return *existing_ptr;
            }
            return def;
        }

        /// @brief 直接从缓存中获取数据
        /// @tparam T 数据类型
        /// @param key 键
        /// @return T7 数据
        /// @warning 如果数据不存在，会导致异常，调用者需要保证数据存在
        template <typename T>
        T &get_direct(const key_type &key) const
        {
            return *std::static_pointer_cast<T>(__caches.at(key));
        }

        /// @brief 从缓存中删除数据
        /// @param key 键
        void del(const key_type &key)
        {
            __caches.erase(key);
        }

        /// @brief 判断缓存中是否存在指定键的数据
        /// @param key 键
        /// @return bool 存在返回true，否则返回false
        bool contains(const key_type &key) const
        {
            return __caches.find(key) != __caches.end();
        }

    private:
        std::unordered_map<key_type, value_type> __caches; // 共享缓存
    };

    /// @brief 线程安全的数据共享缓存类
    /// @tparam KEY_TYPE key类型
    template <typename KEY_TYPE>
    class SharedCacheSafety : public SharedCache<KEY_TYPE>
    {
    public:
        typedef typename SharedCache<KEY_TYPE>::key_type key_type;
        typedef typename SharedCache<KEY_TYPE>::value_type value_type;
        using SharedCache<KEY_TYPE>::SharedCache;

        template <typename T>
        void put(const key_type &key, T &&data)
        {
            ::utools::collab::LockGuard lock(__mutex);
            return SharedCache<KEY_TYPE>::put(key, std::forward<T>(data));
        }

        template <typename T>
        std::shared_ptr<T> get_ptr(const key_type &key) const
        {
            ::utools::collab::LockGuard lock(__mutex);
            return SharedCache<KEY_TYPE>::get_ptr(key);
        }

        template <typename T>
        T &get(const key_type &key) const
        {
            ::utools::collab::LockGuard lock(__mutex);
            return SharedCache<KEY_TYPE>::get(key);
        }

        template <typename T>
        T &get(const key_type &key, const T &def) const
        {
            ::utools::collab::LockGuard lock(__mutex);
            return SharedCache<KEY_TYPE>::get(key, def);
        }

        template <typename T>
        T &get_direct(const key_type &key) const
        {
            ::utools::collab::LockGuard lock(__mutex);
            return SharedCache<KEY_TYPE>::get_direct(key);
        }

        void del(const key_type &key)
        {
            ::utools::collab::LockGuard lock(__mutex);
            return SharedCache<KEY_TYPE>::del(key);
        }

        bool contains(const key_type &key) const
        {
            ::utools::collab::LockGuard lock(__mutex);
            return SharedCache<KEY_TYPE>::contains(key);
        }

    private:
        ::utools::collab::Mutex __mutex; // 保护共享缓存的互斥锁
    };
}

#endif // __UTOOLS_SHARED_CACHE_H__
