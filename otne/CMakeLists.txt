# otne 解释器可执行程序

add_executable(otne
	src/otne_main.cpp
	src/otne_file.cpp
	src/otne_lexer.cpp
	src/otne_parser.cpp
	src/otne_value.cpp
	src/otne_interpreter.cpp
)

target_include_directories(otne PRIVATE src)

# 源码与执行字符集统一使用 UTF-8
target_compile_options(otne PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/utf-8>
	$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-finput-charset=UTF-8>
	$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
)
