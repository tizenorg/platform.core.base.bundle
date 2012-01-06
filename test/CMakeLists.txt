add_executable(test_bundle EXCLUDE_FROM_ALL
		test_bundle.c
		)
target_link_libraries(test_bundle bundle)

add_custom_target(test
	COMMAND LD_LIBRARY_PATH=${CMAKE_BINARY_DIR} ./test_bundle
	DEPENDS test_bundle
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	COMMENT "Run 'make test'"
	)	
