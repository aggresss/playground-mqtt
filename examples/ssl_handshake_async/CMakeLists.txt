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

  set(TEST_TARGET_1 "ssl_server_async")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c)
  target_link_libraries(${TEST_TARGET_1} ssl crypto pthread dl)

  set(TEST_TARGET_2 "ssl_client_async")
  add_executable(${TEST_TARGET_2} ${TEST_TARGET_2}.c)
  target_link_libraries(${TEST_TARGET_2} ssl crypto pthread dl)

endif()

