/// @brief 键盘键位定义
#ifndef __KEYMAP_H__
#define __KEYMAP_H__

#define MOUSE_LEFT 0x1        // 鼠标左键
#define MOUSE_RIGHT 0x2       // 鼠标右键
#define MOUSE_CANCEL 0x3      // CANCEL 键
#define MOUSE_MIDDLE 0x4      // 鼠标中键
#define KEY_BACKSPACE 0x8     // BACKSPACE 键（退格键）
#define KEY_TAB 0x9           // TAB 键
#define KEY_CLEAR 0xC         // CLEAR 键
#define KEY_ENTER 0xD         // ENTER 键
#define KEY_SHIFT 0x10        // SHIFT 键
#define KEY_CTRL 0x11         // CTRL 键
#define KEY_MENU 0x12         // MENU 键
#define KEY_PAUSE 0x13        // PAUSE 键
#define KEY_CAPS_LOCK 0x14    // CAPS LOCK 键
#define KEY_ESC 0x1B          // ESC 键
#define KEY_SPACEBAR 0x20     // SPACEBAR 键
#define KEY_PAGE_UP 0x21      // PAGE UP 键
#define KEY_PAGE_DOWN 0x22    // PAGE DOWN 键
#define KEY_END 0x23          // END 键
#define KEY_HOME 0x24         // HOME 键
#define KEY_LEFT_ARROW 0x25   // LEFT ARROW 键
#define KEY_UP_ARROW 0x26     // UP ARROW 键
#define KEY_RIGHT_ARROW 0x27  // RIGHT ARROW 键
#define KEY_DOWN_ARROW 0x28   // DOWN ARROW 键
#define KEY_SELECT 0x29       // SELECT 键
#define KEY_PRINT_SCREEN 0x2A // PRINT SCREEN 键
#define KEY_EXECUTE 0x2B      // EXECUTE 键
#define KEY_SNAPSHOT 0x2C     // SNAPSHOT 键
#define KEY_INSERT 0x2D       // INSERT 键
#define KEY_DELETE 0x2E       // DELETE 键
#define KEY_HELP 0x2F         // HELP 键
#define KEY_NUM_LOCK 0x90     // NUM LOCK 键

/// @brief A 至 Z 键与 A – Z 字母的 ASCII 码相同：
#define KEY_A 65 // A 键
#define KEY_B 66 // B 键
#define KEY_C 67 // C 键
#define KEY_D 68 // D 键
#define KEY_E 69 // E 键
#define KEY_F 70 // F 键
#define KEY_G 71 // G 键
#define KEY_H 72 // H 键
#define KEY_I 73 // I 键
#define KEY_J 74 // J 键
#define KEY_K 75 // K 键
#define KEY_L 76 // L 键
#define KEY_M 77 // M 键
#define KEY_N 78 // N 键
#define KEY_O 79 // O 键
#define KEY_P 80 // P 键
#define KEY_Q 81 // Q 键
#define KEY_R 82 // R 键
#define KEY_S 83 // S 键
#define KEY_T 84 // T 键
#define KEY_U 85 // U 键
#define KEY_V 86 // V 键
#define KEY_W 87 // W 键
#define KEY_X 88 // X 键
#define KEY_Y 89 // Y 键
#define KEY_Z 90 // Z 键

/// @brief 0 至 9 键与数字 0 – 9 的 ASCII 码相同：
#define KEY_0 48 // 0 键
#define KEY_1 49 // 1 键
#define KEY_2 50 // 2 键
#define KEY_3 51 // 3 键
#define KEY_4 52 // 4 键
#define KEY_5 53 // 5 键
#define KEY_6 54 // 6 键
#define KEY_7 55 // 7 键
#define KEY_8 56 // 8 键
#define KEY_9 57 // 9 键

/// @brief 下列常数代表数字键盘上的键：
#define KEY_SKP_0 0x60         // 0 键
#define KEY_SKP_1 0x61         // 1 键
#define KEY_SKP_2 0x62         // 2 键
#define KEY_SKP_3 0x63         // 3 键
#define KEY_SKP_4 0x64         // 4 键
#define KEY_SKP_5 0x65         // 5 键
#define KEY_SKP_6 0x66         // 6 键
#define KEY_SKP_7 0x67         // 7 键
#define KEY_SKP_8 0x68         // 8 键
#define KEY_SKP_9 0x69         // 9 键
#define KEY_MULTIPLE_SIGN 0x6A // MULTIPLICATION SIGN(*) 键
#define KEY_PLUS_SIGN 0x6B     // PLUS SIGN(+) 键
#define KEY_SKP_ENTER 0x6C     // ENTER 键
#define KEY_MINUS_SIGN 0x6D    // MINUS SIGN(–) 键
#define KEY_DECIMAL_POINT 0x6E // DECIMAL POINT(.) 键
#define KEY_DIVISION_SIGN 0x6F //  DIVISION SIGN(/) 键

/// @brief 下列常数代表功能键：
#define KEY_F1 0x70  // F1 键
#define KEY_F2 0x71  // F2 键
#define KEY_F3 0x72  // F3 键
#define KEY_F4 0x73  // F4 键
#define KEY_F5 0x74  // F5 键
#define KEY_F6 0x75  // F6 键
#define KEY_F7 0x76  // F7 键
#define KEY_F8 0x77  // F8 键
#define KEY_F9 0x78  // F9 键
#define KEY_F10 0x79 // F10 键
#define KEY_F11 0x7A // F11 键
#define KEY_F12 0x7B // F12 键
#define KEY_F13 0x7C // F13 键
#define KEY_F14 0x7D // F14 键
#define KEY_F15 0x7E // F15 键
#define KEY_F16 0x7F // F16 键

/// @brief 多媒体键
#define KEY_VOLUME_UP 175   // 音量增加
#define KEY_VOLUME_DOWN 174 // 音量减小
#define KEY_MID_STOP 179    // 停止
#define KEY_VOLUME_MUTE 173 // 静音
#define KEY_BROWSER 172     // 浏览器
#define KEY_EMAIL 180       // 邮件
#define KEY_SEARCH 170      // 搜索
#define KEY_FAVORITE 171    // 收藏

#endif // __KEYMAP_H__
