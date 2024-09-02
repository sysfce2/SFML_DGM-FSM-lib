cmake_minimum_required ( VERSION 3.19 )

set ( TARGET tests )

bootstrap_clang_format()
glob_headers_and_sources ( HEADERS SOURCES )

add_executable ( ${TARGET}
	${HEADERS} ${SOURCES}
)

target_include_directories ( ${TARGET}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${TARGET}
	libfsm Catch2::Catch2WithMain
)

target_compile_options ( ${TARGET}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)

add_test (
	NAME ${TARGET}
	COMMAND ${TARGET}
)
