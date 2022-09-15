cmake_minimum_required ( VERSION 3.19 )

project ( unit-tests )

include_directories ( ${CATCH_FOLDER} )

file (
	GLOB TEST_HEADERS
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

file (
	GLOB TEST_SOURCES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	"../${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig"
)

source_group (
	TREE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		${TEST_HEADERS}
		${TEST_SOURCES}
)

add_executable ( ${PROJECT_NAME}
	${TEST_HEADERS} ${TEST_SOURCES}
)

target_link_libraries ( ${PROJECT_NAME}
	libfsm
)

add_test (
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
