
# for convenience
set(header_path "${CMAKE_SOURCE_DIR}/include/mpolib2")

# headers to be installed
set(MPOJSON_PUBLIC_INCLUDE
		${header_path}/mpo_json.h
		)

# source files to be built
set(MPOJSON_SRCS
		mpo_json.cpp
        )

add_library(mpojson ${MPOJSON_PUBLIC_INCLUDE} ${MPOJSON_SRCS} )

if (MSVC)
	# see https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
	target_compile_options(mpojson PUBLIC "/Zc:__cplusplus")
endif()

# So that anything that links to our lib gets the headers for all dependencies
# (configured include files go to the build folder, not the source folder (by design))
target_include_directories(mpojson PUBLIC

		# this will be populated when installing but blank when building
		# for client in install mode
		$<INSTALL_INTERFACE:include>

		# this will be populated when building but blank when installing
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

set_target_properties(mpojson PROPERTIES OUTPUT_NAME "mpojson")

install(FILES ${MPOLIB_PUBLIC_INCLUDE} DESTINATION "${include_dest}")

# goes into lib_dest so that Debug/Release is taken into account
install(TARGETS mpojson EXPORT mpojson DESTINATION "${lib_dest}")