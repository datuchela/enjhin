build: 
	./build.sh

debug: 
	./build.sh && ./dist/game

run:
	./dist/game

raylib:
	cd ./lib/raylib/src && \
		make clean && \
		make PLATFORM=PLATFORM_DESKTOP

