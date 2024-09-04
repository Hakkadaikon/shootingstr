# project setting
# -----------------------------------------------------------------
project(
  'shootingstr',
  'c',
  default_options : [
    'warning_level=3',
    'c_std=c23'
  ]
)
# -----------------------------------------------------------------

# yyjson install
# -----------------------------------------------------------------
cmake = import('cmake')
yyjson_options = cmake.subproject_options()
yyjson_options.add_cmake_defines({
  'CMAKE_C_COMPILER': 'gcc'
})

yyjson_subproject = cmake.subproject('yyjson', options: yyjson_options)
yyjson_dep = yyjson_subproject.dependency('yyjson')
# -----------------------------------------------------------------

# # openssl install
# # -----------------------------------------------------------------
# libopenssl_options = cmake.subproject_options()
# libopenssl_options.add_cmake_defines({
#   'BUILD_OPENSSL': 'ON',
#   'OPENSSL_ENABLE_TESTS': 'OFF',
#   'OPENSSL_BUILD_VERSION': '3.3.1',
#   'CMAKE_CXX_COMPILER': '/usr/bin/g++',
#   'CMAKE_C_COMPILER': '/usr/bin/gcc'
# })
# #libopenssl_subproject = cmake.subproject('openssl-cmake', options: libopenssl_options)
# libopenssl_subproject = cmake.subproject('openssl-cmake', options: libopenssl_options)
# libopenssl_dep = libopenssl_subproject.dependency('openssl-cmake')
# # -----------------------------------------------------------------

# libwebsockets install
# -----------------------------------------------------------------
# libwebsockets_options = cmake.subproject_options()
# libwebsockets_options.add_cmake_defines({
#   'CMAKE_BUILD_TYPE': 'Release',
#   'CMAKE_C_COMPILER': 'gcc',
#   'LWS_WITH_SSL': 'ON',
#   'LWS_WITHOUT_TESTAPPS': 'ON',
#   'LWS_WITHOUT_TEST_SERVER': 'ON',
#   'LWS_WITHOUT_TEST_SERVER_EXTPOLL': 'ON',
#   'LWS_WITHOUT_TEST_CLIENT': 'ON',
#   'LWS_WITHOUT_TEST_PING': 'ON',
#   'LWS_WITHOUT_EXTENSIONS': 'ON',
#   'LWS_WITH_STATIC': 'ON',
#   'LWS_WITH_SHARED': 'OFF',
#   'LWS_STATIC_PIC': 'ON',
#   'LWS_INSTALL_LIB_DIR' : '/usr/local/lib/libwebsockets',
#   'LWS_INSTALL_INCLUDE_DIR' : '/usr/include/libwebsockets',
# })
# 
# libwebsockets_subproject = cmake.subproject(
#   'libwebsockets',
#   options: libwebsockets_options
# )
# libwebsockets_dep = libwebsockets_subproject.dependency('websockets')
# -----------------------------------------------------------------

# rocksdb install
# -----------------------------------------------------------------
#rocksdb_subproject = cmake.subproject(
#  'rocksdb',
#  cmake_options: [
#    'build_by_default=false',
#    '-DCMAKE_BUILD_TYPE=Release',
#    '-DBENCHMARK_ENABLE_GTEST_TESTS=0',
#    '-DWITH_GFLAGS=0',
#    '-DROCKSDB_BUILD_SHARED=0'
#  ],
#)
#librocksdb_dep = rocksdb_subproject.dependency('rocksdb')
# -----------------------------------------------------------------

# executable
# -----------------------------------------------------------------
executable(
  'shootingstr',
  sources : [
    '../src/main.c',
    '../src/websockets/websockets.c',
    '../src/nostr/nostr.c',
    '../src/nostr/nostr_storage.c'
  ],
  c_args : [
    '-Wno-unused-parameter',
    '-O3'
  ],
  link_args : [
    '-lwebsockets',
    #'-no-as-needed',
    '-lssl',
    '-lcap',
    '-lrocksdb'
  ],
  dependencies : [
  #  libwebsockets_dep,
  #  libopenssl_dep,
  #  librocksdb_dep,
     yyjson_dep
  ],
  install: false
)
# -----------------------------------------------------------------
