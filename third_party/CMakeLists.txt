# ------------------------------------------------------------------------------
# CMakeLists.txt for aggresss/playground-mqtt/third_party
# ------------------------------------------------------------------------------

include(ExternalProject)

if(WITH_PAHO)
  ExternalProject_Add(external-paho
    BUILD_ALWAYS TRUE
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
endif(WITH_PAHO)

if(WITH_IOTC)
  ExternalProject_Add(external-iotc
    BUILD_ALWAYS TRUE
    SOURCE_DIR ${IOTC_PREFIX}
    URL https://github.com/GoogleCloudPlatform/iot-device-sdk-embedded-c/archive/v1.0.1.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    )
endif(WITH_IOTC)

if(WITH_LIBEVENT)
  ExternalProject_Add(external-libevent
    BUILD_ALWAYS TRUE
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
        --prefix=${LIBEVENT_PREFIX}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    TEST_COMMAND ""
    )
endif(WITH_LIBEVENT)

if(WITH_OPENSSL)
  ExternalProject_Add(external-openssl
    SOURCE_DIR ${OPENSSL_PREFIX}
    BUILD_ALWAYS TRUE
    BUILD_IN_SOURCE TRUE
    URL https://github.com/openssl/openssl/archive/OpenSSL_1_1_1b.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND
      ./Configure
      -fPIC
      no-asm
      linux-generic32
      --prefix=${OPENSSL_PREFIX}
      --cross-compile-prefix=${C_COMPILER_PREFIX}
      no-shared
    BUILD_COMMAND make
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    )
endif(WITH_OPENSSL)

if(WITH_MBEDTLS)
  ExternalProject_Add(external-mbedtls
    BUILD_ALWAYS TRUE
    URL https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.16.1.tar.gz
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${MBEDTLS_PREFIX}
      -DCMAKE_TOOLCHAIN_FILE:PATH=${TOOLCHAIN_FILE}
      -DCMAKE_BUILD_TYPE:STRING=Debug
      -DENABLE_TESTING:BOOL=OFF
      -DENABLE_PROGRAMS:BOOL=OFF
    TEST_COMMAND ""
    )
endif(WITH_MBEDTLS)

if(WITH_WOLFSSL)
  ExternalProject_Add(external-wolfSSL
    BUILD_ALWAYS TRUE
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
        --prefix=${WOLFSSL_PREFIX}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    TEST_COMMAND ""
    )
endif(WITH_WOLFSSL)
