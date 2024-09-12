###############################################################################
# File        : Malefile
# Description : Makefile for example
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY: clean setup build docker-prune docker-build format

clean:
	@rm -rf \
		build \
		meson/subprojects/yyjson \
		meson/subprojects/libwebsockets \
		meson/subprojects/openssl-cmake

setup: clean
	meson setup meson build

build:
	ninja -C build

docker-prune:
	yes | docker system prune --volumes

docker-build:
	DOCKER_BUILDKIT=1 docker build --progress=plain .

# format (use clang)
format:
	@clang-format -i \
		-style="{    \
			BasedOnStyle: Google,                      \
			AlignConsecutiveAssignments: true,         \
			AlignConsecutiveDeclarations: true,        \
			ColumnLimit: 0,                            \
			IndentWidth: 4,                            \
			AllowShortFunctionsOnASingleLine: None,    \
			AllowShortLoopsOnASingleLine: false,       \
			BreakBeforeBraces: Linux,                  \
			SortIncludes: true,                        \
			DerivePointerAlignment: false,             \
			AlignConsecutiveMacros: true,              \
			PointerAlignment: Left,                    \
			AlignOperands: true,                       \
		}"                                                 \
		$(shell find src/ -name '*.cpp' -o -name '*.hpp') \
		$(shell find src/ -name '*.c' -o -name '*.h'    )
