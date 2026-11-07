# Otne 语言说明（v0.0.2）

> 本文档描述当前解释器实现的语法与语义。示例代码位于 [`resources/code/`](../resources/code/)。

## 目录

- [设计目标](#设计目标)
- [程序结构](#程序结构)
- [词法](#词法)
- [类型与值](#类型与值)
- [表达式与运算符](#表达式与运算符)
- [语句](#语句)
- [函数](#函数)
- [内建函数](#内建函数)
- [关键字](#关键字)
- [完整语法（EBNF）](#完整语法ebnf)

## 设计目标

- **易读**：语法接近 Java / Go，关键字统一使用英文
- **安全**：静态位置信息贯穿错误报告，运行时错误精确到行:列
- **可翻译**：语法结构简单，便于后续实现到 C / C++ / Rust / Python 的转换器
- **国际化**：源码统一为 UTF-8，内部按 Unicode 处理

## 程序结构

一个 `.otne` 源文件由 **import 声明** 与 **函数定义** 组成，解释器从 `main` 开始执行。

```otne
import stdio;

func main() {
    println("Hello, World!");
}
```

- 顶层不允许出现语句，语句只能写在函数体内
- 入口函数必须名为 `main`
- 函数定义顺序无关，可相互调用（含递归）

## 词法

| 元素 | 说明 |
| --- | --- |
| 空白 | 空格、制表符、回车、换行仅作分隔 |
| 行注释 | `//` 到行尾 |
| 块注释 | `/* ... */`，可跨行 |
| 标识符 | ASCII 字母或下划线开头，可含数字 |
| 数字 | `123`、`1.5`；小数点后必须跟数字 |
| 字符串 | `"..."`，支持 `\n` `\t` `\r` `\\` `\"` `\'` |
| 关键字 | 见[关键字](#关键字) |

## 类型与值

运行时共有 5 种值：

| 类型 | 示例 | 说明 |
| --- | --- | --- |
| `null` | `null` | 空值，函数无返回值时的结果 |
| `bool` | `true` / `false` | 布尔值 |
| `int` | `42` | 64 位有符号整数 |
| `float` | `3.14` | 双精度浮点数 |
| `string` | `"hello"` | Unicode 字符串 |

`int` 与 `float` 混合运算时结果为 `float`；`int / int` 为整除。

### 类型标注（可选）

变量、参数、返回值都可以附加类型标注，运行时做检查：

```otne
let count: int = 0;      // 标注写法
int total = 0;           // 类型前置写法
func half(value: float): float { return value / 2; }
```

支持的标注：`int` `float` `string` `bool` `any`（`float` 标注接受 `int`）。

### 变量声明

```otne
let a = 1;        // 可变
var b = "text";   // 与 let 等价
const c = 3.14;   // 常量，不可重新赋值
```

## 表达式与运算符

优先级由高到低：

| 优先级 | 运算符 | 说明 |
| --- | --- | --- |
| 1 | `()` `f(args)` | 分组、函数调用 |
| 2 | `!` `-` | 逻辑非、取负 |
| 3 | `*` `/` `%` | 乘、除、取模 |
| 4 | `+` `-` | 加、减 |
| 5 | `<` `<=` `>` `>=` | 比较 |
| 6 | `==` `!=` | 相等 |
| 7 | `&&` | 逻辑与（短路） |
| 8 | `\|\|` | 逻辑或（短路） |
| 9 | `=` `+=` `-=` `*=` `/=` `%=` | 赋值（右结合） |

- `+` 的任意一侧为字符串时执行拼接：`"n = " + 1` → `"n = 1"`
- `&&` / `||` 返回 `bool`，且短路求值
- 真值判断：`false`、`null`、`0`、`0.0`、`""` 为假，其余为真

## 语句

### 条件

```otne
if (n % 15 == 0) {
    println("FizzBuzz");
} elif (n % 3 == 0) {
    println("Fizz");
} else {
    println(n);
}
```

条件表达式无需括号，`if` 后可省略 `()`；示例中保留括号只是为了可读性。

### 循环

```otne
while (i < 10) {
    i += 1;
}

for (let i = 0; i < 10; i += 1) {
    print(i);
}
```

`break` / `continue` 用于跳出或继续循环。

### 返回

```otne
func abs(n: int): int {
    if (n < 0) {
        return -n;
    }
    return n;
}
```

## 函数

```otne
func 函数名(参数名: 类型, ...): 返回类型 {
    // 函数体
}
```

- 参数与返回类型标注均可省略
- 没有 `return` 的函数返回 `null`
- 函数可递归，也可作为值传递：`let f = add;`

## 内建函数

| 函数 | 说明 |
| --- | --- |
| `print(...)` / `printf(...)` | 输出，不换行 |
| `println(...)` | 输出并换行 |
| `str(v)` | 转为字符串 |
| `int(v)` | 转为整数 |
| `float(v)` | 转为浮点数 |
| `len(s)` | 字符串长度 |
| `type(v)` | 返回类型名 |

## 关键字

| 分类 | 关键字 |
| --- | --- |
| 模块 | `import` |
| 声明 | `func`、`let`、`var`、`const` |
| 类型 | `int`、`float`、`string`、`bool`、`any` |
| 字面量 | `true`、`false`、`null` |
| 流程控制 | `if`、`elif`、`else`、`while`、`for`、`break`、`continue`、`return` |

> 关键字与标识符仅支持 ASCII 字母、数字与下划线；中文只能出现在字符串与注释中。

```otne
func main() {
    let greeting = "你好，中引";  // 字符串与注释可以是中文
    println(greeting);
}
```

## 错误报告

所有错误都以 `文件:行:列` 定位：

```text
error: resources/code/demo.otne:2:11: 未定义的变量 'y'
```

| 阶段 | 示例 | 退出码 |
| --- | --- | --- |
| 词法 | 未知字符 `~`、字符串未闭合 | 1 |
| 语法 | 缺少 `;`、缺少 `)`、顶层语句 | 1 |
| 运行时 | 未定义变量、类型不匹配、除零 | 1 |
| 参数 | 未知命令行选项 | 2 |

使用 `otne --tokens 文件` 可只查看词法分析结果。

## 完整语法（EBNF）

机器可读的语法定义（含词法规则、关键字表与优先级说明）见 [`otne.bnf`](otne.bnf)。

```ebnf
program      = { import | function } EOF
import       = "import" IDENT ";" ;
function     = "func" IDENT "(" [ params ] ")" [ ":" type ] block ;
params       = param { "," param } ;
param        = IDENT [ ":" type ] ;
type         = "int" | "float" | "string" | "bool" | "any" ;

block        = "{" { statement } "}" ;
statement    = var_decl | if_stmt | while_stmt | for_stmt | break_stmt
             | continue_stmt | return_stmt | block | expr_stmt ;

var_decl     = ( "let" | "var" | "const" ) IDENT [ ":" type ] "=" expr ";"
             | type IDENT [ ":" type ] "=" expr ";" ;
if_stmt      = "if" expr block { "elif" expr block } [ "else" block ] ;
while_stmt   = "while" expr block ;
for_stmt     = "for" "(" [ init ] ";" [ expr ] ";" [ expr ] ")" block ;
init         = var_decl | expr ";" ;
break_stmt   = "break" ";" ;
continue_stmt= "continue" ";" ;
return_stmt  = "return" [ expr ] ";" ;
expr_stmt    = expr ";" ;

expr         = assignment ;
assignment   = or_expr [ ( "=" | "+=" | "-=" | "*=" | "/=" | "%=" ) assignment ] ;
or_expr      = and_expr { "||" and_expr } ;
and_expr     = equality { "&&" equality } ;
equality     = comparison { ( "==" | "!=" ) comparison } ;
comparison   = term { ( "<" | "<=" | ">" | ">=" ) term } ;
term         = factor { ( "+" | "-" ) factor } ;
factor       = unary { ( "*" | "/" | "%" ) unary } ;
unary        = ( "!" | "-" ) unary | call ;
call         = primary { "(" [ args ] ")" } ;
args         = expr { "," expr } ;
primary      = NUMBER | STRING | "true" | "false" | "null"
             | IDENT | "(" expr ")" ;
```
