function ( download_clang_format )
	file (
		DOWNLOAD
		"https://raw.githubusercontent.com/nerudaj/clang-format/main/.clang-format"
		"${PROJECT_BINARY_DIR}/.clang-format"
	)
endfunction ()

function ( bootstrap_clang_format )
	file (
		COPY "${PROJECT_BINARY_DIR}/.clang-format"
		DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
	)
endfunction()

function ( glob_headers_and_sources hdr_outvarname src_outvarname )
    file ( GLOB_RECURSE LOCAL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include**/*.hpp" )
	file ( GLOB_RECURSE LOCAL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src**/*.cpp" )
	source_group ( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LOCAL_HEADERS})
	source_group ( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LOCAL_SOURCES})
	set ( ${hdr_outvarname} "${LOCAL_HEADERS}" PARENT_SCOPE )
	set ( ${src_outvarname} "${LOCAL_SOURCES}" PARENT_SCOPE )
endfunction ()
