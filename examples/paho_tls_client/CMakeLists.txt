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
  if(WITH_PAHO)
    include_directories(${PAHO_PREFIX}/include)
    link_directories(${PAHO_PREFIX}/lib)
  endif()
  if(WITH_OPENSSL)
    include_directories(${OPENSSL_PREFIX}/include)
    link_directories(${OPENSSL_PREFIX}/lib)
  endif()

  set(TEST_TARGET_1 "paho_tls_async")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c)
  target_link_libraries(${TEST_TARGET_1} paho-mqtt3as-static ssl crypto pthread dl)

endif()

