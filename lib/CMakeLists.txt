cmake_minimum_required ( VERSION 3.19 )

project ( libfsm )

include_directories ( "${JSON_FOLDER}/include" )

file (
	GLOB LIB_HEADERS
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file (
	GLOB LIB_SOURCES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

source_group (
	TREE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		${LIB_HEADERS}
		${LIB_SOURCES}
)

add_library ( ${PROJECT_NAME} STATIC
	${LIB_HEADERS} ${LIB_SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features( ${PROJECT_NAME}
	INTERFACE ${MY_CXX_COMPILE_FEATURES}
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE /W4 /WX /MP
)

add_subdirectory ( tests )
