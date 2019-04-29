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
    include_directories(${OPENSSL_INCLUDE_DIRS})
    link_directories(${OPENSSL_LIBRARIES})
  endif()

  set(TEST_TARGET_1 "tls_server_one-way")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c)
  target_link_libraries(${TEST_TARGET_1} ssl crypto pthread dl)
  
  set(TEST_TARGET_2 "tls_client_one-way")
  add_executable(${TEST_TARGET_2} ${TEST_TARGET_2}.c)
  target_link_libraries(${TEST_TARGET_2} ssl crypto pthread dl)
  
  set(TEST_TARGET_3 "tls_server_two-way")
  add_executable(${TEST_TARGET_3} ${TEST_TARGET_3}.c)
  target_link_libraries(${TEST_TARGET_3} ssl crypto pthread dl)

  set(TEST_TARGET_4 "tls_client_two-way")
  add_executable(${TEST_TARGET_4} ${TEST_TARGET_4}.c)
  target_link_libraries(${TEST_TARGET_4} ssl crypto pthread dl)
  
  set(TEST_TARGET_5 "tcp_server")
  add_executable(${TEST_TARGET_5} ${TEST_TARGET_5}.c)
  
  set(TEST_TARGET_6 "tcp_client")
  add_executable(${TEST_TARGET_6} ${TEST_TARGET_6}.c)
  
  set(TEST_TARGET_7 "tls_client")
  add_executable(${TEST_TARGET_7} ${TEST_TARGET_7}.c)
  target_link_libraries(${TEST_TARGET_7} ssl crypto pthread dl)
  
  set(TEST_TARGET_8 "tls_server")
  add_executable(${TEST_TARGET_8} ${TEST_TARGET_8}.c)
  target_link_libraries(${TEST_TARGET_8} ssl crypto pthread dl)

endif()

