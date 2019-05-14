# ------------------------------------------------------------------------------
# CMakeLists.txt for aggresss/playground-mqtt/third_party
# ------------------------------------------------------------------------------

include(ExternalProject)

option(WITH_PAHO "Build with eclipse paho" OFF)
if(WITH_PAHO)
  set(PAHO_PREFIX ${PROJECT_BINARY_DIR}/third_party/paho PARENT_SCOPE)
  ExternalProject_Add(external-paho
    URL https://github.com/eclipse/paho.mqtt.c/archive/v1.3.0.tar.gz
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/third_party/paho
      -DCMAKE_TOOLCHAIN_FILE:PATH=${TOOLCHAIN_FILE}
      -DCMAKE_BUILD_TYPE:STRING=Debug
      -DPAHO_WITH_SSL:BOOL=FALSE
      -DPAHO_BUILD_DOCUMENTATION:BOOL=FALSE
      -DPAHO_BUILD_SAMPLES:BOOL=FALSE
      -DPAHO_ENABLE_TESTING:BOOL=FALSE
      -DPAHO_BUILD_STATIC:BOOL=TRUE
    TEST_COMMAND ""
    )
endif(WITH_PAHO)

option(WITH_IOTC "Build with google iotc" OFF)
if(WITH_IOTC)
  set(IOTC_PREFIX ${PROJECT_BINARY_DIR}/third_party/iotc PARENT_SCOPE)
  ExternalProject_Add(external-iotc
    SOURCE_DIR ${IOTC_PREFIX}
    URL https://github.com/GoogleCloudPlatform/iot-device-sdk-embedded-c/archive/v1.0.1.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    )
endif(WITH_IOTC)

option(WITH_LIBEVENT "Build with libevent" OFF)
if(WITH_LIBEVENT)
  set(LIBEVENT_PREFIX ${PROJECT_BINARY_DIR}/third_party/libevent PARENT_SCOPE)
  ExternalProject_Add(external-libevent
    BUILD_IN_SOURCE TRUE
    URL https://github.com/libevent/libevent/releases/download/release-2.1.8-stable/libevent-2.1.8-stable.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./autogen.sh &&
      ./configure
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        --enable-static=${THIRDPARTY_WITH_STATIC}
        --enable-shared=${THIRDPARTY_WITH_SHARED}
        --host=${C_COMPILER_MACHINE}
        --prefix=${PROJECT_BINARY_DIR}/third_party/libevent
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    TEST_COMMAND ""
    )
endif(WITH_LIBEVENT)

option(WITH_OPENSSL "Build with openssl" OFF)
if(WITH_OPENSSL)
  set(OPENSSL_PREFIX ${PROJECT_BINARY_DIR}/third_party/openssl PARENT_SCOPE)
  ExternalProject_Add(external-openssl
    SOURCE_DIR ${OPENSSL_PREFIX}
    BUILD_IN_SOURCE TRUE
    URL https://github.com/openssl/openssl/archive/OpenSSL_1_1_1b.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND
      ./Configure
      -fPIC
      no-asm
      linux-generic32
      --prefix=${PROJECT_BINARY_DIR}/third_party/openssl
      --cross-compile-prefix=${C_COMPILER_PREFIX}
      no-shared
    BUILD_COMMAND make
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    )
endif(WITH_OPENSSL)

option(WITH_MBEDTLS "Build with mbedtls" OFF)
if(WITH_MBEDTLS)
  set(MBEDTLS_PREFIX ${PROJECT_BINARY_DIR}/third_party/mbedtls PARENT_SCOPE)
  ExternalProject_Add(external-mbedtls
    URL https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.16.1.tar.gz
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/third_party/mbedtls
      -DCMAKE_TOOLCHAIN_FILE:PATH=${TOOLCHAIN_FILE}
      -DCMAKE_BUILD_TYPE:STRING=Debug
      -DENABLE_TESTING:BOOL=OFF
      -DENABLE_PROGRAMS:BOOL=OFF
    TEST_COMMAND ""
    )
endif(WITH_MBEDTLS)

option(WITH_WOLFSSL "Build with wolfssl" OFF)
if(WITH_WOLFSSL)
  set(WOLFSSL_PREFIX ${PROJECT_BINARY_DIR}/third_party/wolfssl PARENT_SCOPE)
  ExternalProject_Add(external-wolfSSL
    BUILD_IN_SOURCE TRUE
    URL https://github.com/wolfSSL/wolfssl/archive/v4.0.0-stable.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./autogen.sh &&
      ./configure
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        --enable-static=${THIRDPARTY_WITH_STATIC}
        --enable-shared=${THIRDPARTY_WITH_SHARED}
        --disable-debug
        --host=${C_COMPILER_MACHINE}
        --prefix=${PROJECT_BINARY_DIR}/third_party/wolfssl
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    TEST_COMMAND ""
    )
endif(WITH_WOLFSSL)

option(WITH_DEPS "Build with deps" ON)
if(WITH_DEPS)
  set(DEPS_PREFIX ${PROJECT_BINARY_DIR}/third_party/deps PARENT_SCOPE)
  ExternalProject_Add(external-deps
    SOURCE_DIR ${DEPS_PREFIX}
    URL https://github.com/aggresss/deps/archive/v1.0.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    )
endif(WITH_DEPS)

