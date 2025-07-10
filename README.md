# COBRA-SNAKE


# Modular Snake Robot System 🐍

本项目为一套基于 STM32H745 的多自由度、模块化、可重构蛇形机器人系统，集成了机械结构设计、分布式控制、激光建图与上位机交互功能，面向复杂环境中的地面移动与空间感知任务。

## 🚀 项目特色

- 三种可切换形态：`SNAKE_LINE`（蛇形）、`SNAKE_CAR`（车形）、`SNAKE_SCAN`（激光建图模式）
- 分布式控制架构：主控 STM32H745 + 多个 F103 电调节点，基于 FDCAN 通信
- 上位机 UI：基于 Python + NiceGUI 的图形控制界面，支持 `value` / `slider` / `keyboard` 控制模式
- 激光建图：搭载 RPLIDAR C1，运行 Hector Mapping 实现 2D 实时建图
- 模块化设计：关节、电机、主控、头尾等结构热插拔，支持拓展摄像头、红外、超声等传感器

## 🗂️ 项目结构

```bash
.
├── hardware/              # 原理图、PCB 设计文件
├── firmware/
│   ├── H745/              # H745 主控工程
│   └── F103/              # F103 电调固件（极简）
├── ui/                    # 上位机 NiceGUI 控制界面
├── mapping/               # 激光雷达建图配置与 Hector Mapping 部署
├── docs/                  # 项目说明文档与流程图、演示图片
└── README.md
````

## ⚙️ 环境配置

* **硬件平台**：

  * 主控：STM32H745ZIT6
  * 电调：STM32F103RBT6
  * 激光雷达：RPLIDAR C1
* **软件依赖**：

  * STM32CubeIDE / Keil MDK
  * Python 3.10+
  * `nicegui` (`pip install nicegui`)
  * ROS Noetic（推荐 Ubuntu 20.04）

## 🧪 快速启动

### 1. 编译 H745 主控固件

```bash
cd H745/
# 使用 ARM-Keil v5 打开并编译工程
```

### 2. 启动上位机 UI 控制界面

```bash
cd ui/
python main.py
```

### 3. 激光雷达建图（Hector Mapping）

```bash
cd mapping/
roslaunch hector_slam.launch
```

## 📷 项目演示图

| SNAKE\_LINE 模式     | SNAKE\_CAR 模式     | SNAKE\_SCAN 模式     |
| ------------------ | ----------------- | ------------------ |
| ![](docs/_DSC4436.jpg) | ![](docs/_DSC4493.jpg) | ![](docs/_DSC4537.jpg) |

## 📌 TODO

* [ ] 加入摄像头视觉功能
* [ ] 支持 ROS2 框架接入
* [ ] 动作函数自动生成与轨迹规划模块

## 📄 License

本项目仅用于学习与研究用途，暂未开放正式开源许可。如需引用或商用，请联系作者获得授权。

