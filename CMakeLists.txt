CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(bundle C)
set (VERSION "0.1.31")

### Required packages
INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED glib-2.0 dlog capi-base-common)
FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

### Local include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

### Build
add_library(bundle SHARED 
		src/bundle.c
		src/keyval_type.c
		src/keyval.c
		src/keyval_array.c
		)
set_target_properties(bundle PROPERTIES SOVERSION "0")
set_target_properties(bundle PROPERTIES VERSION "${VERSION}")
message(STATUS "Version from debian/changelog: ${VERSION},  Major version: ${VERSION_MAJOR}")
target_link_libraries(bundle ${pkgs_LDFLAGS})


### Make pkgconfig file
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/bundle.pc.in ${CMAKE_BINARY_DIR}/bundle.pc @ONLY)


### Install
install(TARGETS bundle
    DESTINATION ${LIB_INSTALL_DIR}
	)
install(FILES 
	${CMAKE_SOURCE_DIR}/include/bundle.h 
	${CMAKE_SOURCE_DIR}/include/SLP_bundle_PG.h 
	DESTINATION include/
		)
install(FILES ${CMAKE_BINARY_DIR}/bundle.pc 
    DESTINATION ${LIB_INSTALL_DIR}/pkgconfig/
		)


### Test
add_subdirectory(test)

