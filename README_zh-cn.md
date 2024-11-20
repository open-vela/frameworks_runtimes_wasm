# Wasm 模块

[简体中文|[English](./README.md)]

Openvela Wasm 框架提供了额外的模块和 SDK 来构建 `WebAssembly` 应用程序。

## 项目结构

```tree
├── chre
├── libc
├── memory
├── vela-sysroot
├── vendor
├── README.md
└── README_zh-cn.md
```

* `chre`：该模块提供了从 Context Hub Runtime Environment 到 Wasm 运行时的功能。
* `libc`: 用于 Wasm 的 C 标准库。
* `memory`: Wasm 和原生代码之间的内存访问管理。
* `vela-sysroot`: 用于构建 WebAssembly 应用程序的额外头文件集合。
* `vendor`: 第三方芯片供应商的 Wasm 应用程序 SDK。

## 如何使用

### Wasm 运行时的功能模块

目前仅支持 WAMR（WebAssembly Micro Runtime），您如果要使用该功能模块:
1. 在 `menuconfig` 中启用 `WAMR`。
2. 在相应的子菜单中启用 `WASM_LIBC`（libc）或 `WASM_MEMORY_OPERATIONS`（memory）。
3. libc 或 memory 的功能可在 `iwasm` 命令行工具中使用，并可以直接在 Wasm 应用程序中使用。

### 使用 SDK 构建 Wasm 应用程序

`vendor` 目录提供了第三方芯片供应商的 SDK 集合，目前仅支持 `bouffalo lab` 的 BL616。要使用 SDK 构建应用程序，您可以参考以下文档：[BL616 SDK](./vendor/bouffalo/README.md)
