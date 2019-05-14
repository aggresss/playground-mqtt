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
  include_directories(${PAHO_PREFIX}/include)
  link_directories(${PAHO_PREFIX}/lib)
  include_directories(${DEPS_PREFIX})
  aux_source_directory(${DEPENDS_DIRS}/flag SRCS_DEPS)
  aux_source_directory(${DEPENDS_DIRS}/cJSON SRCS_DEPS)
  aux_source_directory(${DEPENDS_DIRS}/hmac_sha1 SRCS_DEPS)
  aux_source_directory(${DEPENDS_DIRS}/b64 SRCS_DEPS)
  set(TEST_TARGET_1 "qnlinking_mqtt")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c ${SRCS_DEPS})
  target_link_libraries(${TEST_TARGET_1} paho-mqtt3a-static pthread)

endif()

