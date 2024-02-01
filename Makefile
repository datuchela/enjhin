build: 
	./build.sh

run:
	./build.sh && ./dist/game

raylib:
	cd ./lib/raylib/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

