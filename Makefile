SOURCE_FILES_PATH := ./src/raygui.c ./src/debug.c ./src/fizziks.c ./src/main.c

# Builds binary with debugging information on Linux
debug: $(SOURCE_FILES_PATH)
	./scripts/build.sh -f linux -t linux -g

# Builds binary without debugging information on Linux
release: $(SOURCE_FILES_PATH)
	./scripts/build.sh -f linux -t linux

# Builds Windows executable (.exe) on Linux
linux-win-release: $(SOURCE_FILES_PATH)
	./scripts/build.sh -f linux -t windows

# Builds binary with debugging information on Windows
win-debug: $(SOURCE_FILES_PATH)
	./scripts/build.sh -f windows -t windows -g

# Builds binary without debugging information on Windows
win-release: $(SOURCE_FILES_PATH)
	./scripts/build.sh -f windows -t windows

# Builds and packages up the binary with assets for Linux on Linux
package: release
	./scripts/package.sh

# Builds and packages up the binary with assets for Windows on Linux
linux-win-package: linux-win-release
	./scripts/package.sh -w

raylib:
	cd ./raylib-5.0/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

