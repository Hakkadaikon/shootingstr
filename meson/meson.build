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
libwebsockets_dep = dependency('libwebsockets')
# -----------------------------------------------------------------

sources = ['../src/main.c']

executable(
  'shootingstr',
  sources,
  c_args : [
    '-Wno-unused-parameter'
  ],
  link_args : [
    '-lwebsockets',
    #'-no-as-needed',
    '-lssl',
    '-lcap'
  ],
  dependencies: [yyjson_dep, libwebsockets_dep],
  install: false
)
# -----------------------------------------------------------------
