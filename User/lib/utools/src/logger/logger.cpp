#include "../utools_cfg.h"
#include "logger.h"
#include <stdio.h>

namespace utools
{
    std::function<void(const char *)> logger::__print_fun{[](const char *c_str)
                                                          { printf(c_str); }};
    std::array<bool, 7> logger::__log_levels{false, true, true, true, true, true, true};
    std::string logger::__end_str{"\n"};
    std::string logger::__space_str{" "};

#if UTOOLS_LOGGER_FILTER_ENABLE == UTOOLS_TRUE // 开启过滤功能后可用
    std::vector<std::string> logger::__filter_keywords{};
    ::utools::collab::Mutex logger::__filter_mutex{};
#endif

#if UTOOLS_LOGGER_ASYNC_ENABLE == UTOOLS_TRUE // 开启异步功能后可用
    ::utools::collab::SyncQueue<std::string, 100> logger::__async_print_queue{};
    bool logger::__is_running{false};
    ::utools::collab::Task logger::__async_print_task;
#endif
}