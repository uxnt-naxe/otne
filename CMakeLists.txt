cmake_minimum_required(VERSION 3.16)

project(otne
	VERSION 0.2.0
	DESCRIPTION "Otne (中引) programming language"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 未指定构建类型时默认使用 Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_subdirectory(otne)

# 可执行文件统一输出到 build/bin
set_target_properties(otne PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 测试：include(CTest) 定义 BUILD_TESTING 选项并启用测试
include(CTest)
if(BUILD_TESTING)
	find_program(BASH_PROGRAM bash)
	if(BASH_PROGRAM)
		add_test(NAME smoke
			COMMAND "${BASH_PROGRAM}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/smoke.sh" $<TARGET_FILE:otne>)
	endif()
endif()