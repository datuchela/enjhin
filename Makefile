build: 
	./scripts/build.sh -f linux -t linux

linux-win:
	./scripts/build.sh -f linux -t windows

win:
	./scripts/build.sh -f windows -t windows

release: build
	./scripts/release.sh

release-linux-win: linux-win
	./scripts/release.sh -w

build-debug: 
	./scripts/build.sh -f linux -t linux -g

build-debug-win:
	./scripts/build.sh -f windows -t windows -g

debug: build-debug
	gdb ./game

debug-win: build-debug-win
	gdb ./game.exe

run:
	./game

run-win:
	./game.exe

raylib:
	cd ./raylib-5.0/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

