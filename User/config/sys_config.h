#ifndef __SYS_CONFIG__
#define __SYS_CONFIG__
#include "json.hpp"
#include <fstream>
using json = nlohmann::json;
json custom_config =
    {{"version", 0.001}, {"name", "hand_unit_main_ctrller"}};

std::string custom_config_string = custom_config.dump();

#endif // !__SYS_CONFIG__
