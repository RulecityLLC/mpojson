# this config file should exist from when we built the source and may be needed to avoided weird build issues
#add_compile_definitions(HAVE_MPOLIB_CONFIG_H)

set(TEST_MPOJSON_SRCS
        main.cpp
        test_json.cpp
        )

add_executable(test_mpojson ${TEST_MPOJSON_SRCS})

# dependencies
find_package(mpo 2.0 REQUIRED)
find_package(jansson REQUIRED)

message("JANSSNO: ${JANSSON_LIBRARIES}")

include_directories("../thirdparty/googletest-release-1.8.1/googlemock/include")
include_directories("../thirdparty/googletest-release-1.8.1/googletest/include")

# make visual studio run from the correct directory (only works for VS2010 and above)
set_property(TARGET test_mpojson PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# this will automatically give the indicated targets access to the headers/libs of the indicated dependencies
if(WIN32)
    if (MSVC)
        # see https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
        target_compile_options(test_mpojson PUBLIC "/Zc:__cplusplus")
        target_link_libraries(test_mpojson PUBLIC mpojson mpo jansson::jansson gmock)
    else()
        # we don't want to have to fiddle around with the GCC windows DLLs, so we have to manually append "-static" to the end
        target_link_libraries(test_mpojson PUBLIC mpojson mpo jansson::jansson gmock -static)
    endif()
else()
    # we assume in most unix-ish environments that the compiler's runtime is in the PATH (no need to statically link test programs)
    target_link_libraries(test_mpojson PUBLIC mpojson mpo jansson::jansson gmock)
endif()
