<div align="center">
  <img src="resources/icons/otne.svg" alt="Otne" width="160" height="160">

  <h1>Otne · 中引编程语言</h1>

  <p>一门免费开源的解释型编程语言</p>

  <p>
    <a href="README.md">简体中文</a> •
    <a href="README.en.md">English</a> •
    <a href="CHANGELOG.md">更新日志</a> •
    <a href="docs/otne-language.md">语言说明</a>
  </p>
</div>

## 简介

Otne（中引）是由 Uxnt 与 uxnt-naxe 于 2022 年 3 月 6 日发起设计的编程语言，语法参照 Java 与 Go。

当前版本（v0.2.0）是一个用 C++17 实现的**树遍历解释器**，包含词法分析、语法分析、运行时环境与内建函数，可真正执行 `.otne` 程序。

## 快速开始

### 环境要求

| 依赖 | 版本要求 |
| --- | --- |
| CMake | 3.16 及以上 |
| C++ 编译器 | 支持 C++17（GCC / Clang / MSVC / MinGW-w64） |

### 构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

可执行文件生成在 `build/bin/otne`。

也可以使用项目自带的 Makefile（内部封装 CMake）：

```bash
make                                              # 构建（Release）
make debug                                        # Debug 构建
make rebuild                                      # 删build目录后重建
make run                                          # 运行默认示例
make run FILE=resources/code/Fibonacci.otne       # 运行指定文件
make tokens FILE=resources/code/HelloWorld.otne   # 只输出词法单元
make test                                         # 运行冒烟测试
make ctest                                        # 通过 CTest 运行测试
make clean                                        # 清理编译产物
make help                                         # 查看全部目标
```

### 运行

```bash
# 无参数时运行默认示例 resources/code/HelloWorld.otne
./build/bin/otne

# 运行指定源文件
./build/bin/otne resources/code/Fibonacci.otne

# 只查看词法分析结果
./build/bin/otne --tokens resources/code/HelloWorld.otne
```

> 默认示例使用相对路径查找，请在项目根目录下运行。

### 命令行选项

| 选项 | 说明 |
| --- | --- |
| `-h`, `--help` | 显示帮助 |
| `-v`, `--version` | 显示版本 |
| `-t`, `--tokens` | 只输出词法单元，不执行 |

## 示例

```rs
// HelloWorld.otne
use stdio;
func HelloWorld() : string { ret "Hello World!"; }
func main() {
    println( HelloWorld() );
}
```

```
$ otne resources/code/HelloWorld.otne
Hello, World!
```

更多示例：

| 文件 | 演示内容 |
| --- | --- |
| [`HelloWorld.otne`](resources/code/HelloWorld.otne) | 最小可运行程序 |
| [`HelloUniverse.otne`](resources/code/HelloUniverse.otne) | 函数、参数类型、字符串拼接 |
| [`Fibonacci.otne`](resources/code/Fibonacci.otne) | 递归、`for` / `while` |
| [`FizzBuzz.otne`](resources/code/FizzBuzz.otne) | `if` / `elif` / `else`、取模 |
| [`Types.otne`](resources/code/Types.otne) | 值类型、类型标注与内建函数 |

## 语言特性

- **简洁语法**：类 Java / Go 的写法，关键字统一使用英文
- **可选类型标注**：`let n: int = 1`、`func half(v: float): float`
- **动态类型**：`int` / `float` / `string` / `bool` / `null`，`+` 支持字符串拼接
- **完整控制流**：`if` / `elif` / `else`、`while`、C 风格 `for`、`break` / `continue` / `return`
- **函数是一等值**：可递归调用，也可赋值给变量
- **精确报错**：词法、语法、运行时错误均带 `文件:行:列`

完整语法见 [语言说明](docs/otne-language.md)，机器可读的语法定义见 [`docs/otne.bnf`](docs/otne.bnf)。

## 项目结构

```text
otne/
├── CMakeLists.txt          顶层构建脚本
├── Makefile                便捷构建脚本（封装 CMake）
├── otne/
│   ├── CMakeLists.txt
│   └── src/
│       ├── otne_main.cpp         CLI 入口
│       ├── otne_lexer.*          词法分析
│       ├── otne_parser.*         语法分析
│       ├── otne_ast.hpp          语法树定义
│       ├── otne_value.*          运行时值与作用域
│       ├── otne_interpreter.*    解释执行
│       ├── otne_token.hpp        Token 定义
│       ├── otne_diagnostic.hpp   诊断信息
│       ├── otne_file.*           源文件读取（UTF-8）
│       └── otne_utf8.hpp         UTF-8 转换工具
├── docs/otne-language.md   语言说明与语法
├── docs/otne.bnf           BNF 语法定义（含词法规则）
├── tests/smoke.sh          冒烟测试
├── resources/
│   ├── code/               示例源码
│   └── icons/              图标素材
├── CHANGELOG.md
├── README.en.md
└── LICENSE
```

## 测试

```bash
cmake --build build
./tests/smoke.sh                 # 默认使用 build/bin/otne
./tests/smoke.sh build/bin/otne  # 指定可执行文件
```

也可以直接执行 `make test`（或 `make ctest`）。

冒烟测试覆盖全部示例程序，以及词法、语法、运行时错误与退出码。

## 参与贡献

欢迎提交 Issue 与 Pull Request。提交前请确保：

1. 项目可以正常构建：`cmake --build build`
2. 冒烟测试全部通过：`./tests/smoke.sh`
3. 新增示例放在 `resources/code/`，并在本文档的示例表格中登记
4. 语法变更同步更新 `docs/otne-language.md` 与 `docs/otne.bnf`

## 许可证

本项目基于 [GNU General Public License v3.0](LICENSE) 发布。
