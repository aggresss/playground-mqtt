# ------------------------------------------------------------------------------
# CMakeLists.txt for aggresss/playground-mqtt/example subdirectory
# ------------------------------------------------------------------------------


string(REGEX MATCH "[^/]+$" SUBMODULE ${CMAKE_CURRENT_LIST_DIR})
option(${SUBMODULE} "Build ${SUBMODULE}" OFF)

include_directories(${TOP_DIRS})
include_directories(${CONFIG_DIRS})

add_compile_options(-std=gnu99)

if(${SUBMODULE} OR BUILD_ALL)
  # Build
  if(WITH_LIBUV)
    include_directories(${LIBUV_PREFIX}/include)
    link_directories(${LIBUV_PREFIX}/lib)
  endif()
  if(WITH_MBEDTLS)
    include_directories(${MBEDTLS_PREFIX}/include)
    link_directories(${MBEDTLS_PREFIX}/lib)
  endif()

  set(TEST_TARGET_1 "http_test")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c http-client/http_client.c)
  target_link_libraries(${TEST_TARGET_1} mbedtls mbedx509 mbedcrypto)
  target_link_libraries(${TEST_TARGET_1} uv_a pthread)

endif()

