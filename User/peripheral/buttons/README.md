# 跟焦器按键功能

## 1 概述

实现对硬件上所有按键的扫描，并提供绑定功能，将按键事件绑定到用户回调函数上。

## 2 按键功能

设备上共集成9个按键

### 2.1 正面按键

分布在跟焦器遥控器（Hand Unit）正面6个，具体见下图。

<img src=".\..\..\..\Images\hand_unit1.png" alt="hand_unit1" style="zoom:50%;" />

图中按键的功能分别为：

- ```左上```：```REC```，录制按键
- ```中上```：```F1```，功能按键1
- ```右上```：```F2``` ```OK```，功能按键2和确认按键
- ```左下```：```MENU```，菜单按键
- ```中下```：```POWER```，电源按键
- ```右下```：```LOCK```，锁定按键

### 2.2 握持处按键（User Buttons）

分布在左手握持处3个按键，具体见下图。

<img src=".\..\..\..\Images\hand_unit2.png" alt="hand_unit2" style="zoom:50%;" />

图中按键的功能分别为：

- ```左```：```F3```，功能按键3
- ```右```：```F4```，功能按键4
- ```下```：```F5```，功能按键5

## 3 实现要求

按键的检测等功能，采用ukeypad库中相关的keyboard模块实现，具体见[ukeypad](https://gitee.com/lyworkspace/ukeypad.git)。

## 4 接口要求

提供用户事件绑定功能，可以将用户的事件请求和按键事件绑定在一起，当按键事件发生时，调用用户事件请求。