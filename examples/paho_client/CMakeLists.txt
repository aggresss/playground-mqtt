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

  set(TEST_TARGET_1 "paho_async_origin")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c)
  target_link_libraries(${TEST_TARGET_1} paho-mqtt3a-static pthread)
  
    set(TEST_TARGET_2 "paho_async_modify")
  add_executable(${TEST_TARGET_2} ${TEST_TARGET_2}.c)
  target_link_libraries(${TEST_TARGET_2} paho-mqtt3a-static pthread)

endif()

