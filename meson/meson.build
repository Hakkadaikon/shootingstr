# project setting
# -----------------------------------------------------------------
project(
  'shootingstr',
  'c',
  default_options : [
    'warning_level=3',
  ]
)
CC='gcc'
# -----------------------------------------------------------------

# yyjson install
# -----------------------------------------------------------------
cmake = import('cmake')
yyjson_subproject = cmake.subproject('yyjson')
yyjson_dep = yyjson_subproject.dependency('yyjson')
# -----------------------------------------------------------------

# libwebsockets install
# -----------------------------------------------------------------
libwebsockets_subproject = cmake.subproject(
  'libwebsockets',
  cmake_options: [
    'CMAKE_BUILD_TYPE=Release',
    'LWS_WITH_SSL=ON',
    'LWS_WITH_ZLIB=ON'
  ]
)
libwebsockets_dep = libwebsockets_subproject.dependency('websockets')
# -----------------------------------------------------------------

sources = ['../src/main.c']

executable(
  'shootingstr',
  sources,
  link_args : [
    '-lwebsockets',
    '-lssl',
    #'--no-as-needed',
    '-lcap'
  ],
  dependencies: [yyjson_dep, libwebsockets_dep],
  install: false)
# -----------------------------------------------------------------
