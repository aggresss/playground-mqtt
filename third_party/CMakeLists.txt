# ------------------------------------------------------------------------------
# CMakeLists.txt for aggresss/playground-mqtt/third_party
# ------------------------------------------------------------------------------

include(ExternalProject)

option(WITH_IOTC "Build with google iotc" OFF)
if(WITH_IOTC)
  ExternalProject_Add(external-iotc
    SOURCE_DIR ${IOTC_PREFIX}
    URL https://github.com/GoogleCloudPlatform/iot-device-sdk-embedded-c/archive/v1.0.1.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    )
    set(IOTC_PREFIX ${PROJECT_BINARY_DIR}/third_party/iotc PARENT_SCOPE)
endif(WITH_IOTC)

option(WITH_LIBEVENT "Build with libevent" OFF)
if(WITH_LIBEVENT)
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
    set(LIBEVENT_PREFIX ${PROJECT_BINARY_DIR}/third_party/libevent PARENT_SCOPE)
endif(WITH_LIBEVENT)

option(WITH_OPENSSL "Build with openssl" OFF)
if(WITH_OPENSSL)
  ExternalProject_Add(external-openssl
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
    INSTALL_COMMAND make install
    TEST_COMMAND ""
    )
    set(OPENSSL_PREFIX ${PROJECT_BINARY_DIR}/third_party/openssl PARENT_SCOPE)
endif(WITH_OPENSSL)

option(WITH_MBEDTLS "Build with mbedtls" OFF)
if(WITH_MBEDTLS)
  ExternalProject_Add(external-mbedtls
    URL https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.16.1.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./scripts/config.pl set MBEDTLS_THREADING_C &&
                      ./scripts/config.pl set MBEDTLS_THREADING_PTHREAD
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/third_party/mbedtls
      -DCMAKE_TOOLCHAIN_FILE:PATH=${TOOLCHAIN_FILE}
      -DCMAKE_BUILD_TYPE:STRING=Debug
      -DENABLE_TESTING:BOOL=OFF
      -DENABLE_PROGRAMS:BOOL=ON
    TEST_COMMAND ""
    )
    set(MBEDTLS_PREFIX ${PROJECT_BINARY_DIR}/third_party/mbedtls PARENT_SCOPE)
endif(WITH_MBEDTLS)

option(WITH_WOLFSSL "Build with wolfssl" OFF)
if(WITH_WOLFSSL)
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
    set(WOLFSSL_PREFIX ${PROJECT_BINARY_DIR}/third_party/wolfssl PARENT_SCOPE)
endif(WITH_WOLFSSL)

option(WITH_DEPS "Build with deps" OFF)
if(WITH_DEPS)
  ExternalProject_Add(external-deps
    SOURCE_DIR ${PROJECT_BINARY_DIR}/third_party/deps
    GIT_REPOSITORY https://github.com/aggresss/deps.git
    GIT_PROGRESS TRUE
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    )
    set(DEPS_PREFIX ${PROJECT_BINARY_DIR}/third_party/deps PARENT_SCOPE)
endif(WITH_DEPS)

option(WITH_PAHO "Build with eclipse paho" OFF)
if(WITH_PAHO)
  ExternalProject_Add(external-paho
    DEPENDS external-openssl
    URL https://github.com/eclipse/paho.mqtt.c/archive/v1.3.0.tar.gz
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/third_party/paho
      -DCMAKE_TOOLCHAIN_FILE:PATH=${TOOLCHAIN_FILE}
      -DCMAKE_BUILD_TYPE:STRING=Debug
      -DPAHO_WITH_SSL:BOOL=TRUE
      -DOPENSSL_SEARCH_PATH:PATH=${PROJECT_BINARY_DIR}/third_party/openssl
      -DPAHO_BUILD_DOCUMENTATION:BOOL=FALSE
      -DPAHO_BUILD_SAMPLES:BOOL=TRUE
      -DPAHO_ENABLE_TESTING:BOOL=FALSE
      -DPAHO_BUILD_STATIC:BOOL=TRUE
    TEST_COMMAND ""
    )
    set(PAHO_PREFIX ${PROJECT_BINARY_DIR}/third_party/paho PARENT_SCOPE)
endif(WITH_PAHO)
