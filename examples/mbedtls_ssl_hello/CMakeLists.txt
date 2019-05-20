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
  if(WITH_MBEDTLS)
    include_directories(${MBEDTLS_PREFIX}/include)
    link_directories(${MBEDTLS_PREFIX}/lib)
  endif()

  set(TEST_TARGET_1 "tls_server")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c)
  target_link_libraries(${TEST_TARGET_1} mbedtls mbedx509 mbedcrypto)
  
  set(TEST_TARGET_2 "tls_client")
  add_executable(${TEST_TARGET_2} ${TEST_TARGET_2}.c)
  target_link_libraries(${TEST_TARGET_2} mbedtls mbedx509 mbedcrypto)
  
  set(TEST_TARGET_3 "tls_client2")
  add_executable(${TEST_TARGET_3} ${TEST_TARGET_3}.c query_config.c)
  target_link_libraries(${TEST_TARGET_3} mbedtls mbedx509 mbedcrypto)

endif()

