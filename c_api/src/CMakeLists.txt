cmake_minimum_required(VERSION 3.2)

# The defines we need for the library
set(CHECKUP_SDK_INCLUDE ${CHECKUP_SDK_ROOT}/include)
set(CHECKUP_CAPI_INCLUDE ${CHECKUP_CAPI_ROOT}/include)
set(CHECKUP_CAPI_SRC ${CHECKUP_CAPI_ROOT}/src)

bento_headers(header_files "${CHECKUP_CAPI_INCLUDE}" "c_api")
bento_sources(source_files "${CHECKUP_CAPI_SRC}" "c_api")

# Generate the static library
bento_dynamic_lib("checkup_dylib" "c_api" "${header_files};${source_files};" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE};${CHECKUP_CAPI_INCLUDE}")
target_link_libraries("checkup_dylib" "checkup_sdk" "bento_sdk")