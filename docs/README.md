## 项目框架结构介绍：基于哈夫曼编码的压缩软件

本项目旨在实现一个利用哈夫曼编码算法的文件压缩与解压工具。项目采用模块化设计，确保软件的健壮性、解耦性和可重构性。以下将详细介绍项目的框架结构，包括目录布局、模块描述。

### 目录结构

```
HuffmanZip/
├── Counter.h
├── HuffmanTree.h
├── HuffmanTreeNode.h
├── HuffmanZip.h
├── Counter.cpp
├── HuffmanTree.cpp
├── HuffmanTreeNode.cpp
├── HuffmanZip.cpp
├── main.cpp
├── README.md
```

### 模块概述

1. **Counter 模块 (`Counter.h` & `Counter.cpp`)**
    - **职责**：统计输入文件中各字符的出现频率。
    - **核心类**：`Counter`
    - **关键方法**：
        - `getCountMap(const std::string& filename)`: 读取指定文件并返回字符出现频率的映射。

2. **HuffmanTreeNode 模块 (`HuffmanTreeNode.h` & `HuffmanTreeNode.cpp`)**
    - **职责**：定义哈夫曼树的节点，支持节点间的比较与序列化/反序列化操作。
    - **核心类**：`HuffmanTreeNode`
    - **关键方法**：
        - 构造函数：支持多种方式初始化节点。
        - 重载运算符：允许节点之间进行比较（`<`, `>`, `==`, `<=`, `>=`）。
        - `serialize()`: 将节点及其子树序列化为字符串。
        - `deserialize(const std::string &s)`: 从序列化字符串中重建节点及其子树。
        - 静态辅助方法：`writeCharMap` 和 `writeCodeMap` 用于构建字符与编码的映射表。

3. **HuffmanTree 模块 (`HuffmanTree.h` & `HuffmanTree.cpp`)**
    - **职责**：构建和管理哈夫曼树，提供字符编码与解码的映射。
    - **核心类**：`HuffmanTree`
    - **关键方法**：
        - 构造函数：支持从频率映射、字符串或现有节点构建哈夫曼树。
        - `serialize()`: 将整棵树序列化为字符串。
        - `deserialize(const std::string &s)`: 从序列化字符串中重建哈夫曼树。
        - `getCharMap()`: 获取编码到字符的映射。
        - `getCodeMap()`: 获取字符到编码的映射。

4. **HuffmanZip 模块 (`HuffmanZip.h` & `HuffmanZip.cpp`)**
    - **职责**：提供压缩与解压的高层接口，整合其他模块进行实际操作。
    - **核心类**：`HuffmanZip`
    - **关键方法**：
        - `compress(const std::string& inputPath, const std::string& outputPath)`: 对指定输入文件进行压缩，输出到指定路径。
        - `decompress(const std::string& inputPath, const std::string& outputPath)`: 对指定压缩文件进行解压，输出到指定路径。
        - TODO: 实现对多文件的压缩、对文件夹的压缩、文件名的存储、压缩密码的设置、解压时产生是否替换同名文件的提示。

5. **main 模块 (`main.cpp`)**
    - **职责**：定义程序的运行方式，定义压缩程序的指令格式。
    - 开发过程中，用`main.cpp`方便地进行单元测试。
