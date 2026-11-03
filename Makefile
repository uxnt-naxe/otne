# Otne 便捷构建脚本：封装 CMake 常用命令
# 用法：make help

BUILD_DIR  ?= build
BUILD_TYPE ?= Release
FILE       ?= resources/code/HelloWorld.otne
JOBS       ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
CMAKE      ?= cmake

BIN := $(BUILD_DIR)/bin/otne

.PHONY: all build release debug rebuild run tokens test ctest clean distclean help

all: build

## build: 配置并构建（BUILD_TYPE 默认 Release）
build:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	$(CMAKE) --build $(BUILD_DIR) --parallel $(JOBS)

## release: Release 构建
release:
	$(MAKE) build BUILD_TYPE=Release

## debug: Debug 构建
debug:
	$(MAKE) build BUILD_TYPE=Debug

## rebuild: 删除构建目录后重新构建
rebuild:
	$(MAKE) distclean
	$(MAKE) build

## run: 运行源文件（FILE=路径，默认 resources/code/HelloWorld.otne）
run: build
	./$(BIN) $(FILE)

## tokens: 只输出词法单元（FILE=路径）
tokens: build
	./$(BIN) --tokens $(FILE)

## test: 运行冒烟测试
test: build
	./tests/smoke.sh $(BIN)

## ctest: 通过 CTest 运行测试
ctest: build
	cd $(BUILD_DIR) && ctest --output-on-failure

## clean: 清理编译产物（保留构建目录）
clean:
	@$(CMAKE) --build $(BUILD_DIR) --target clean 2>/dev/null || true

## distclean: 删除整个构建目录
distclean:
	rm -rf $(BUILD_DIR)

## help: 显示可用目标
help:
	@echo "Otne 构建目标："
	@echo
	@grep -E '^## ' $(MAKEFILE_LIST) | sed -e 's/^## /  make /'
	@echo
	@echo "可用变量："
	@echo "  BUILD_DIR=$(BUILD_DIR)  BUILD_TYPE=$(BUILD_TYPE)"
	@echo "  FILE=$(FILE)"
	@echo
	@echo "示例：make run FILE=resources/code/Fibonacci.otne"
