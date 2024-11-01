
# 用户功能文件夹

## 概述

## 目录结构

整个程序分为三个部分，分别是

`peripheral`：外设层，所有传感器等外设驱动程序，并提供相关的数据接口

`router`：路由层，提供相关的路由功能，主要为`SharedCacheSafety`和`AsyncPubSub`的功能

`module`：模块层，用于管理用户功能，如UI、通讯等

`app_entry.h`：为程序入口，负责初始化程序，并启动各个模块

`app_version.h`：程序版本信息

除主程序外，还有如下目录用于支持

`lib`：存放第三方库

`asset`：存放静态资源
