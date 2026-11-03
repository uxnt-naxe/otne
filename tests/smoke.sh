#!/usr/bin/env bash
# Otne 冒烟测试：运行示例与错误用例，校验输出与退出码
# 用法：tests/smoke.sh [可执行文件路径]

set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="${1:-$ROOT/build/bin/otne}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

if [[ ! -x "$BIN" ]]; then
    echo "找不到可执行文件：$BIN" >&2
    exit 2
fi

failures=0

pass() { echo "PASS  $1"; }

fail() {
    echo "FAIL  $1"
    shift
    for line in "$@"; do
        echo "      $line"
    done
    failures=$((failures + 1))
}

# 期望输出完全一致
expect_exact() {
    local name="$1" file="$2" expected="$3"
    local actual
    actual="$("$BIN" "$ROOT/$file" 2>&1)"
    if [[ "$actual" == "$expected" ]]; then
        pass "$name"
    else
        fail "$name" "期望: $expected" "实际: $actual"
    fi
}

# 期望输出包含某段文本
expect_contains() {
    local name="$1" file="$2" pattern="$3"
    local actual
    actual="$("$BIN" "$ROOT/$file" 2>&1)"
    if [[ "$actual" == *"$pattern"* ]]; then
        pass "$name"
    else
        fail "$name" "期望包含: $pattern" "实际: $actual"
    fi
}

# 期望以错误退出，并匹配错误信息
expect_error() {
    local name="$1" source="$2" pattern="$3"
    local file="$TMP/${name}.otne"
    printf '%s\n' "$source" > "$file"

    local output code
    output="$("$BIN" "$file" 2>&1)"
    code=$?
    if [[ $code -ne 0 && "$output" == *"$pattern"* ]]; then
        pass "$name"
    else
        fail "$name" "退出码: $code" "输出: $output"
    fi
}

echo "== 示例程序 =="
expect_contains "HelloWorld" "resources/code/HelloWorld.otne" "Hello, World!"
expect_exact "HelloUniverse" "resources/code/HelloUniverse.otne" \
"Hello, Universe!
Hello, 中引!
1 + 2 = 3"
expect_contains "Fibonacci" "resources/code/Fibonacci.otne" "0 1 1 2 3 5 8 13 21 34"
expect_contains "FizzBuzz" "resources/code/FizzBuzz.otne" "FizzBuzz"
expect_contains "Types" "resources/code/Types.otne" "int: 42"

echo "== 错误处理 =="
expect_error "lex-error" 'func main() { let x = 1 ~ 2; }' "未知字符"
expect_error "syntax-error" 'func main() { let x = 1 }' "缺少 ';'"
expect_error "undefined-variable" 'func main() { println(y); }' "未定义的变量"
expect_error "divide-by-zero" 'func main() { println(1 / 0); }' "除数不能为 0"
expect_error "type-mismatch" 'func main() { let s: string = 1; }' "需要 string"
expect_error "const-assign" 'func main() { const n = 1; n = 2; }' "不能被修改"
expect_error "no-main" 'func other() { }' "未找到入口函数"

echo
if [[ $failures -eq 0 ]]; then
    echo "全部通过"
else
    echo "失败 $failures 项"
fi
exit $((failures > 0))
