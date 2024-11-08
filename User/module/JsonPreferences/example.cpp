#include <iostream>
#include "JsonPreferences.h"

int JsonPreferences_test()
{
    // 创建 JsonPreferences 对象
    JsonPreferences prefs;

    // 存储各种类型的值
    prefs.putChar("char_key", 'A');
    prefs.putUChar("uchar_key", 255);
    prefs.putShort("short_key", -32768);
    prefs.putUShort("ushort_key", 65535);
    prefs.putInt("int_key", -2147483648);
    prefs.putUInt("uint_key", 4294967295);
    prefs.putLong("long_key", -2147483648);
    prefs.putULong("ulong_key", 4294967295);
    prefs.putLong64("long64_key", -9223372036854775808LL);
    prefs.putULong64("ulong64_key", 18446744073709551615ULL);
    prefs.putFloat("float_key", 3.14f);
    prefs.putDouble("double_key", 3.14159265358979323846);
    prefs.putBool("bool_key", true);
    prefs.putString("string_key", "Hello, World!");

    // 读取并打印各种类型的值
    std::cout << "char_key: " << prefs.getChar("char_key") << std::endl;
    std::cout << "uchar_key: " << static_cast<int>(prefs.getUChar("uchar_key")) << std::endl;
    std::cout << "short_key: " << prefs.getShort("short_key") << std::endl;
    std::cout << "ushort_key: " << prefs.getUShort("ushort_key") << std::endl;
    std::cout << "int_key: " << prefs.getInt("int_key") << std::endl;
    std::cout << "uint_key: " << prefs.getUInt("uint_key") << std::endl;
    std::cout << "long_key: " << prefs.getLong("long_key") << std::endl;
    std::cout << "ulong_key: " << prefs.getULong("ulong_key") << std::endl;
    std::cout << "long64_key: " << prefs.getLong64("long64_key") << std::endl;
    std::cout << "ulong64_key: " << prefs.getULong64("ulong64_key") << std::endl;
    std::cout << "float_key: " << prefs.getFloat("float_key") << std::endl;
    std::cout << "double_key: " << prefs.getDouble("double_key") << std::endl;
    std::cout << "bool_key: " << prefs.getBool("bool_key") << std::endl;
    std::cout << "string_key: " << prefs.getString("string_key") << std::endl;

    // 检查键是否存在
    std::cout << "Is 'char_key' present? " << prefs.isKey("char_key") << std::endl;
    std::cout << "Is 'nonexistent_key' present? " << prefs.isKey("nonexistent_key") << std::endl;

    // 删除一个键
    prefs.remove("char_key");
    std::cout << "Is 'char_key' present after removal? " << prefs.isKey("char_key") << std::endl;

    // 清除所有键值对
    prefs.clear();
    std::cout << "Is 'int_key' present after clear? " << prefs.isKey("int_key") << std::endl;

    // 打印 JSON 字符串表示
    std::cout << "JSON string representation: " << prefs.toString() << std::endl;

    return 0;
}