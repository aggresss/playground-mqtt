# ------------------------------------------------------------------------------
# CMakeLists.txt for aggresss/playground-mqtt subdirectory
# ------------------------------------------------------------------------------

string(REGEX MATCH "[^/]+$" SUBMODULE ${CMAKE_CURRENT_LIST_DIR})
option(${SUBMODULE} "Build ${SUBMODULE}" OFF)

if(${SUBMODULE} OR BUILD_ALL)
  # Build
  if(WITH_OPENSSL)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    link_directories(${OPENSSL_LIBRARIES})
  endif()

  set(TEST_TARGET_1 "tls_server")
  add_executable(${TEST_TARGET_1} server.c)
  target_compile_options(${TEST_TARGET_1} PRIVATE -std=c99)
  target_link_libraries(${TEST_TARGET_1} ssl crypto)
  target_link_libraries(${TEST_TARGET_1} pthread)

endif()

