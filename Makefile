build: 
	./scripts/build.sh -f linux -t linux

win:
	./scripts/build.sh -f windows -t windows

run:
	./game

linux-win:
	./scripts/build.sh -f linux -t windows

release: build
	./scripts/release.sh

release-linux-win: linux-win
	./scripts/release.sh -w

debug: build-debug
	gdb ./game

debug-win: build-debug-win
	gdb ./game.exe

build-debug: 
	./scripts/build.sh -f linux -t linux -g

build-debug-win:
	./scripts/build.sh -f windows -t windows -g

raylib:
	cd ./raylib-5.0/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

