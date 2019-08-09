cmake_minimum_required (VERSION 3.5)

project (BitcoinFun)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

message("Building Bitcoin Fun")
message("------------------------------- Options -------------------------------")

# Build Type: -DCMAKE_BUILD_TYPE=Debug|Release
# Default: Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message("-- Build Type: ${CMAKE_BUILD_TYPE} (-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})")
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_CXX_FLAGS "-g -O0 -Wall")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  set(CMAKE_CXX_FLAGS "-g -O2 -Wall")
ELSE()
  set(CMAKE_CXX_FLAGS "-g -Wall")
ENDIF()

# set C++ standard
set(CMAKE_CXX_STANDARD 14)
# stop building after the first error
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdelete-non-virtual-dtor -Wfatal-errors")

# The number of concurrent jobs when compiling a third-party library
if(JOBS)
  message("-- Concurrent Jobs: ${JOBS} (-DJOBS=${JOBS})")
  set(MAKE_JOBS "-j${JOBS}") 
else()
  message("-- Concurrent Jobs: 1 (-DJOBS=1)")
  set(MAKE_JOBS "") 
endif()

############################## Chain Source #########################
# Bitcoin Core
set(CHAIN_NAME "Bitcoin Core")

# add chain type definitions
add_definitions(-DCHAIN_TYPE_${CHAIN_TYPE})
add_definitions(-DCHAIN_TYPE_STR="${CHAIN_TYPE}")

# add a macro that needed by chain's source code
add_definitions(-DHAVE_CONFIG_H)

# Whether to enable the wallet when building the blockchain code

option(CHAIN_ENABLE_WALLET "Enable wallet when building chain code" OFF)
if(CHAIN_ENABLE_WALLET)
  message("-- Wallet of Chain Code Enabled (-DCHAIN_ENABLE_WALLET=ON)")
else()
  message("-- Wallet of Chain Code Disabled (-DCHAIN_ENABLE_WALLET=OFF)")
endif()

# Root Dir of Chain's Source Code
if(NOT CHAIN_SRC_ROOT)
  message(FATAL_ERROR "Root dir of chain's source code not defined! (-DCHAIN_SRC_ROOT=<path>)")
endif()
message("-- Root Dir of ${CHAIN_NAME}: ${CHAIN_SRC_ROOT} (-DCHAIN_SRC_ROOT=${CHAIN_SRC_ROOT})")
if (NOT EXISTS "${CHAIN_SRC_ROOT}/src/version.h")
  message(FATAL_ERROR "The source code of ${CHAIN_NAME} not exists!")
endif()

######################### install & package ###################################

# Debian/Ubuntu software package
if(GENERATE_DEB_PACKAGE)
  message("-- Generate Debian/Ubuntu software package: Enabled (-DGENERATE_DEB_PACKAGE=ON)")
else()
  message("-- Generate Debian/Ubuntu software package: Disabled (-DGENERATE_DEB_PACKAGE=OFF)")
endif()

###################################### Libs ######################################
message("------------------------------- Libs -------------------------------")

set(BITCOIN_LIBRARIES ${BITCOIN_LIBRARIES}
    "${CHAIN_SRC_ROOT}/src/libbitcoin_common.a"
    "${CHAIN_SRC_ROOT}/src/libbitcoin_consensus.a"
    "${CHAIN_SRC_ROOT}/src/libbitcoin_util.a"
    "${CHAIN_SRC_ROOT}/src/crypto/libbitcoin_crypto_base.a")
  
list(GET BITCOIN_LIBRARIES 0 BITCOIN_LIBRARIE)
if (NOT EXISTS ${BITCOIN_LIBRARIE})
  message(STATUS "building ${CHAIN_NAME}...")

  execute_process(WORKING_DIRECTORY "${CHAIN_SRC_ROOT}" COMMAND ./autogen.sh)

  if (NOT CHAIN_ENABLE_WALLET)
    execute_process(WORKING_DIRECTORY "${CHAIN_SRC_ROOT}" COMMAND ./configure --with-gui=no --disable-wallet --disable-tests --disable-bench ${CHAIN_EXTRA_FLAGS})
  else()
    execute_process(WORKING_DIRECTORY "${CHAIN_SRC_ROOT}" COMMAND ./configure --with-gui=no --disable-tests --disable-bench ${CHAIN_EXTRA_FLAGS})
  endif()

  execute_process(WORKING_DIRECTORY "${CHAIN_SRC_ROOT}" COMMAND make ${MAKE_JOBS})

endif()

foreach(BITCOIN_LIBRARIE ${BITCOIN_LIBRARIES})
  if (NOT EXISTS ${BITCOIN_LIBRARIE})
    message(FATAL_ERROR "${BITCOIN_LIBRARIE} not exists!")
  endif()
endforeach()

# libsecp256k1
set(secp256k1_DIR "${CHAIN_SRC_ROOT}/src/secp256k1")
set(secp256k1_LIBRARIES "${secp256k1_DIR}/.libs/libsecp256k1.a")
if (NOT EXISTS ${secp256k1_LIBRARIES})
  message(STATUS "build secp256k1...")
  execute_process(WORKING_DIRECTORY "${secp256k1_DIR}" COMMAND ./autogen.sh)
  execute_process(WORKING_DIRECTORY "${secp256k1_DIR}" COMMAND ./configure --enable-module-recovery)
  execute_process(WORKING_DIRECTORY "${secp256k1_DIR}" COMMAND make ${MAKE_JOBS})
endif()
if (NOT EXISTS ${secp256k1_LIBRARIES})
  message(FATAL_ERROR "build secp256k1 failed!")
endif()

# other libraries
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibConfig REQUIRED)
find_package(Glog REQUIRED)
find_package(libzmq REQUIRED)
find_package(CURL REQUIRED)
find_package(LibEvent REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost 1.47.0 COMPONENTS thread filesystem system regex program_options chrono REQUIRED)
find_package(LibGMP REQUIRED)
find_package(LibPthread REQUIRED)


###################################### Building Tools ######################################

# Find binutils
message("-- CMAKE_OBJCOPY: ${CMAKE_OBJCOPY}")
if(NOT CMAKE_OBJCOPY)
  message("-- CMAKE_OBJCOPY not found, debug info will not separated from targets")
  mark_as_advanced(CMAKE_OBJCOPY)
endif()

########################## Generate version number #############################
# The base version
# The update of this version number may not be timely.
# We will get the version number from the git tag.
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "1")

set(VERSION_STR "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("-- version string: ${VERSION_STR}")

# package version of cpack
SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")


###################################### Targets ######################################

add_definitions(-DFMT_HEADER_ONLY)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/src
  src
  test
  ${CHAIN_SRC_ROOT}/src
  ${CHAIN_SRC_ROOT}/src/config
  ${CHAIN_SRC_ROOT}/src/secp256k1/include
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${LIBZMQ_INCLUDE_DIR}
  ${GLOG_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
  ${LIBEVENT_INCLUDE_DIR})

set(THIRD_LIBRARIES ${BITCOIN_LIBRARIES} ${secp256k1_LIBRARIES} ${GLOG_LIBRARIES} ${ZLIB_LIBRARIES} ${LIBZMQ_LIBRARIES} ${Hiredis_LIBRARIES} ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${LIBCONFIGPP_LIBRARY}
                    ${LIBEVENT_LIB} ${LIBEVENT_PTHREADS_LIB} ${GMP_LIBRARIES} ${LIBEVENT_OPENSSL_LIB} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY}
                    ${PTHREAD_LIBRARIES} ${PROTOBUF_LIBRARIES})

file(GLOB LIB_SOURCES src/*.cc)

add_library(
 funlibs STATIC
 ${LIB_SOURCES})

set(FUN_SOURCES src/fun/main.cc)
add_executable(bitcoin-fun ${FUN_SOURCES})
target_link_libraries(bitcoin-fun funlibs ${THIRD_LIBRARIES})

###################################### Install & Package ######################################
message("------------------------------- Install & Package -------------------------------")

# `make install` support

# targets
set(INSTALL_TARGETS bitcoin-fun)

# tmp dir for install & packet
set(PACKAGE_TMP_DIR ${CMAKE_BINARY_DIR}/package_tmp)

install(CODE "execute_process(WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/install.sh)" COMPONENT main)

# separate debug info from targets
if(CMAKE_OBJCOPY)
  foreach(tgt ${INSTALL_TARGETS})
    add_custom_command(TARGET ${tgt} POST_BUILD
      COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:${tgt}> $<TARGET_FILE:${tgt}>.dbg
      COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:${tgt}>
      COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:${tgt}>.dbg $<TARGET_FILE:${tgt}>)
  endforeach()
endif()

# install targets , dirs & files
foreach(tgt ${INSTALL_TARGETS})
  # binary
  install(PROGRAMS     ${CMAKE_BINARY_DIR}/${tgt}
          COMPONENT   main
          DESTINATION /usr/local/bitcoinfun/bin)
endforeach()

# documents
install(FILES
            LICENSE
        COMPONENT
            main
        DESTINATION
            /usr/local/bitcoinfun)


# install debug info
if(CMAKE_OBJCOPY)
  foreach(tgt ${INSTALL_TARGETS})
    install(FILES       ${CMAKE_BINARY_DIR}/${tgt}.dbg
            COMPONENT   dbginfo
            DESTINATION /usr/local/bitcoinfun/bin)
  endforeach()
endif()

# generate deb package by CPack
if(GENERATE_DEB_PACKAGE)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME "bitcoinfun")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "StefanLiu stefanliu@outlook.com")

  message("-- Package Name: ${CPACK_PACKAGE_NAME}")

  # components
  set(CPACK_DEB_COMPONENT_INSTALL ON)
  set(CPACK_DEB_USE_DISPLAY_NAME_IN_FILENAME ON)
  set(CPACK_DEBIAN_MAIN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
  set(CPACK_DEBIAN_DBGINFO_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-dbg)

  # dependencies
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

  include(CPack)
endif()
