cmake_minimum_required(VERSION 3.15)
project (http_twitter)

#//////////////////////////
#asio
#//////////////////////////

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_ADDRESSOF)
add_definitions(-DASIO_HAS_STD_ARRAY)
add_definitions(-DASIO_HAS_CSTDINT)
add_definitions(-DASIO_HAS_STD_SHARED_PTR)
add_definitions(-DASIO_HAS_STD_TYPE_TRAITS)
add_definitions(-DASIO_HAS_VARIADIC_TEMPLATES)
add_definitions(-DASIO_HAS_STD_FUNCTION)
add_definitions(-DASIO_HAS_STD_CHRONO)
add_definitions(-DBOOST_ALL_NO_LIB)
if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0501)
  add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

#//////////////////////////
#openssl
#//////////////////////////

if (APPLE)
  set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR} /usr/local/Cellar/openssl@1.1/1.1.1h/)
  set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
  set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
endif()

if (MSVC)
  set(OPENSSL_ROOT_DIR G:/wx/ssl)
  set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
  set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.lib)
  set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.lib)
endif()

if(UNIX AND NOT APPLE)
  find_package(OpenSSL REQUIRED)
endif()

include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(asio-1.22.2/include)
include_directories(json-3.10.5/single_include)

set(src ${src})
set(src ${src} ssl_read.hh)
set(src ${src} ssl_read.cc)
set(src ${src} http_twitter.cc)

add_executable(http_twitter ${src} http_twitter.cc)

if (MSVC)
  target_link_libraries (http_twitter ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} CRYPT32.LIB)
else()
  target_link_libraries (http_twitter ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
endif()
