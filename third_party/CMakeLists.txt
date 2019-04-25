# ------------------------------------------------------------------------------
# CMakeLists.txt for aggresss/playground-mqtt/third_party
# ------------------------------------------------------------------------------

include(ExternalProject)

if(WITH_PAHO)
  ExternalProject_Add(external-paho
    BUILD_ALWAYS 1
    URL https://github.com/eclipse/paho.mqtt.c/archive/v1.3.0.tar.gz
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${PAHO_PREFIX}
      -DCMAKE_TOOLCHAIN_FILE:PATH=${TOOLCHAIN_FILE}
      -DCMAKE_BUILD_TYPE:STRING=Debug
      -DPAHO_WITH_SSL:BOOL=FALSE
      -DPAHO_BUILD_DOCUMENTATION:BOOL=FALSE
      -DPAHO_BUILD_SAMPLES:BOOL=FALSE
      -DPAHO_BUILD_STATIC:BOOL=TRUE
    TEST_COMMAND ""
    )
endif()

