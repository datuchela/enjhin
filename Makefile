game: 
	./bin/build.sh

run:
	./bin/build.sh && ./dist/game

raylib:
	cd ./raylib/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

