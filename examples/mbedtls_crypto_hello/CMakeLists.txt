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

  set(TEST_TARGET_1 "test_aes256")
  aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/b64 B64_SRC)
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c ${B64_SRC})
  target_link_libraries(${TEST_TARGET_1} mbedcrypto)

endif()

