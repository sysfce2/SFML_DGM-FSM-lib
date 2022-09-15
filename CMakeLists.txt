cmake_minimum_required ( VERSION 3.19 )

include ( "${CMAKE_SOURCE_DIR}/cmake/cpp.cmake" )

project ( dgm-lib )
include ( "${CMAKE_SOURCE_DIR}/cmake/version.cmake" )
enable_testing()

include ( "${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake" )

add_subdirectory ( lib )

install (
	FILES       "${CMAKE_SOURCE_DIR}/changelog.txt"
	DESTINATION "."
)

# Packaging
set ( CPACK_GENERATOR "ZIP" )
set ( CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}" )
set ( CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}" )
set ( CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}" )
set ( CPACK_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${PROJECT_VERSION}-windows-vc16-${CMAKE_GENERATOR_PLATFORM}" )

include ( CPack )
