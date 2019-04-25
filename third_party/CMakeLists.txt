# ------------------------------------------------------------------------------
# link-c-sdk/third_party CMakeLists.txt
# ------------------------------------------------------------------------------

include(ExternalProject)

if(WITH_PAHO)
  ExternalProject_Add(external-paho
    BUILD_ALWAYS 1
    PREFIX ${PAHO_PREFIX}
    URL https://github.com/eclipse/paho.mqtt.c/archive/v1.3.0.tar.gz
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=Debug
      -DPAHO_WITH_SSL=FALSE
      -DPAHO_BUILD_DOCUMENTATION=FALSE
      -DPAHO_BUILD_SAMPLES=FALSE
      -DPAHO_BUILD_STATIC=TRUE
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    TEST_COMMAND ""
    )
endif()

