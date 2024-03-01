cmake_minimum_required(VERSION 3.16)

# MSVC utf-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

# GNU utf-8
add_compile_options("$<$<C_COMPILER_ID:GNU>:-fexec-charset=GBK>")
add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-fexec-charset=GBK>")
add_compile_options("$<$<C_COMPILER_ID:GNU>:-finput-charset=utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-finput-charset=utf-8>")


file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/*"
	"${PROJECT_SOURCE_DIR}/src/*"
	"${PROJECT_SOURCE_DIR}/src/Otne.cpp"
)

add_executable (Otne ${SRCS})