game: 
	./build.sh

run:
	./build.sh && ./dist/game

raylib:
	cd ./raylib/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

