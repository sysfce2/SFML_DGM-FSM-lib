cmake_minimum_required ( VERSION 3.26 )

project ( dgm-fsm-lib VERSION "1.3.0" )

# Project and compilation rules
include ( "${PROJECT_SOURCE_DIR}/cmake/cpm.cmake" )
include ( "${PROJECT_SOURCE_DIR}/cmake/cpp.cmake" )
include ( "${PROJECT_SOURCE_DIR}/cmake/utils.cmake" )
include ( "${PROJECT_SOURCE_DIR}/cmake/dependencies.cmake" )

download_clang_format()

add_subdirectory ( "${PROJECT_SOURCE_DIR}/lib" )
enable_testing()
add_subdirectory ( "${PROJECT_SOURCE_DIR}/tests" )

# Packaging rules
install (
	FILES       "${PROJECT_SOURCE_DIR}/changelog.txt"
	DESTINATION "."
)

install (
	FILES       "${PROJECT_SOURCE_DIR}/LICENSE"
	DESTINATION "."
)

install (
	FILES       "${PROJECT_SOURCE_DIR}/Readme.md"
	DESTINATION "."
)

# Packaging
if ( ${IS_PROJECT_TOP_LEVEL} )
	set ( COMPILER_ID "${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
	message ( "Computed compiler ID: ${COMPILER_ID}" )

	set ( CPACK_GENERATOR "ZIP" )
	set ( CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${COMPILER_ID}-${CMAKE_GENERATOR_PLATFORM}" )

	include ( CPack )
endif()
