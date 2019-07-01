# ------------------------------------------------------------------------------
# CMakeLists.txt for aggresss/playground-mqtt/example subdirectory
# ------------------------------------------------------------------------------


string(REGEX MATCH "[^/]+$" SUBMODULE ${CMAKE_CURRENT_LIST_DIR})
option(${SUBMODULE} "Build ${SUBMODULE}" OFF)

include_directories(${TOP_DIRS})
include_directories(${CONFIG_DIRS})

add_compile_options(-std=gnu99)

if(${SUBMODULE} OR BUILD_ALL)

  set(TEST_TARGET_1 "paho_heap_test")
  add_executable(${TEST_TARGET_1} ${TEST_TARGET_1}.c Heap.c Tree.c StackTrace.c Log.c Thread.c LinkedList.c OsWrapper.c Messages.c)
  target_link_libraries(${TEST_TARGET_1} pthread)

endif()

