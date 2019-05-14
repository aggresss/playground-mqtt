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
  if(WITH_OPENSSL)
    include_directories(${OPENSSL_PREFIX}/include)
    link_directories(${OPENSSL_PREFIX}/lib)
  endif()

  set(TEST_TARGET_1 "bio_tcp_server")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c)
  target_link_libraries(${TEST_TARGET_1} ssl crypto pthread dl)

  set(TEST_TARGET_2 "bio_tcp_client")
  add_executable(${TEST_TARGET_2} ${TEST_TARGET_2}.c)
  target_link_libraries(${TEST_TARGET_2} ssl crypto pthread dl)

  set(TEST_TARGET_3 "bio_tls_server")
  add_executable(${TEST_TARGET_3} ${TEST_TARGET_3}.c)
  target_link_libraries(${TEST_TARGET_3} ssl crypto pthread dl)

  set(TEST_TARGET_4 "bio_tls_client")
  add_executable(${TEST_TARGET_4} ${TEST_TARGET_4}.c)
  target_link_libraries(${TEST_TARGET_4} ssl crypto pthread dl)

endif()

